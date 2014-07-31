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
		else stat_val[j].masked = 1;
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

static void parse_4tuple_list(struct nlattr *nested, struct estats_connection_list *cli)
{
        struct nlattr *tb[NEA_4TUPLE_MAX+1];
        struct nlattr *attr;
	struct estats_connection* cp = NULL;
	struct list_head* conn_head;

	uint8_t rem_addr[16];
	uint8_t local_addr[16];
        uint16_t rem_port = 0;
        uint16_t local_port = 0;
	uint8_t addr_type = 0;
        int cid = 0;

        mnl_attr_parse_nested(nested, parse_4tuple_cb, tb);

        if (tb[NEA_LOCAL_ADDR]) {
		memcpy(&local_addr[0], mnl_attr_get_payload(tb[NEA_LOCAL_ADDR]), 16);
        }
        if (tb[NEA_REM_ADDR]) {
		memcpy(&rem_addr[0], mnl_attr_get_payload(tb[NEA_REM_ADDR]), 16);
        }
        if (tb[NEA_LOCAL_PORT]) {
                local_port = mnl_attr_get_u16(tb[NEA_LOCAL_PORT]);
        }
        if (tb[NEA_REM_PORT]) {
                rem_port = mnl_attr_get_u16(tb[NEA_REM_PORT]);
        }
	if (tb[NEA_ADDR_TYPE]) {
		addr_type = mnl_attr_get_u8(tb[NEA_ADDR_TYPE]);
	}
	else printf("No addrtype from kernel\n");
        if (tb[NEA_CID]) {
                cid = mnl_attr_get_u32(tb[NEA_CID]);
        }

	if (cid > 0) {
		conn_head = &(cli->connection_head);

		cp = malloc(sizeof(estats_connection));
		memset((void*) cp, 0, sizeof(estats_connection_list));
		if (cp == NULL) {
			dbgprintf("No mem; malloc failed");
			return;
		}

		memcpy(&(cp->rem_addr[0]), &rem_addr[0], 16);
		memcpy(&(cp->local_addr[0]), &local_addr[0], 16);
		cp->rem_port = rem_port;
		cp->local_port = local_port;
		cp->addr_type = addr_type;
		cp->cid = cid;

		list_add_tail(conn_head, &cp->list);
        	cp = NULL;
        }
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

static int data_cb(const struct nlmsghdr *nlh, void *data)
{
  //fprintf(stderr, "DEBUG: data_cb(): called.\n");
        struct nlattr *tb[NLE_ATTR_MAX+1] = {};
        struct genlmsghdr *genl = mnl_nlmsg_get_payload(nlh);
        //fprintf(stderr, "DEBUG: data_cb(): genlmsghdr->cmd: %c.\n", genl->cmd);
	struct estats_connection_list *cli;

	mnl_attr_parse(nlh, sizeof(*genl), data_attr_cb, tb);

        if (tb[NLE_ATTR_4TUPLE]) {
		if (data != NULL) {
			cli = (struct estats_connection_list*) data;
			parse_4tuple_list(tb[NLE_ATTR_4TUPLE], cli);
		}
		else
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
          default :
            printf("DEBUG: XXX parse_table_var_name_cb(): unknown type: %d.\n", type);
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
            printf("DEBUG: XXX parse_var_cb(): unknown type: %d.\n", type);
	}
	// XXX tb[type] = attr;  // XXX Broken, we just keep rewriting over, so need to pass in index!

	return MNL_CB_OK;
}

#if 0  // XXX Deprecated!
static void parse_table_var_name(struct nlattr* nested, int index)
{
  struct nlattr *tb[NEA_TIME_MAX+1];  // XXX Wrong!
	
        const char* name_ptr = NULL;
	uint32_t type = 0;

        printf("DEBUG: XXX parse_table_var_name(): Working on index (table): %d.\n", index);

	mnl_attr_parse_nested(nested, parse_table_var_name_cb, tb);

	if (tb[NEA_VAR_NAME]) {
                name_ptr = mnl_attr_get_str(tb[NEA_VAR_NAME]);
	}
	if (tb[NEA_VAR_TYPE]) {
		type = mnl_attr_get_u32(tb[NEA_VAR_TYPE]);
	}

        printf("DEBUG: XXX parse_table_var_name(): Got %s, type: %d.\n", name_ptr, type);

  /*
        struct nlattr *tb_perf[PERF_INDEX_MAX+1]   = {};
        struct nlattr *tb_path[PATH_INDEX_MAX+1]   = {};
        struct nlattr *tb_stack[STACK_INDEX_MAX+1] = {};
        struct nlattr *tb_app[APP_INDEX_MAX+1]     = {};
        struct nlattr *tb_tune[TUNE_INDEX_MAX+1]   = {};
        struct nlattr *tb_extras[EXTRAS_INDEX_MAX+1] = {};

        struct index_attr ia = { .index = index };
        int i, j;

          printf("DEBUG: XXX parse_table(): index: %d.\n", index);
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

        printf("DEBUG: XXX parse_table(): max_index[%d]: %d.\n", index, max_index[index]);
        for (i = 0; i < max_index[index]; i++) {

		j = single_index(index, i);
		
                if (ia.tb[i]) {

                  printf("DEBUG: XXX parse_table(): setting val[%d] union to tb[%d].\n", j, i);
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
		else stat_val[j].masked = 1;
        }
  */
}
#endif

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
                        printf("Performance Table:\n");
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

        /*  XXX Deprecated.
        if (tb[NLE_ATTR_NUM_TABLES])
          parse_num_tables(tb[NLE_ATTR_NUM_TABLES], NULL);
        if (tb[NLE_ATTR_NUM_VARS])
          parse_num_vars(tb[NLE_ATTR_NUM_VARS], NULL);
        if (tb[NLE_ATTR_PERF_VARS])
                parse_table_var_name(tb[NLE_ATTR_PERF_VARS], PERF_TABLE);
        if (tb[NLE_ATTR_PATH_VARS])
                parse_table_var_name(tb[NLE_ATTR_PATH_VARS], PATH_TABLE);
        if (tb[NLE_ATTR_STACK_VARS])
                parse_table_var_name(tb[NLE_ATTR_STACK_VARS], STACK_TABLE);
        if (tb[NLE_ATTR_APP_VARS])
                parse_table_var_name(tb[NLE_ATTR_APP_VARS], APP_TABLE);
        if (tb[NLE_ATTR_TUNE_VARS])
                parse_table_var_name(tb[NLE_ATTR_TUNE_VARS], TUNE_TABLE);
        if (tb[NLE_ATTR_EXTRAS_VARS])
                parse_table_var_name(tb[NLE_ATTR_EXTRAS_VARS], EXTRAS_TABLE);
        */

        return MNL_CB_OK;
}

