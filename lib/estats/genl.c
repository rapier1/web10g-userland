/*
 * Copyright (c) 2013 The Board of Trustees of the University of Illinois,
 *                    Carnegie Mellon University.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the MIT License for more details.
 *
 * You should have received a copy of the MIT License along with this library;
 * if not, see http://opensource.org/licenses/MIT.
 *
 */
#include <estats/estats-int.h>

static struct estats_val stat_val[TOTAL_NUM_VARS];
static struct estats_timeval stat_tv;
static struct estats_connection_tuple stat_tuple;
static uint32_t num_tables = 0;
static uint32_t num_vars = 0;

struct index_attr {
        struct nlattr **tb;
        int index;
};

/* data_cb_object - helper struct for data_cb
	(the primary parse function for list_conns and list_conns_vars)
	essentially, this function/list_head grouping
	is an object that knows what list entry to add
	and what global data to access
	when parsing a list_conns or list_conns_vars reply message */
/* data_cb_object method interface */
/* adds global data to new entry in list */
typedef void (*estats_add_from_globals_to_list)(struct list_head *);
/* gets cid of next entry in list, i.e. one past last cid in list
	 (returns -1 if list empty) */
typedef int (*estats_get_next_cid)(struct list_head *, uint32_t */*cid*/);

struct data_cb_object {
	struct list_head *lh;
	estats_add_from_globals_to_list	add_globals_to_list;
	estats_get_next_cid		get_next_cid;
};
typedef struct data_cb_object	data_cb_object;

/* Methods for data_cb_object w/ estats_connection_list */
/* add_from_globals_to_list for connection_list:
	adds global data to new entry in list */
static void
add_globals_to_connection_list(struct list_head *lh) {
	estats_connection* cp = NULL;

	cp = malloc(sizeof(estats_connection));
	if (cp == NULL) {
		dbgprintf("No mem; malloc failed");
		return;
	}
	memset((void*) cp, 0, sizeof(estats_connection_list));

	memcpy(&(cp->rem_addr[0]), &(stat_tuple.rem_addr[0]), 16);
	memcpy(&(cp->local_addr[0]), &(stat_tuple.local_addr[0]), 16);
	cp->rem_port = stat_tuple.rem_port;
	cp->local_port = stat_tuple.local_port;
	cp->addr_type = stat_tuple.addr_type;
	cp->cid = stat_tuple.cid;

	list_add_tail(lh, &cp->list);
}

/* get_next_cid for connection_list:
	gets cid of next entry in list, i.e. one past last cid in list
	 (returns -1 if list empty) */
static int
get_next_cid_from_connection_list(struct list_head *lh, uint32_t *cid) {
	estats_connection* conn;
	conn = list_tail(lh, estats_connection, list);
	if (!conn)
		return -1;
	*cid = conn->cid+1;
	return 0;
}

/* Methods for data_cb_object w/ estats_connection_vars_list */
/* add_from_globals_to_list for connection_vars_list:
	adds global data to new entry in list */
static void
add_globals_to_connection_vars_list(struct list_head *lh) {
	estats_error *err = NULL;
	estats_connection_vars *cp = NULL;
	int k;

	if (err = estats_connection_vars_new(&cp)) {
		dbgprintf("No mem; malloc failed");
		/* this is likely to be the static _OOM_Error,
			but just in case - free before returning */
		estats_error_free(&err);
		return;
	}

	cp->data->tuple = stat_tuple;

	cp->data->tv.sec = stat_tv.sec;
	cp->data->tv.usec = stat_tv.usec;

	for (k = 0; k < TOTAL_NUM_VARS; k++) {
		cp->data->val[k] = stat_val[k];
	}

	list_add_tail(lh, &cp->list);
}

/* get_next_cid for connection_vars_list:
	gets cid of next entry in list, i.e. one past last cid in list
	 (returns -1 if list empty) */
static int
get_next_cid_from_connection_vars_list(struct list_head *lh, uint32_t *cid) {
	estats_connection_vars* conn;
	conn = list_tail(lh, estats_connection_vars, list);
	if (!conn)
		return -1;
	*cid = conn->data->tuple.cid+1;
	return 0;
}

