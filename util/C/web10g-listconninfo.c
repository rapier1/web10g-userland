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
#include "scripts.h"

int main(int argc, char **argv)
{

	struct estats_error* err = NULL;
	struct estats_nl_client* cl = NULL;
	struct estats_connection_list* clist = NULL;
	struct estats_connection_info* ci;
	struct estats_connection_tuple_ascii tuple_ascii = { 0 };

	Chk(estats_nl_client_init(&cl));
	Chk(estats_connection_list_new(&clist));

	printf("%-8s %-12s %-8s %-8s %-20s %-8s %-20s %-8s\n", "CID", "cmdline", "PID", "UID", "LocalAddr", "LocalPort", "RemAddr", "RemPort");
	printf("-------- ------------ -------- -------- -------- -------------------- -------- -------------------- --------\n");
	printf("\n");

	Chk(estats_list_conns(clist, cl));

	Chk(estats_connection_list_add_info(clist));

	estats_list_for_each(&clist->connection_info_head, ci, list) {
	    Chk(estats_connection_tuple_as_strings(&tuple_ascii, &ci->tuple));
//	    if (estats_connection_tuple_as_strings(&tuple_ascii, &ci->tuple))
//		    continue;

	    if ((int)(ci->pid) == -1) {
		if ((int)(ci->uid) == -1) {
		    printf("%-8d %-12s %-8s %-8s %-20s %-8s %-20s %-8s\n", ci->cid, ci->cmdline, "\0", "\0", tuple_ascii.local_addr, tuple_ascii.local_port, tuple_ascii.rem_addr, tuple_ascii.rem_port);
		}
		else 
		    printf("%-8d %-12s %-8s %-8u %-20s %-8s %-20s %-8s\n", ci->cid, ci->cmdline, "\0", ci->uid, tuple_ascii.local_addr, tuple_ascii.local_port, tuple_ascii.rem_addr, tuple_ascii.rem_port);
	    }
	    else printf("%-8d %-12s %-8u %-8u %-20s %-8s %-20s %-8s\n", ci->cid, ci->cmdline, ci->pid, ci->uid, tuple_ascii.local_addr, tuple_ascii.local_port, tuple_ascii.rem_addr, tuple_ascii.rem_port);
	}

 Cleanup:
	estats_connection_list_free(&clist);
	estats_nl_client_destroy(&cl);

	if (err != NULL) {
		PRINT_AND_FREE(err);
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