struct estats_error*
estats_list_conns(estats_connection_list* cli, const estats_nl_client* cl)
{
	estats_error* err = NULL;
	struct mnl_socket* nl;
	int fam_id; 
	int ret;

	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	struct genlmsghdr *genl;
	unsigned int seq, portid;

	struct list_head* conn_head;
	struct estats_list* conn_pos;
	struct estats_list* list_pos;
	estats_connection* cp = NULL;
	estats_connection* conn;
	estats_connection* tmp;

	ErrIf(cli == NULL, ESTATS_ERR_INVAL);

	conn_head = &(cli->connection_head);

	list_for_each_safe(conn_head, conn, tmp, list) {
		list_del(&conn->list);
		free(conn);
	}

	nl = cl->mnl_sock;
	fam_id = cl->fam_id;
	portid = mnl_socket_get_portid(nl);

	nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type = fam_id;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
	nlh->nlmsg_seq = seq = time(NULL);
	genl = mnl_nlmsg_put_extra_header(nlh, sizeof(struct genlmsghdr));

 	genl->cmd = TCPE_CMD_LIST_CONNS;

	ret = mnl_socket_sendto(nl, nlh, nlh->nlmsg_len);

	Err2If(ret == -1, ESTATS_ERR_GENL, "mnl_socket_send");

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
          /*
          fprintf(stderr, "DEBUG: XXX estats_list_conns(): ret: on entering loop. %d.\n", ret);
          const struct nlmsghdr* nl_msghdr = (void*)buf;
if (nl_msghdr->nlmsg_type >= NLMSG_MIN_TYPE) {
          fprintf(stderr, "DEBUG: XXX estats_list_conns(): test to see if we call data_cb() passed.\n");
} else
  fprintf(stderr, "DEBUG: XXX estats_list_conns(): nlmsg_type (%d) < NLMSG_MIN_TYPE (%d).\n", nl_msghdr->nlmsg_type, NLMSG_MIN_TYPE);
          */
		ret = mnl_cb_run(buf, ret, seq, portid, data_cb, cli);
                //fprintf(stderr, "DEBUG: XXX estats_list_conns(): After call to mnl_cb_run(), ret: %d.\n", ret);
		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}

	Err2If(ret == -1, ESTATS_ERR_GENL, "error");

 Cleanup:
 	return err;
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

        if (cl->mask.if_mask[0]) mnl_attr_put_u64(nlh, NEA_PERF_MASK, cl->mask.masks[0]);
        if (cl->mask.if_mask[1]) mnl_attr_put_u64(nlh, NEA_PATH_MASK, cl->mask.masks[1]);
        if (cl->mask.if_mask[2]) mnl_attr_put_u64(nlh, NEA_STACK_MASK, cl->mask.masks[2]);
        if (cl->mask.if_mask[3]) mnl_attr_put_u64(nlh, NEA_APP_MASK, cl->mask.masks[3]);
        if (cl->mask.if_mask[4]) mnl_attr_put_u64(nlh, NEA_TUNE_MASK, cl->mask.masks[4]);
        if (cl->mask.if_mask[5]) mnl_attr_put_u64(nlh, NEA_EXTRAS_MASK, cl->mask.masks[5]);

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

//	Chk(estats_connection_tuple_copy(&data->tuple, &stat_tuple));

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
          /*
          fprintf(stderr, "DEBUG: XXX estats_get_mib(): ret: %d.\n", ret);
          const struct nlmsghdr* nl_msghdr = (void*)buf;
if (nl_msghdr->nlmsg_type >= NLMSG_MIN_TYPE) {
          fprintf(stderr, "DEBUG: XXX estats_list_conns(): test to see if we call data_cb() passed.\n");
} else
  fprintf(stderr, "DEBUG: XXX estats_list_conns(): nlmsg_type (%d) < NLMSG_MIN_TYPE (%d).\n", nl_msghdr->nlmsg_type, NLMSG_MIN_TYPE);
          */
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