static int parse_table_cb(const struct nlattr *attr, void *data)
{
        struct index_attr *ia = (struct index_attr *)data;
	const struct nlattr **tb = (const struct nlattr **)ia->tb;
	int type = mnl_attr_get_type(attr);
        int tblnum = ia->index;
	int j;

        //fprintf(stderr, "DEBUG: XXX parse_table_cb(): type: %d, tblnum: %d.\n", type, tblnum);

	if (mnl_attr_type_valid(attr, max_index[tblnum]) < 0) {
		dbgprintf("mnl_attr_type_valid max_index[tblnum]\n");
		return MNL_CB_ERROR;
	}

	j = single_index(tblnum, type);

	switch(estats_var_array[j].valtype) {

	case ESTATS_UNSIGNED8:
		if (mnl_attr_validate(attr, MNL_TYPE_U8) < 0) {
			dbgprintf("mnl_attr_validate ESTATS_UNSIGNED8\n");
			return MNL_CB_ERROR;
		}
		break;
	case ESTATS_UNSIGNED16:
		if (mnl_attr_validate(attr, MNL_TYPE_U16) < 0) {
			dbgprintf("mnl_attr_validate ESTATS_UNSIGNED16\n");
			return MNL_CB_ERROR;
		}
		break;
	case ESTATS_UNSIGNED32:
	case ESTATS_SIGNED32:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
			dbgprintf("mnl_attr_validate ESTATS_(UN)SIGNED32\n");
			return MNL_CB_ERROR;
		}
		break;
	case ESTATS_UNSIGNED64:
		if (mnl_attr_validate(attr, MNL_TYPE_U64) < 0) {
			dbgprintf("mnl_attr_validate ESTATS_UNSIGNED64\n");
			return MNL_CB_ERROR;
		}
		break;
	default:
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void parse_table(struct nlattr *nested, int index)
{
        struct nlattr *tb_perf[PERF_INDEX_MAX+1]   = {};
        struct nlattr *tb_path[PATH_INDEX_MAX+1]   = {};
        struct nlattr *tb_stack[STACK_INDEX_MAX+1] = {};
        struct nlattr *tb_app[APP_INDEX_MAX+1]     = {};
        struct nlattr *tb_tune[TUNE_INDEX_MAX+1]   = {};
        struct nlattr *tb_extras[EXTRAS_INDEX_MAX+1] = {};

        struct index_attr ia = { .index = index };
        int i, j;

        //fprintf(stderr, "DEBUG: XXX parse_table(): index: %d.\n", index);
        switch (index) {
        case PERF_TABLE:
                ia.tb = tb_perf;
                break;
        case PATH_TABLE:
                ia.tb = tb_path;
                break;
        case STACK_TABLE:
                ia.tb = tb_stack;
                break;
        case APP_TABLE:
                ia.tb = tb_app;
                break;
        case TUNE_TABLE:
                ia.tb = tb_tune;
                break;
        case EXTRAS_TABLE:
                ia.tb = tb_extras;
                break;
        }

        mnl_attr_parse_nested(nested, parse_table_cb, &ia);

        for (i = 0; i < max_index[index]; i++) {

		j = single_index(index, i);
		
                if (ia.tb[i]) {

			stat_val[j].masked = 0;

			switch(estats_var_array[j].valtype) {

                        case ESTATS_UNSIGNED64: 
				stat_val[j].uv64 = mnl_attr_get_u64(ia.tb[i]);
                                break;
                        case ESTATS_UNSIGNED32:
				stat_val[j].uv32 = mnl_attr_get_u32(ia.tb[i]);
                                break;
                        case ESTATS_SIGNED32:
				stat_val[j].sv32 = (int32_t) mnl_attr_get_u32(ia.tb[i]); 
                                break;
                        case ESTATS_UNSIGNED16:
				stat_val[j].uv16 = mnl_attr_get_u16(ia.tb[i]);
                                break;
                        case ESTATS_UNSIGNED8:
				stat_val[j].uv8 = mnl_attr_get_u8(ia.tb[i]);
                                break;
                        default:
                                break;
                        }
                }
		else
			stat_val[j].masked = 1;
        }
}

static int parse_time_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = (const struct nlattr **)data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NEA_TIME_MAX) < 0) {
		perror("mnl_attr_type_valid NEA_TIME_MAX\n");
		return MNL_CB_ERROR;
	}

	switch(type) {
	case NEA_TIME_SEC:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
			perror("mnl_attr_validate NEA_TIME_SEC\n");
			return MNL_CB_ERROR;
		}
		break;
	case NEA_TIME_USEC:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
			perror("mnl_attr_validate NEA_TIME_USEC\n");
			return MNL_CB_ERROR;
		}
		break;
	}
	tb[type] = attr;

	return MNL_CB_OK;
}

static void parse_time(struct nlattr *nested, void *data)
{
	struct nlattr *tb[NEA_TIME_MAX+1];
	uint32_t sec = 0;
	uint32_t usec = 0;

	mnl_attr_parse_nested(nested, parse_time_cb, tb);

	if (tb[NEA_TIME_SEC]) {
		sec = mnl_attr_get_u32(tb[NEA_TIME_SEC]);
	}
	if (tb[NEA_TIME_USEC]) {
		usec = mnl_attr_get_u32(tb[NEA_TIME_USEC]);
	}

	stat_tv.sec = sec;
	stat_tv.usec = usec;
}

