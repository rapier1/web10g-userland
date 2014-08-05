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

struct estats_error*
estats_nl_client_init(struct estats_nl_client** cl)
{
	estats_error *err = NULL;
	struct mnl_socket *sock;
	int id;
	int ret;
	int i;

	ErrIf(cl == NULL, ESTATS_ERR_INVAL);
	*cl = NULL;

	Chk(Malloc((void**) cl, sizeof(estats_nl_client)));
	memset((void*) *cl, 0, sizeof(estats_nl_client));

	sock = mnl_socket_open(NETLINK_GENERIC);
	ErrIf(sock == NULL, ESTATS_ERR_NOLINK);

	ret = mnl_socket_bind(sock, 0, MNL_SOCKET_AUTOPID);
	ErrIf(ret == -1, ESTATS_ERR_NOLINK);

	(*cl)->mnl_sock = sock;

	ret = resolve_genladdr("tcp_estats", *cl);
	ErrIf(ret, ESTATS_ERR_NOLINK);

	(*cl)->mask.masks[0] = DEFAULT_PERF_MASK;
	(*cl)->mask.masks[1] = DEFAULT_PATH_MASK;
	(*cl)->mask.masks[2] = DEFAULT_STACK_MASK;
	(*cl)->mask.masks[3] = DEFAULT_APP_MASK;
	(*cl)->mask.masks[4] = DEFAULT_TUNE_MASK;
	(*cl)->mask.masks[5] = DEFAULT_EXTRAS_MASK;

	for (i = 0; i < MAX_TABLE; i++) {
		(*cl)->mask.if_mask[i] = 0;
	}

 Cleanup:
        return err;
}

void
estats_nl_client_destroy(struct estats_nl_client** cl)
{
	if (cl == NULL || *cl == NULL)
		return;

	mnl_socket_close((*cl)->mnl_sock);
	
	free(*cl);

	*cl = NULL;
}

struct estats_error*
estats_nl_client_set_mask(struct estats_nl_client* cl, struct estats_mask* mask)
{
	estats_error* err = NULL;
	int i;

	ErrIf(cl == NULL || mask == NULL, ESTATS_ERR_INVAL);

	cl->mask = *mask;

 Cleanup:
 	return err;
}

struct estats_error*
estats_nl_client_get_sock(struct mnl_socket** nl, const estats_nl_client* cl)
{
	estats_error* err = NULL;

	ErrIf(nl == NULL || cl == NULL, ESTATS_ERR_INVAL);

	*nl = cl->mnl_sock;

 Cleanup:
 	return err;
}

struct estats_error*
estats_nl_client_get_fam_id(int* id, const estats_nl_client* cl)
{
	estats_error* err = NULL;

	ErrIf(id == NULL || cl == NULL, ESTATS_ERR_INVAL);

	*id = cl->fam_id;

 Cleanup:
 	return err;
}
