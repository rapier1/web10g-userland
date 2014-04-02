/*
 * Copyright (c) 2012 The Board of Trustees of the University of Illinois,
 *                    Carnegie Mellon University.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 */
#include <estats/estats-int.h>

static struct estats_val stat_val[TOTAL_INDEX_MAX];

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

	if (mnl_attr_type_valid(attr, max_index[tblnum]) < 0) {
		dbgprintf("mnl_attr_type_valid");
		return MNL_CB_ERROR;
	}

	j = single_index(tblnum, type);

	switch(estats_var_array[j].type) {

	case TCPE_UNSIGNED8:
		if (mnl_attr_validate(attr, MNL_TYPE_U8) < 0) {
			dbgprintf("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	case TCPE_UNSIGNED16:
		if (mnl_attr_validate(attr, MNL_TYPE_U16) < 0) {
			dbgprintf("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	case TCPE_UNSIGNED32:
	case TCPE_SIGNED32:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
			dbgprintf("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	case TCPE_UNSIGNED64:
		if (mnl_attr_validate(attr, MNL_TYPE_U64) < 0) {
			dbgprintf("mnl_attr_validate");
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

        struct index_attr ia = { .index = index };
        int i, j;

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
        }

        mnl_attr_parse_nested(nested, parse_table_cb, &ia);

        for (i = 0; i < max_index[index]; i++) {

		j = single_index(index, i);
		
                if (ia.tb[i]) {

			switch(estats_var_array[j].type) {

                        case TCPE_UNSIGNED64: 
				stat_val[j].uv64 = mnl_attr_get_u64(ia.tb[i]);
                                break;
                        case TCPE_UNSIGNED32:
				stat_val[j].uv32 = mnl_attr_get_u32(ia.tb[i]);
                                break;
                        case TCPE_SIGNED32:
				stat_val[j].sv32 = (int32_t) mnl_attr_get_u32(ia.tb[i]); 
                                break;
                        case TCPE_UNSIGNED16:
				stat_val[j].uv16 = mnl_attr_get_u16(ia.tb[i]);
                                break;
                        case TCPE_UNSIGNED8:
				stat_val[j].uv8 = mnl_attr_get_u8(ia.tb[i]);
                                break;
                        default:
                                break;
                        }
                }
		else stat_val[j].mask = 1;
        }
}

static int parse_4tuple_cb(const struct nlattr *attr, void *data)
{
        const struct nlattr **tb = (const struct nlattr **)data;
        int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NEA_4TUPLE_MAX) < 0) {
		perror("mnl_attr_type_valid");
		return MNL_CB_ERROR;
	}

	switch(type) {
	case NEA_REM_ADDR:
		if (mnl_attr_validate(attr, MNL_TYPE_BINARY) < 0) {
			perror("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	case NEA_LOCAL_ADDR:
		if (mnl_attr_validate(attr, MNL_TYPE_BINARY) < 0) {
			perror("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	case NEA_REM_PORT:
		if (mnl_attr_validate(attr, MNL_TYPE_U16) < 0) {
			perror("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	case NEA_LOCAL_PORT:
		if (mnl_attr_validate(attr, MNL_TYPE_U16) < 0) {
			perror("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	}
	tb[type] = attr;

        return MNL_CB_OK;
}

static void parse_4tuple(struct nlattr *nested, struct estats_client *cl)
{
        struct nlattr *tb[NEA_4TUPLE_MAX+1];
        struct nlattr *attr;
	struct estats_connection* cp = NULL;
	struct estats_list* conn_head;

	uint8_t rem_addr[17];
	uint8_t local_addr[17];
        uint16_t rem_port = 0;
        uint16_t local_port = 0;
        int cid = 0;

        mnl_attr_parse_nested(nested, parse_4tuple_cb, tb);

        if (tb[NEA_LOCAL_ADDR]) {
		memcpy(&local_addr[0], mnl_attr_get_payload(tb[NEA_LOCAL_ADDR]), 17);
        }
        if (tb[NEA_REM_ADDR]) {
		memcpy(&rem_addr[0], mnl_attr_get_payload(tb[NEA_REM_ADDR]), 17);
        }
        if (tb[NEA_LOCAL_PORT]) {
                local_port = mnl_attr_get_u16(tb[NEA_LOCAL_PORT]);
        }
        if (tb[NEA_REM_PORT]) {
                rem_port = mnl_attr_get_u16(tb[NEA_REM_PORT]);
        }
        if (tb[NEA_CID]) {
                cid = mnl_attr_get_u32(tb[NEA_CID]);
        }

	if (cid > 0) {
		conn_head = &(cl->connection_list_head);

		cp = malloc(sizeof(estats_connection));
		if (cp == NULL) {
			dbgprintf("No mem; malloc failed");
			return;
		}

		memcpy(&(cp->rem_addr[0]), &rem_addr[0], 17);
		memcpy(&(cp->local_addr[0]), &local_addr[0], 17);
		cp->rem_port = rem_port;
		cp->local_port = local_port;
		cp->cid = cid;
		
		_estats_list_add_tail(&(cp->list), conn_head);
        	cp = NULL;
        }
}

static int data_attr_cb(const struct nlattr *attr, void *data)
{
        const struct nlattr **tb = data;
        int type = mnl_attr_get_type(attr);

        if (mnl_attr_type_valid(attr, NLE_ATTR_MAX) < 0)
                return MNL_CB_OK;

        switch(type) {
        case NLE_ATTR_4TUPLE:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_4TUPLE");
                        return MNL_CB_ERROR;
                }
                break;
        case NLE_ATTR_PERF:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_PERF");
                        return MNL_CB_ERROR;
                }
                break;
        case NLE_ATTR_PATH:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_PATH");
                        return MNL_CB_ERROR;
                }
                break;
        case NLE_ATTR_STACK:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_STACK");
                        return MNL_CB_ERROR;
                }
                break;
        case NLE_ATTR_APP:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_APP");
                        return MNL_CB_ERROR;
                }
                break;
        case NLE_ATTR_TUNE:
                if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
                        dbgprintf("mnl_attr_validate NLE_ATTR_TUNE");
                        return MNL_CB_ERROR;
                }
                break;
        }
        tb[type] = attr;
        return MNL_CB_OK;
}

static int data_cb(const struct nlmsghdr *nlh, void *data)
{
        struct nlattr *tb[NLE_ATTR_MAX+1] = {};
        struct genlmsghdr *genl = mnl_nlmsg_get_payload(nlh);
	struct estats_client *cl = (struct estats_client*) data;

	mnl_attr_parse(nlh, sizeof(*genl), data_attr_cb, tb);

        if (tb[NLE_ATTR_4TUPLE])
                parse_4tuple(tb[NLE_ATTR_4TUPLE], cl);
        if (tb[NLE_ATTR_PERF])
                parse_table(tb[NLE_ATTR_PERF], PERF_TABLE);
        if (tb[NLE_ATTR_PATH])
                parse_table(tb[NLE_ATTR_PATH], PATH_TABLE);
        if (tb[NLE_ATTR_STACK])
                parse_table(tb[NLE_ATTR_STACK], STACK_TABLE);
        if (tb[NLE_ATTR_APP])
                parse_table(tb[NLE_ATTR_APP], APP_TABLE);
        if (tb[NLE_ATTR_TUNE])
                parse_table(tb[NLE_ATTR_TUNE], TUNE_TABLE);
 
        return MNL_CB_OK;
}

struct estats_error*
estats_list_conns(estats_client* cl, estats_connection_func func)
{
	estats_error* err = NULL;
	struct mnl_socket* nl;
	int fam_id; 
	int ret;

	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	struct genlmsghdr *genl;

	unsigned int seq, portid;

	struct estats_list* conn_head;
	struct estats_list* conn_pos;
	struct estats_list* list_pos;
	struct estats_list* tmp;
	estats_connection* cp = NULL;

	conn_head = &(cl->connection_list_head);

    	TCPE_LIST_FOREACH_SAFE(conn_pos, tmp, conn_head) {
        	estats_connection* curr_conn = TCPE_LIST_ENTRY(conn_pos, estats_connection, list);
        	_estats_list_del(conn_pos);
        	free(curr_conn);
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

	Err2If(ret == -1, TCPE_ERR_GENL, "mnl_socket_send");

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, seq, portid, data_cb, cl);
		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}

	Err2If(ret == -1, TCPE_ERR_GENL, "error");

	TCPE_LIST_FOREACH(list_pos, &(cl->connection_list_head)) {
		estats_connection* cp = TCPE_LIST_ENTRY(list_pos, estats_connection, list);
		struct estats_connection_tuple* ct = (struct estats_connection_tuple*) cp;
		func(ct);
	}

 Cleanup:
 	return err;
}

struct estats_error*
estats_read_vars(struct estats_data* data, int cid, const estats_client* cl)
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

	ErrIf(cid < 1 || cl == NULL, TCPE_ERR_INVAL);

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
	Err2If(!attrp, TCPE_ERR_GENL, "attr_nest_start failure");

        mnl_attr_put_u32(nlh, NEA_CID, cid);

        mnl_attr_nest_end(nlh, attrp);

        attrp = mnl_attr_nest_start_check(nlh, getpagesize(), NLE_ATTR_MASK);
	Err2If(!attrp, TCPE_ERR_GENL, "attr_nest_start failure");

        if (cl->mask.if_mask[0]) mnl_attr_put_u64(nlh, NEA_PERF_MASK, cl->mask.masks[0]);
        if (cl->mask.if_mask[1]) mnl_attr_put_u64(nlh, NEA_PATH_MASK, cl->mask.masks[1]);
        if (cl->mask.if_mask[2]) mnl_attr_put_u64(nlh, NEA_STACK_MASK, cl->mask.masks[2]);
        if (cl->mask.if_mask[3]) mnl_attr_put_u64(nlh, NEA_APP_MASK, cl->mask.masks[3]);
        if (cl->mask.if_mask[4]) mnl_attr_put_u64(nlh, NEA_TUNE_MASK, cl->mask.masks[4]);

        mnl_attr_nest_end(nlh, attrp);

	ret = mnl_socket_sendto(nl, nlh, nlh->nlmsg_len);

	Err2If(ret == -1, TCPE_ERR_GENL, "mnl_socket_send");

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, seq, portid, data_cb, NULL);
		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}

	if (ret == -1) {
		printf("%s\n", strerror(errno));
		Err2(TCPE_ERR_GENL, "mnl_cb_run error");
	}

	for (k = 0; k < TOTAL_INDEX_MAX; k++) {
		data->val[k] = stat_val[k];
	}

 Cleanup:
	return err;
}

struct estats_error*
estats_write_var(const char* varname, uint32_t val, int cid, const estats_client* cl)
{
	estats_error* err = NULL;
	struct mnl_socket* nl;
	int fam_id; 
	int ret;

	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	struct genlmsghdr *genl;
	struct nlattr *attrp;

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
	Err2If(!attrp, TCPE_ERR_GENL, "attr_nest_start failure");

        mnl_attr_put_u32(nlh, NEA_CID, cid);

        mnl_attr_nest_end(nlh, attrp);

        attrp = mnl_attr_nest_start_check(nlh, getpagesize(), NLE_ATTR_WRITE);
	Err2If(!attrp, TCPE_ERR_GENL, "attr_nest_start failure");

	mnl_attr_put_str(nlh, NEA_WRITE_VAR, varname);
        mnl_attr_put_u32(nlh, NEA_WRITE_VAL, val);

        mnl_attr_nest_end(nlh, attrp);

	ret = mnl_socket_sendto(nl, nlh, nlh->nlmsg_len);
	Err2If(ret == -1, TCPE_ERR_GENL, "mnl_socket_send");

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	Err2If(ret == -1, TCPE_ERR_GENL, "mnl_socket_recvfrom");

	ret = mnl_cb_run(buf, ret, seq, portid, NULL, NULL);

	if (ret == -1) {
		printf("%s\n", strerror(errno));
		Err2(TCPE_ERR_GENL, "mnl_cb_run error");
	}

 Cleanup:
 	return err;
}