static int parse_4tuple_cb(const struct nlattr *attr, void *data)
{
        const struct nlattr **tb = (const struct nlattr **)data;
        int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NEA_4TUPLE_MAX) < 0) {
		perror("mnl_attr_type_valid NEA_4TUPLE_MAX\n");
		return MNL_CB_ERROR;
	}

	switch(type) {
	case NEA_REM_ADDR:
		if (mnl_attr_validate(attr, MNL_TYPE_BINARY) < 0) {
			perror("mnl_attr_validate MNL_TYPE_BINARY\n");
			return MNL_CB_ERROR;
		}
		break;
	case NEA_LOCAL_ADDR:
		if (mnl_attr_validate(attr, MNL_TYPE_BINARY) < 0) {
			perror("mnl_attr_validate MNL_TYPE_BINARY\n");
			return MNL_CB_ERROR;
		}
		break;
	case NEA_REM_PORT:
		if (mnl_attr_validate(attr, MNL_TYPE_U16) < 0) {
			perror("mnl_attr_validate MNL_TYPE_U16\n");
			return MNL_CB_ERROR;
		}
		break;
	case NEA_LOCAL_PORT:
		if (mnl_attr_validate(attr, MNL_TYPE_U16) < 0) {
			perror("mnl_attr_validate MNL_TYPE_U16\n");
			return MNL_CB_ERROR;
		}
		break;
	}
	tb[type] = attr;

        return MNL_CB_OK;
}

static void parse_4tuple(struct nlattr *nested)
{
  //fprintf(stderr, "DEBUG: parse_4tuple(): called.\n");
        struct nlattr *tb[NEA_4TUPLE_MAX+1];
        struct nlattr *attr;

	uint8_t rem_addr[16];
	uint8_t local_addr[16];
        uint16_t rem_port = 0;
        uint16_t local_port = 0;
        int cid = 0;

        mnl_attr_parse_nested(nested, parse_4tuple_cb, tb);

        if (tb[NEA_LOCAL_ADDR]) {
		memcpy(&stat_tuple.local_addr, mnl_attr_get_payload(tb[NEA_LOCAL_ADDR]), 16);
        }
        if (tb[NEA_REM_ADDR]) {
		memcpy(&stat_tuple.rem_addr, mnl_attr_get_payload(tb[NEA_REM_ADDR]), 16);
        }
        if (tb[NEA_LOCAL_PORT]) {
                stat_tuple.local_port = mnl_attr_get_u16(tb[NEA_LOCAL_PORT]);
        }
        if (tb[NEA_REM_PORT]) {
                stat_tuple.rem_port = mnl_attr_get_u16(tb[NEA_REM_PORT]);
        }
	if (tb[NEA_ADDR_TYPE]) {
		stat_tuple.addr_type = mnl_attr_get_u8(tb[NEA_ADDR_TYPE]);
	}
        if (tb[NEA_CID]) {
                stat_tuple.cid = mnl_attr_get_u32(tb[NEA_CID]);
        }
}

static int data_attr_cb(const struct nlattr *attr, void *data)
{
        const struct nlattr **tb = data;
        int type = mnl_attr_get_type(attr);
        //fprintf(stderr, "DEBUG: data_attr_cb(): type: %d.\n", type);

        if (mnl_attr_type_valid(attr, NLE_ATTR_MAX) < 0)
                return MNL_CB_OK;

        switch(type) {
        case NLE_ATTR_4TUPLE:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_4TUPLE\n");
                        return MNL_CB_ERROR;
                }
                break;
        case NLE_ATTR_TIME:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_TIME\n");
                        return MNL_CB_ERROR;
                }
                break;
        case NLE_ATTR_PERF_VALS:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_PERF\n");
                        return MNL_CB_ERROR;
                }
                break;
        case NLE_ATTR_PATH_VALS:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_PATH\n");
                        return MNL_CB_ERROR;
                }
                break;
        case NLE_ATTR_STACK_VALS:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_STACK\n");
                        return MNL_CB_ERROR;
                }
                break;
        case NLE_ATTR_APP_VALS:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_APP\n");
                        return MNL_CB_ERROR;
                }
                break;
        case NLE_ATTR_TUNE_VALS:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_TUNE\n");
                        return MNL_CB_ERROR;
                }
                break;
        case NLE_ATTR_EXTRAS_VALS:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_EXTRAS\n");
                        return MNL_CB_ERROR;
                }
                break;
          default:
            fprintf(stderr, "DEBUG: data_attr_cb(): unknown type: %d.\n", type);
        }
        tb[type] = attr;

        return MNL_CB_OK;
}

