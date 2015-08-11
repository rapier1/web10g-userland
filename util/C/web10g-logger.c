/*
 * Copyright (c) 2013-2014 Centre for Advanced Internet Architectures, 
 * Swinburne University of Technology. 
 *
 * Author: Sebastian Zander (szander@swin.edu.au)
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
/* this really shouldn't be here - it's an internal header from lib/estats.
   (It's here because in scripts.h, Chk2 needs it, which is a mistake -
	so for now, I've changed Chk2 to Chk2Ign in this file...)
/*#include "estats/debug-int.h"*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


void usage(void)
{
	printf("\n\n");
	printf("web10g-logger [-e exclude_ip] [-i interval] [-m mask]\n");
	printf("\n  Repeatedly list tcp_estats variables for all connections\n");
	printf("  -i Optional poll interval given in milliseconds. Default 1000ms\n");
	printf("     e.g. the following command polls the variables every 100ms:\n");
	printf("\n");
	printf("     web10g-logger -i 100\n");
	printf("\n");
	printf("  -e Optional exclude_ip excludes flows from/to an IP or host name, e.g.\n");
	printf("     the following command does not print information for flows from/to\n");
	printf("     192.168.0.1.\n");
	printf("\n");
	printf("     web10g-logger -e 192.168.0.1\n");
	printf("\n");
	printf("  -m Optional mask given as a 5-tuple of hex values, e.g.\n");
	printf("\n");
	printf("     web10g-logger -m f,f,f,f,f\n");
	printf("\n");
	printf("    returns the first 4 entries of each of the MIB tables.\n");
	printf("    (Perf, Path, Stack, App, Tune; cf. RFC 4898)\n");
	printf("\n");
	printf("    web10g-logger -m 0,0,0,,0\n");
	printf("\n");
	printf("    returns only the MIB App table, etc.\n\n");
	printf("    The mask limits the time the kernel holds a lock on the\n");
	printf("    socket of interest.\n");
	printf("\n");
}



int main(int argc, char **argv)
{
	estats_error* err = NULL;
	estats_nl_client* cl = NULL;
	estats_val_data* data = NULL;
	struct estats_connection_list* clist = NULL;
	struct estats_connection* cp;
	struct estats_connection_tuple_ascii asc;

	int cid, i, flag = 0; 
	int opt;

	char *strmask = NULL;
	struct estats_mask mask;
	uint64_t tmpmask;
	const char delim = ',';

	mask.masks[0] = DEFAULT_PERF_MASK;
	mask.masks[1] = DEFAULT_PATH_MASK;
	mask.masks[2] = DEFAULT_STACK_MASK;
	mask.masks[3] = DEFAULT_APP_MASK;
	mask.masks[4] = DEFAULT_TUNE_MASK;

	unsigned int interval = 1000;
	char exclude_ip[64];

	for (i = 0; i < MAX_TABLE; i++) {
		mask.if_mask[i] = 0;
	}

	while ((opt = getopt(argc, argv, "e:hi:m:")) != -1) {
		switch (opt) {
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
			break;
		case 'e':
			{
			struct sockaddr_in sa;
			struct addrinfo hints;
			struct addrinfo *result;
			int ret;

			strncpy(exclude_ip, optarg, sizeof(exclude_ip));

			memset(&hints, 0, sizeof(struct addrinfo));
			hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
			hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
			hints.ai_flags = 0;
			hints.ai_protocol = 0;		/* Any protocol */

			if ((ret = getaddrinfo(exclude_ip, NULL, &hints, &result) != 0)) {
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
				exit(EXIT_FAILURE);
			}
			strcpy(exclude_ip, inet_ntoa(((struct sockaddr_in *) result->ai_addr)->sin_addr));
			
			freeaddrinfo(result);

			if (inet_pton(AF_INET, exclude_ip, &(sa.sin_addr)) == 0) {
				fprintf(stderr, "Exclude IP '%s' is not a valid IP address\n", exclude_ip);
				exit(EXIT_FAILURE);
			}
			}

			break;
		case 'i':
			interval = atoi(optarg);
			break;
		case 'm':
			strmask = strdup(optarg);

			for (i = 0; i < 5; i++) {
				char *strtmp;
				strtmp = strsep(&strmask, &delim);
				if (strtmp && strlen(strtmp)) {
					char *str;
					str = (str = strchr(strtmp, 'x')) ? str+1 : strtmp;
					if (sscanf(str, "%"PRIx64, &tmpmask) == 1) {
						mask.masks[i] = tmpmask & mask.masks[i];
						mask.if_mask[i] = 1;
					}
				}
			}

			break;
		default:
			exit(EXIT_FAILURE);
			break;
		}
	}

	Chk(estats_nl_client_init(&cl));
	Chk(estats_connection_list_new(&clist));
	Chk(estats_nl_client_set_mask(cl, &mask));
	Chk(estats_val_data_new(&data));

	while (1) {

		/* active get connections */
		Chk(estats_list_conns(clist, cl));

		list_for_each(&clist->connection_head, cp, list) {
			/* instead of casting, estats_connection_tuple should be
				an explicit member of estats_connection */
			struct estats_connection_tuple* ct = (struct estats_connection_tuple*) &(cp->rem_addr[0]);

			Chk2Ign(estats_connection_tuple_as_strings(&asc, ct));
			Chk2Ign(estats_read_vars(data, atoi(asc.cid), cl));
	
			if (data->length == 0)
				continue;

			if (strcmp(exclude_ip, asc.local_addr) == 0 || strcmp(exclude_ip, asc.rem_addr) == 0)
				continue;

			if (!flag) {
				printf("Time, CID, SrcIp, SrcPort, DstIp, DstPort,");
				for (i = 0; i < data->length; i++) {
					if (data->val[i].masked)
						continue;
					printf("%s", estats_var_array[i].name);
					if (i != data->length-1) 
						printf(", ");
				}
				printf ("\n");
				flag = 1;
			}
			
			printf("%u.%06u,", data->tv.sec, data->tv.usec);
			printf("%s,%s,%s,%s,%s,", asc.cid, asc.local_addr, asc.local_port, asc.rem_addr, asc.rem_port);

			for (i = 0; i < data->length; i++) {

				if (data->val[i].masked) continue;

				switch(estats_var_array[i].valtype) {
					case ESTATS_UNSIGNED64:
						printf("%"PRIu64"", data->val[i].uv64);
						break;
					case ESTATS_UNSIGNED32:
						printf("%"PRIu32"", data->val[i].uv32);
						break;
					case ESTATS_SIGNED32:
						printf("%"PRId32"", data->val[i].sv32);
						break;
					case ESTATS_UNSIGNED16:
						printf("%"PRIu16"", data->val[i].uv16);
						break;
					case ESTATS_UNSIGNED8:
						printf("%"PRIu8"", data->val[i].uv8);
						break;
					default:
						break;
				}

				if (i < data->length - 1) {
					printf(",");
				}
			}

			printf("\n");
 Continue:
			while(0) {} /* make the compiler happy */
		}

		usleep(interval * 1000);
	}

 Cleanup:
	estats_connection_list_free(&clist);
	estats_val_data_free(&data);
	estats_nl_client_destroy(&cl);

	if (err != NULL) {
		PRINT_AND_FREE(err);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
