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
	struct estats_connection* cp;
	struct estats_connection_tuple_ascii asc;

	Chk(estats_nl_client_init(&cl));
	Chk(estats_connection_list_new(&clist));

	printf("%-8s %-20s %-8s %-20s %-8s\n", "CID", "LocalAddr", "LocalPort", "RemAddr", "RemPort");
	printf("-------- -------------------- -------- -------------------- --------\n");
	printf("\n");

	Chk(estats_list_conns(clist, cl));

	estats_list_for_each(&clist->connection_head, cp, list) {
		/* instead of casting, estats_connection_tuple should be
			an explicit member of estats_connection */
		struct estats_connection_tuple* ct = (struct estats_connection_tuple*) &(cp->rem_addr[0]);
		Chk(estats_connection_tuple_as_strings(&asc, ct));

		printf("%-8s %-20s %-8s %-20s %-8s\n", asc.cid, asc.local_addr, asc.local_port, asc.rem_addr, asc.rem_port);
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