/* NOTE this is not robust against poorly formed replies to commands
      (i.e. replies that are missing required attributes)
*/
static int data_cb(const struct nlmsghdr *nlh, void *data)
{
  //fprintf(stderr, "DEBUG: data_cb(): called.\n");
        struct nlattr *tb[NLE_ATTR_MAX+1] = {};
        struct genlmsghdr *genl = mnl_nlmsg_get_payload(nlh);
        //fprintf(stderr, "DEBUG: data_cb(): genlmsghdr->cmd: %c.\n", genl->cmd);
	data_cb_object *cb_obj = (data_cb_object *)data;

	mnl_attr_parse(nlh, sizeof(*genl), data_attr_cb, tb);

	if (tb[NLE_ATTR_4TUPLE]) {
		parse_4tuple(tb[NLE_ATTR_4TUPLE]);
	}
	if (tb[NLE_ATTR_TIME]) {
		parse_time(tb[NLE_ATTR_TIME], NULL);
	}
	if (tb[NLE_ATTR_PERF_VALS]) {
		parse_table(tb[NLE_ATTR_PERF_VALS], PERF_TABLE);
	}
	if (tb[NLE_ATTR_PATH_VALS]) {
		parse_table(tb[NLE_ATTR_PATH_VALS], PATH_TABLE);
	}
	if (tb[NLE_ATTR_STACK_VALS]) {
		parse_table(tb[NLE_ATTR_STACK_VALS], STACK_TABLE);
	}
	if (tb[NLE_ATTR_APP_VALS]) {
		parse_table(tb[NLE_ATTR_APP_VALS], APP_TABLE);
	}
	if (tb[NLE_ATTR_TUNE_VALS]) {
		parse_table(tb[NLE_ATTR_TUNE_VALS], TUNE_TABLE);
	}
	if (tb[NLE_ATTR_EXTRAS_VALS]) {
		parse_table(tb[NLE_ATTR_EXTRAS_VALS], EXTRAS_TABLE);
	}

	if (cb_obj != NULL && cb_obj->lh != NULL
			&& cb_obj->add_globals_to_list != NULL) {
		cb_obj->add_globals_to_list(cb_obj->lh);
	}
 
	return MNL_CB_OK;
}

/* Routines concerned with simply getting the attribute name (as opposed to value).  This really doesn't ammke a lot of sense, i.e., we simply took John's code for parsing (and storing the vars) and adapted it for get-mib.  In the future, I think I'll rewrite to not even use libmnl for (at least) get-mib.  TODO(aka) */

#if 0  // XXX Deprecated.
static void parse_num_tables(struct nlattr* attr, void* data)
{
  /* Note, since NLE_ATTR_NUM_TABLES is not nested, we don't need a mnl_attr_parse_nested()! */

        num_tables = mnl_attr_get_u32(attr);
}

static void parse_num_vars(struct nlattr* attr, void* data)
{
  /* Note, since NLE_ATTR_NUM_VARS is not nested, we don't need a mnl_attr_parse_nested()! */
        num_vars = mnl_attr_get_u32(attr);
}
#endif

static int parse_table_name_cb(const struct nlattr* attr, void* data)
{
        // XXX const struct nlattr **tb = (const struct nlattr **)data;
	int type = mnl_attr_get_type(attr);
        //fprintf(stderr, "DEBUG: parse_table_name_cb(): working with type: %d.\n", type);

	switch(type) {
	case NEA_VAR_NAME:
		if (mnl_attr_validate(attr, MNL_TYPE_STRING) < 0) {
			perror("mnl_attr_validate NEA_VAR_NAME\n");
			return MNL_CB_ERROR;
		} else {
                        const char* ptr = mnl_attr_get_str(attr);
                        printf("\t%s", ptr);  // note lack of '\n'
                }
		break;
	case NEA_VAR_TYPE:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
			perror("mnl_attr_validate NEA_VAR_TYPE\n");
			return MNL_CB_ERROR;
		} else {
                        type = mnl_attr_get_u32(attr);
                        printf("\t(%d)\n", type);
                }
		break;
	default:
		break;
	}
	// XXX tb[type] = attr;  // XXX Broken, we just keep rewriting over, so need to pass in index!

	return MNL_CB_OK;
}

