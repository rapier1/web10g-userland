/*
 * Copyright (c) 2013 The Board of Trustees of the University of Illinois,
 *		      Carnegie Mellon University.
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

void usage(void)
{
	printf("\n\n");
	printf("deltavars cid [-m mask] [-s] [-i interval]\n");
	printf("\n  Repeatedly list deltas of values for connection specified\n");
	printf("  by cid (which can be obtained with \"listconns\"),\n");
	printf("  with optional mask given as a 5-tuple of hex\n");
	printf("  values, e.g.\n");
	printf("\n");
	printf("  deltavars cid -m f,f,f,f,f\n");
	printf("\n");
	printf("  returns the first 4 entries of each of the MIB tables.\n");
	printf("  (Perf, Path, Stack, App, Tune; cf. RFC 4898)\n");
	printf("\n");
	printf("  deltavars cid -m 0,0,0,,0\n");
	printf("\n");
	printf("  returns only the MIB App table, etc.\n");
	printf("\n  The mask limits the time the kernel holds a lock on\n");
	printf("  the socket of interest.\n");
	printf("\n");
	printf("  -s print formated comma seperated output to stdout\n");
	printf("\n");
	printf("  -i reporting interval in milliseconds [default 1000]\n");
	printf("\n");
}

int main(int argc, char **argv)
{

	estats_error* err = NULL;
	estats_nl_client* cl = NULL;
	estats_val_data* data_delta = NULL;
	estats_val_data* data_new = NULL;
	estats_val_data* data_prev = NULL;
	estats_val_data* data_ptr;
	int cid, i, j;
	int opt, option;
	int header_flag = 0;
	int stdout_flag = 0;
	int interval = 1000;

	char *strmask = NULL;
	const char delim = ',';
	uint64_t tmpmask;

	struct estats_mask mask;

	mask.masks[0] = DEFAULT_PERF_MASK;
	mask.masks[1] = DEFAULT_PATH_MASK;
	mask.masks[2] = DEFAULT_STACK_MASK;
	mask.masks[3] = DEFAULT_APP_MASK;
	mask.masks[4] = DEFAULT_TUNE_MASK;

	for (i = 0; i < MAX_TABLE; i++) {
		mask.if_mask[i] = 0;
	}

	if (argc < 2) {
		usage();
		exit(EXIT_FAILURE);
	}

	while ((opt = getopt(argc, argv, "hm:si:")) != -1) {
		switch (opt) {
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
			break;
		case 'm':
			strmask = strdup(optarg);

			for (j = 0; j < 5; j++) {
				char *strtmp;
				strtmp = strsep(&strmask, &delim);
				if (strtmp && strlen(strtmp)) {
				char *str;
				str = (str = strchr(strtmp, 'x')) ? str+1 : strtmp;
				if (sscanf(str, "%"PRIx64, &tmpmask) == 1) {
					mask.masks[j] = tmpmask & mask.masks[j];
					mask.if_mask[j] = 1;
				}
				}
			}
			option = opt;

			break;
		case 's':
			stdout_flag = 1;
			break;
		case 'i':
			interval = atoi(optarg);
			break;
		default:
			exit(EXIT_FAILURE);
			break;
		}
	}
	if ((option == 'm') && (optind+1 > argc)) {
		printf("Too few non-option args\n");
		exit(EXIT_FAILURE);
	}

	cid = atoi(argv[optind]);


	Chk(estats_nl_client_init(&cl));
	Chk(estats_nl_client_set_mask(cl, &mask));
	Chk(estats_val_data_new(&data_delta));
	Chk(estats_val_data_new(&data_new));
	Chk(estats_val_data_new(&data_prev));


	while (1) {
		
		SWAP(data_new, data_prev);
		
		Chk(estats_read_vars(data_new, cid, cl));
		
		Chk(estats_val_data_delta(data_delta, data_new, data_prev));
		
		if ((stdout_flag) && (!header_flag)) {
			printf("sec,usec,");
			for (j = 0; j < data_new->length; j++) {
				if (data_new->val[j].masked) continue;
				printf("%s,", estats_var_array[j].name);
			}
			printf("\n");
			header_flag = 1;
		} 
		
		if (!stdout_flag)
			printf("Timestamp sec: %u, usec: %u\n", data_new->tv.sec, data_new->tv.usec);
		else
			printf("%u,%u,", data_new->tv.sec, data_new->tv.usec);
		
		for (j = 0; j < data_new->length; j++) {
			
			if (!stdout_flag) {
				if (j == 0)
					printf("\n\n Perf Table\n\n");
				if (j == PERF_INDEX_MAX)
					printf("\n\n Path Table\n\n");
				if (j == PERF_INDEX_MAX+PATH_INDEX_MAX)
					printf("\n\n Stack Table\n\n");
				if (j == PERF_INDEX_MAX+PATH_INDEX_MAX+STACK_INDEX_MAX)
					printf("\n\n App Table\n\n");
				if (j == PERF_INDEX_MAX+PATH_INDEX_MAX+STACK_INDEX_MAX+APP_INDEX_MAX)
					printf("\n\n Tune Table\n\n");
			}
			
			if (data_new->val[j].masked) continue;
			
			switch(estats_var_array[j].type) {
			case ESTATS_TYPE_COUNTER32:
			case ESTATS_TYPE_COUNTER64:
				data_ptr = data_delta;
				break;
			default:
				data_ptr = data_new;
				break;
			}
			
			/* I could have had an if in each case statement but that seems like
			 * it would have been much more expensive - cjr
			 */
			if (!stdout_flag) {
				switch(estats_var_array[j].valtype) {
				case ESTATS_UNSIGNED64:
					printf("%-20s= %"PRIu64"\n", estats_var_array[j].name, data_ptr->val[j].uv64);
					break;
				case ESTATS_UNSIGNED32:
					printf("%-20s= %"PRIu32"\n", estats_var_array[j].name, data_ptr->val[j].uv32);
					break;
				case ESTATS_SIGNED32:
					printf("%-20s= %"PRId32"\n", estats_var_array[j].name, data_ptr->val[j].sv32);
					break;
				case ESTATS_UNSIGNED16:
					printf("%-20s= %"PRIu16"\n", estats_var_array[j].name, data_ptr->val[j].uv16);
					break;
				case ESTATS_UNSIGNED8:
					printf("%-20s= %"PRIu8"\n", estats_var_array[j].name, data_ptr->val[j].uv8);
					break;
				default:
					break;
				}
			} else {
				switch(estats_var_array[j].valtype) {
				case ESTATS_UNSIGNED64:
					printf("%"PRIu64",", data_ptr->val[j].uv64);
					break;
				case ESTATS_UNSIGNED32:
					printf("%"PRIu32",", data_ptr->val[j].uv32);
					break;
				case ESTATS_SIGNED32:
					printf("%"PRId32",", data_ptr->val[j].sv32);
					break;
				case ESTATS_UNSIGNED16:
					printf("%"PRIu16",", data_ptr->val[j].uv16);
					break;
				case ESTATS_UNSIGNED8:
					printf("%"PRIu8",", data_ptr->val[j].uv8);
					break;
				default:
					break;
				}
			}
			
		}
		
		usleep(interval * 1000);
		printf ("\n");
		if (!stdout_flag)
			printf("\n");
	}

 Cleanup:

	estats_val_data_free(&data_delta);
	estats_val_data_free(&data_new);
	estats_val_data_free(&data_prev);
	estats_nl_client_destroy(&cl);

	if (err != NULL) {
		PRINT_AND_FREE(err);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
