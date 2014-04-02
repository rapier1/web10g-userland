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

static int parse_attr_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = (const struct nlattr **)data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, CTRL_ATTR_MAX) < 0) {
		dbgprintf("mnl_attr_type_valid");
		return MNL_CB_ERROR;
	}
	switch(type) {
	case CTRL_ATTR_FAMILY_ID:
		if (mnl_attr_validate(attr, MNL_TYPE_U16) < 0) {
			dbgprintf("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	case CTRL_ATTR_MCAST_GROUPS:
		if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
			dbgprintf("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	}
	tb[type] = attr;

	return MNL_CB_OK;
}

static int data_cb(const struct nlmsghdr *nlh, void *data)
{
	struct nlattr **tb = (struct nlattr **)data;
	struct genlmsghdr *genl = mnl_nlmsg_get_payload(nlh);

	mnl_attr_parse(nlh, sizeof(*genl), parse_attr_cb, tb);

	return MNL_CB_OK;
}

int resolve_genladdr(const char *name, struct estats_client* cl)
{
	struct mnl_socket *sock = cl->mnl_sock;
	char buf[MNL_SOCKET_BUFFER_SIZE];

	struct nlmsghdr *nlh;
	struct genlmsghdr *genl;
	int ret;
	unsigned int seq, portid;
	struct nlattr *tb[CTRL_ATTR_MAX+1] = {};
        int fmly_id = 0;
        const char *mcast_grp_name;
	int mcast_grp_id = 0;
	struct nlattr *pos;

	nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type	= GENL_ID_CTRL;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
	nlh->nlmsg_seq = seq = time(NULL);

	genl = mnl_nlmsg_put_extra_header(nlh, sizeof(struct genlmsghdr));
	genl->cmd = CTRL_CMD_GETFAMILY;
	genl->version = 0;

	mnl_attr_put_strz(nlh, CTRL_ATTR_FAMILY_NAME, name);
	portid = mnl_socket_get_portid(sock);

	if (mnl_socket_sendto(sock, nlh, nlh->nlmsg_len) < 0) {
		dbgprintf("mnl_socket_send");
		return -1;
	}

	while (1) {
		ret = mnl_socket_recvfrom(sock, buf, sizeof(buf));
		if (ret == -1) {
			dbgprintf("mnl_socket_recvfrom");
			break;
		}
		ret = mnl_cb_run(buf, ret, seq, portid, data_cb, tb);
		if (ret == -1) {
			dbgprintf("mnl_cb_run");
			break;
		} else if (ret == 0)
			break;
	}
	if (tb[CTRL_ATTR_FAMILY_ID]) {
                fmly_id = mnl_attr_get_u16(tb[CTRL_ATTR_FAMILY_ID]);
	}

	cl->fam_id = fmly_id;

	return 0;
}