static int parse_var_cb(const struct nlattr* nested, void* data)
{
        // const struct nlattr **tb = (const struct nlattr **)data;
	int type = mnl_attr_get_type(nested);

        // XXX fprintf(stderr, "DEBUG: XXX parse_var_cb(): working with type: %d.\n", type);

	switch(type) {
        case NLE_ATTR_VAR:
                if (mnl_attr_validate(nested, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_VAR\n");
                        return MNL_CB_ERROR;
                } else {
                        mnl_attr_parse_nested(nested, parse_table_name_cb, data);
                }
                break;
          default :
		  //printf("DEBUG: XXX parse_var_cb(): unknown type: %d.\n", type);
		  break;
	}
	// XXX tb[type] = attr;  // XXX Broken, we just keep rewriting over, so need to pass in index!

	return MNL_CB_OK;
}

static int get_mib_attr_cb(const struct nlattr* attr, void* data)
{
        // XXX const struct nlattr **tb = data;
        int type = mnl_attr_get_type(attr);
        //fprintf(stderr, "DEBUG: get_mib_attr_cb(): working with attr type: %d.\n", type);

        if (mnl_attr_type_valid(attr, NLE_ATTR_MAX) < 0)
                return MNL_CB_OK;

        switch(type) {
        case NLE_ATTR_NUM_TABLES:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_NUM_TABLES\n");
                        return MNL_CB_ERROR;
                } else {
                        num_tables = mnl_attr_get_u32(attr);
                        printf("contains %d table(s) ", num_tables);  // note lack of '\n'
                }
                break;
        case NLE_ATTR_NUM_VARS:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_NUM_VARS\n");
                        return MNL_CB_ERROR;
                } else {
                        num_vars = mnl_attr_get_u32(attr);
                        printf("comprised of %d variables.\n", num_vars);
                }
                break;
        case NLE_ATTR_PERF_VARS:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_PERF\n");
                        return MNL_CB_ERROR;
                } else {
                        printf("Performance Table:\n");
                        mnl_attr_parse_nested(attr, parse_var_cb, NULL);
                }
                break;
        case NLE_ATTR_PATH_VARS:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_PATH\n");
                        return MNL_CB_ERROR;
                } else {
                        printf("Path Table:\n");
                        mnl_attr_parse_nested(attr, parse_var_cb, NULL);
                }
                break;
        case NLE_ATTR_STACK_VARS:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_STACK\n");
                        return MNL_CB_ERROR;
                } else {
                        printf("Stack Table:\n");
                        mnl_attr_parse_nested(attr, parse_var_cb, NULL);
                }
                break;
        case NLE_ATTR_APP_VARS:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_APP\n");
                        return MNL_CB_ERROR;
                } else {
                        printf("Application Table:\n");
                        mnl_attr_parse_nested(attr, parse_var_cb, NULL);
                }
                break;
        case NLE_ATTR_TUNE_VARS:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_TUNE\n");
                        return MNL_CB_ERROR;
                } else {
                        printf("Tune Table:\n");
                        mnl_attr_parse_nested(attr, parse_var_cb, NULL);
                }
                break;
        case NLE_ATTR_EXTRAS_VARS:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_EXTRAS\n");
                        return MNL_CB_ERROR;
                } else {
                        printf("Extras Table:\n");
                        mnl_attr_parse_nested(attr, parse_var_cb, NULL);
                }
                break;
          default :
                dbgprintf("get_mib_attr_cb(): unknown type: %d.\n", type);
        }
        // XXX tb[type] = attr;

        return MNL_CB_OK;
}

static int get_mib_cb(const struct nlmsghdr *nlh, void *data)
{
        struct nlattr *tb[NLE_ATTR_MAX+1] = {};
        struct genlmsghdr *genl = mnl_nlmsg_get_payload(nlh);
	struct estats_connection_list *cli;

        /* get_mib_attr_cb() prints each variable out to stdout as it sees it. */
	mnl_attr_parse(nlh, sizeof(*genl), get_mib_attr_cb, tb);

        return MNL_CB_OK;
}

/* note that this is not robust against a kernel that does not return
    NLE_ATTR_TIMESTAMP in the response from TCPE_CMD_TIMESTAMP. */
static int timestamp_attr_cb(const struct nlattr* attr, void* data)
{
	int type = mnl_attr_get_type(attr);

	switch(type) {
	case NLE_ATTR_TIMESTAMP:
		if (mnl_attr_validate(attr, MNL_TYPE_U64) < 0) {
			dbgprintf("mnl_attr_validate NLE_ATTR_TIMESTAMP\n");
			return MNL_CB_ERROR;
		} else {
			*(uint64_t *)data = mnl_attr_get_u64(attr);
		}
		break;
	default:
		dbgprintf("timestamp_attr_cb(): unknown type: %d.\n", type);
		break;
	}

	return MNL_CB_OK;
}

static int timestamp_cb(const struct nlmsghdr *nlh, void *data)
{
	struct genlmsghdr *genl = mnl_nlmsg_get_payload(nlh);
	if (genl->cmd != TCPE_CMD_TIMESTAMP)
		return MNL_CB_ERROR;
	return mnl_attr_parse(nlh, sizeof(*genl), timestamp_attr_cb, data);
}

/* retrieves timestamp representing <time now> minus ms_delta milliseconds */
struct estats_error*
estats_get_timestamp(uint64_t *timestamp, uint32_t ms_delta,
			const estats_nl_client* cl)
{
	estats_error* err = NULL;
	struct mnl_socket* nl;
	int fam_id; 
	int ret;

	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	struct genlmsghdr *genl;
	size_t seq, portid;

	ErrIf(timestamp == NULL, ESTATS_ERR_INVAL);

	nl = cl->mnl_sock;
	fam_id = cl->fam_id;

	portid = mnl_socket_get_portid(nl);

	nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type = fam_id;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
	nlh->nlmsg_seq = seq = time(NULL);
	genl = mnl_nlmsg_put_extra_header(nlh, sizeof(struct genlmsghdr));

	genl->cmd = TCPE_CMD_TIMESTAMP;

	if (ms_delta>0)
		mnl_attr_put_u32(nlh, NLE_ATTR_TIMESTAMP_DELTA, ms_delta);

	ret = mnl_socket_sendto(nl, nlh, nlh->nlmsg_len);

	Err2If(ret == -1, ESTATS_ERR_GENL, "mnl_socket_send");

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
          //fprintf(stderr, "DEBUG: XXX estats_get_timestamp(): ret: %d.\n", ret);
		ret = mnl_cb_run(buf, ret, seq, portid, timestamp_cb, timestamp);
		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}

	if (ret == -1) {
		printf("%s\n", strerror(errno));
		Err2(ESTATS_ERR_GENL, "mnl_cb_run error");
	}
Cleanup:
	return err;
}

/*
 _list_conns_vars_common_:
	this is the "core" messaging loop of both list_conns and list_conns_vars.

	"cmd" is the appropriate netlink command (TCPE_CMD_xxx)
	"cb_obj" is used to encapsulate the appropriate list and
		methods for interacting with the list.
	"filter" and "timestamp" are used to filter which connections are
		returned by last active timestamp
	"cl" encapsulates the netlink socket and relevant parameters
*/
static struct estats_error*
_list_conns_vars_common_(uint8_t cmd, data_cb_object *cb_obj,
			bool filter, uint64_t timestamp,
			const estats_nl_client* cl)
{
	estats_error* err = NULL;
	struct mnl_socket* nl;
	int fam_id;
	int ret;

	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	struct genlmsghdr *genl;
	size_t seq, portid;
	struct nlattr *cid_attr;
	uint32_t cid, next_cid;

	struct timeval time_s; 
	time_t seconds;
	time_t microsecs;

	gettimeofday(&time_s, NULL);
	seconds = time_s.tv_sec;
	microsecs = time_s.tv_usec;

	ErrIf(cb_obj == NULL || cb_obj->lh == NULL
		|| cb_obj->add_globals_to_list == NULL
		|| cb_obj->get_next_cid == NULL, ESTATS_ERR_INVAL);
	
	nl = cl->mnl_sock;
	fam_id = cl->fam_id;
	portid = mnl_socket_get_portid(nl);
		
	seq = (seconds * 1000000 + microsecs);
	cid = 0;

	while (1) {
		nlh = mnl_nlmsg_put_header(buf);
	
		nlh->nlmsg_type = fam_id;
		nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
		nlh->nlmsg_seq = ++seq;
		genl = mnl_nlmsg_put_extra_header(nlh, sizeof(struct genlmsghdr));
	
		genl->cmd = cmd;

		if (filter)
			mnl_attr_put_u64(nlh, NLE_ATTR_TIMESTAMP, timestamp);

		cid_attr = mnl_attr_nest_start(nlh, NLE_ATTR_4TUPLE);
		mnl_attr_put_u32(nlh, NEA_CID, cid);
		mnl_attr_nest_end(nlh, cid_attr);
	
		ret = mnl_socket_sendto(nl, nlh, nlh->nlmsg_len);
	
		Err2If(ret == -1, ESTATS_ERR_GENL, "mnl_socket_send");
	
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));

		while (ret > 0) {
			ret = mnl_cb_run(buf, ret, seq, portid, data_cb, cb_obj);
			if (ret <= 0)
				break;
			ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
		}
		Err2If(ret == -1, ESTATS_ERR_GENL, "error");
		if (cb_obj->get_next_cid(cb_obj->lh, &next_cid) < 0)
			/* break if list is empty... */
			break;
		if (next_cid == cid)
			/* break if we got no new connections this time */
			break;
		cid = next_cid;
	}
	
Cleanup:
 	return err;
}

/*
 _list_conns_common:
	sets up the parameters of the "core" messaging loop for the
		command TCPE_CMD_LIST_CONNS

	"filter" and "timestamp" are used to filter which connections are
		returned by last active timestamp
	"cl" encapsulates the netlink socket and relevant parameters
*/
static struct estats_error*
_list_conns_common(estats_connection_list* cli, bool filter, uint64_t timestamp,
			const estats_nl_client* cl)
{
	estats_error* err = NULL;

	struct list_head* conn_head;
	estats_connection* conn;
	estats_connection* tmp;

	data_cb_object cb_obj;

	ErrIf(cli == NULL, ESTATS_ERR_INVAL);

	conn_head = &(cli->connection_head);

	list_for_each_safe(conn_head, conn, tmp, list) {
		list_del(&conn->list);
		free(conn);
	}

	cb_obj.lh = conn_head;
	cb_obj.add_globals_to_list = add_globals_to_connection_list;
	cb_obj.get_next_cid = get_next_cid_from_connection_list;

	return _list_conns_vars_common_(TCPE_CMD_LIST_CONNS, &cb_obj,
					filter, timestamp, cl);
Cleanup:
	return err;
}

/* frees connection_list entries, then
	fills connection_list with list of all existing connections */
struct estats_error*
estats_list_conns(estats_connection_list* cli, const estats_nl_client* cl)
{
	return _list_conns_common(cli, false, 0, cl);
}

/* frees connection_list entries, then
	fills connection_list with list of all existing connections
	which have seen activity since the given timestamp */
struct estats_error*
estats_list_conns_active_since(
	estats_connection_list* cli, uint64_t timestamp,
	const estats_nl_client* cl)
{
	return _list_conns_common(cli, true, timestamp, cl);
}

/* frees connection_list entries, then
	fills connection_list with list of all existing connections
	which have seen activity within ms_delta (in ms) of <now> */
struct estats_error*
estats_list_conns_active_within(
	estats_connection_list* cli, uint32_t ms_delta,
	const estats_nl_client* cl)
{
	uint64_t timestamp;
	estats_error *err = NULL;
	err = estats_get_timestamp(&timestamp, ms_delta, cl);
	if (err != NULL)
		return err;
	return _list_conns_common(cli, true, timestamp, cl);
}

/*
 _list_conns_vars_common: (NOTE - distinct from _list_conns_vars_common_)
	sets up the parameters of the "core" messaging loop for the
		command TCPE_CMD_READ_ALL

	"filter" and "timestamp" are used to filter which connections are
		returned by last active timestamp
	"cl" encapsulates the netlink socket and relevant parameters
*/
static struct estats_error*
_list_conns_vars_common(estats_connection_vars_list* cli, bool filter, uint64_t timestamp,
			const estats_nl_client* cl)
{
	estats_error* err = NULL;

	struct list_head* conn_head;
	estats_connection_vars* conn;
	estats_connection_vars* tmp;

	data_cb_object cb_obj;

	ErrIf(cli == NULL, ESTATS_ERR_INVAL);

	conn_head = &(cli->connection_vars_head);

	list_for_each_safe(conn_head, conn, tmp, list) {
		list_del(&conn->list);
		estats_connection_vars_free(&conn);
	}

	cb_obj.lh = conn_head;
	cb_obj.add_globals_to_list = add_globals_to_connection_vars_list;
	cb_obj.get_next_cid = get_next_cid_from_connection_vars_list;

	return _list_conns_vars_common_(TCPE_CMD_READ_ALL, &cb_obj,
					filter, timestamp, cl);
Cleanup:
	return err;
}

/* frees connection_vars_list entries, then
	fills connection_vars_list with
	list of all existing connections (with vars) */
struct estats_error*
estats_list_conns_vars(estats_connection_vars_list* cli, const estats_nl_client* cl)
{
	return _list_conns_vars_common(cli, false, 0, cl);
}

/* frees connection_vars_list entries, then
	fills connection_vars_list with
	list of all existing connections (with vars)
	which have seen activity since the given timestamp */
struct estats_error*
estats_list_conns_vars_active_since(
	estats_connection_vars_list* cli, uint64_t timestamp,
	const estats_nl_client* cl)
{
	return _list_conns_vars_common(cli, true, timestamp, cl);
}

/* frees connection_vars_list entries, then
	fills connection_vars_list with
	list of all existing connections (with vars)
	which have seen activity within ms_delta (in ms) of <now> */
struct estats_error*
estats_list_conns_vars_active_within(
	estats_connection_vars_list* cli, uint32_t ms_delta,
	const estats_nl_client* cl)
{
	uint64_t timestamp;
	estats_error *err = NULL;
	err = estats_get_timestamp(&timestamp, ms_delta, cl);
	if (err != NULL)
		return err;
	return _list_conns_vars_common(cli, true, timestamp, cl);
}

struct estats_error*
estats_read_vars(struct estats_val_data* data, int cid, const estats_nl_client* cl)
{
	estats_error* err = NULL;
	struct mnl_socket* nl;
	int fam_id; 
	int ret;
	int k;

	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	struct genlmsghdr *genl;
	struct nlattr *attrp;

	unsigned int seq, portid;

	ErrIf(cid < 1 || cl == NULL, ESTATS_ERR_INVAL);

	nl = cl->mnl_sock;
	fam_id = cl->fam_id;

	portid = mnl_socket_get_portid(nl);

	nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type = fam_id;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
	nlh->nlmsg_seq = seq = time(NULL);
	genl = mnl_nlmsg_put_extra_header(nlh, sizeof(struct genlmsghdr));

	genl->cmd = TCPE_CMD_READ_VARS;

        attrp = mnl_attr_nest_start_check(nlh, getpagesize(), NLE_ATTR_4TUPLE);
	Err2If(!attrp, ESTATS_ERR_GENL, "attr_nest_start failure");

        mnl_attr_put_u32(nlh, NEA_CID, cid);

        mnl_attr_nest_end(nlh, attrp);

        attrp = mnl_attr_nest_start_check(nlh, getpagesize(), NLE_ATTR_MASK);
	Err2If(!attrp, ESTATS_ERR_GENL, "attr_nest_start failure");
	
	if (cl->mask.if_mask[0])
		mnl_attr_put_u64(nlh, NEA_PERF_MASK, cl->mask.masks[0]);
        if (cl->mask.if_mask[1])
		mnl_attr_put_u64(nlh, NEA_PATH_MASK, cl->mask.masks[1]);
        if (cl->mask.if_mask[2])
		mnl_attr_put_u64(nlh, NEA_STACK_MASK, cl->mask.masks[2]);
        if (cl->mask.if_mask[3])
		mnl_attr_put_u64(nlh, NEA_APP_MASK, cl->mask.masks[3]);
        if (cl->mask.if_mask[4])
		mnl_attr_put_u64(nlh, NEA_TUNE_MASK, cl->mask.masks[4]);
        if (cl->mask.if_mask[5])
		mnl_attr_put_u64(nlh, NEA_EXTRAS_MASK, cl->mask.masks[5]);

        mnl_attr_nest_end(nlh, attrp);

	ret = mnl_socket_sendto(nl, nlh, nlh->nlmsg_len);

	Err2If(ret == -1, ESTATS_ERR_GENL, "mnl_socket_send");

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
          //fprintf(stderr, "DEBUG: XXX estats_read_var(): ret: %d.\n", ret);
		ret = mnl_cb_run(buf, ret, seq, portid, data_cb, NULL);
		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}

	if (ret == -1) {
		printf("%s\n", strerror(errno));
		Err2(ESTATS_ERR_GENL, "mnl_cb_run error");
	}

	data->tuple = stat_tuple;

	data->tv.sec = stat_tv.sec;
	data->tv.usec = stat_tv.usec;

	for (k = 0; k < TOTAL_NUM_VARS; k++) {
		data->val[k] = stat_val[k];
	}


 Cleanup:
	return err;
}

struct estats_error*
estats_write_var(const char* varname, uint32_t val, int cid, const estats_nl_client* cl)
{
	estats_error* err = NULL;
	struct mnl_socket* nl;
	int fam_id; 
	int ret;

	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr* nlh;
	struct genlmsghdr* genl;
	struct nlattr* attrp;

	unsigned int seq, portid;

	nl = cl->mnl_sock;
	fam_id = cl->fam_id;

	portid = mnl_socket_get_portid(nl);

	nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type = fam_id;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
	nlh->nlmsg_seq = seq = time(NULL);
	genl = mnl_nlmsg_put_extra_header(nlh, sizeof(struct genlmsghdr));

	genl->cmd = TCPE_CMD_WRITE_VAR;

        attrp = mnl_attr_nest_start_check(nlh, getpagesize(), NLE_ATTR_4TUPLE);
	Err2If(!attrp, ESTATS_ERR_GENL, "attr_nest_start failure");

        mnl_attr_put_u32(nlh, NEA_CID, cid);

        mnl_attr_nest_end(nlh, attrp);

        attrp = mnl_attr_nest_start_check(nlh, getpagesize(), NLE_ATTR_WRITE);
	Err2If(!attrp, ESTATS_ERR_GENL, "attr_nest_start failure");

	mnl_attr_put_str(nlh, NEA_WRITE_VAR, varname);
        mnl_attr_put_u32(nlh, NEA_WRITE_VAL, val);

        mnl_attr_nest_end(nlh, attrp);

	ret = mnl_socket_sendto(nl, nlh, nlh->nlmsg_len);
	Err2If(ret == -1, ESTATS_ERR_GENL, "mnl_socket_send");

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	Err2If(ret == -1, ESTATS_ERR_GENL, "mnl_socket_recvfrom");

	ret = mnl_cb_run(buf, ret, seq, portid, NULL, NULL);

	if (ret == -1) {
		printf("%s\n", strerror(errno));
		Err2(ESTATS_ERR_GENL, "mnl_cb_run error");
	}

 Cleanup:
 	return err;
}

/* NOTE - data is unused here.  Should it be removed?? */
struct estats_error*
estats_get_mib(struct estats_val_data* data, const estats_nl_client* cl)
{
	estats_error* err = NULL;
	struct mnl_socket* nl;
	int fam_id; 
	int ret;
	int k;

	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	struct genlmsghdr *genl;
	struct nlattr *attrp;

	unsigned int seq, portid;

	ErrIf(cl == NULL, ESTATS_ERR_INVAL);

	nl = cl->mnl_sock;
	fam_id = cl->fam_id;

	portid = mnl_socket_get_portid(nl);

	nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type = fam_id;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
	nlh->nlmsg_seq = seq = time(NULL);
	genl = mnl_nlmsg_put_extra_header(nlh, sizeof(struct genlmsghdr));

	genl->cmd = TCPE_CMD_INIT;

	ret = mnl_socket_sendto(nl, nlh, nlh->nlmsg_len);

	Err2If(ret == -1, ESTATS_ERR_GENL, "mnl_socket_sendto");

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
        if (ret > 0)
                printf("TCP Extended Statistics (RFC 4898) ");  // note lack of '\n'
	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, seq, portid, get_mib_cb, NULL);
		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}
#if 0
	Err2If(ret == -1, ESTATS_ERR_GENL, "error");
#else
	if (ret == -1) {
		printf("%s\n", strerror(errno));
		Err2(ESTATS_ERR_GENL, "mnl_cb_run error");
	}
#endif

 Cleanup:
	return err;
}
