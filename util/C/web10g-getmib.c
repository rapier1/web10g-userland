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

void usage(void)
{
        printf("\n\n");
        printf("getmib\n");
	printf("\n");
}

int main(int argc, char **argv)
{

	estats_error* err = NULL;
	struct estats_nl_client* cl = NULL;
	estats_val_data* data = NULL;
	int cid, i, j; 
	int opt, option;

        /*
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
        */

        /*
        while ((opt = getopt(argc, argv, "hm:")) != -1) {
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
	*/

	Chk(estats_nl_client_init(&cl));
	//Chk(estats_nl_client_set_mask(cl, &mask));
	Chk(estats_val_data_new(&data));

	Chk(estats_get_mib(data, cl));

        // The MIB is generated in the DLKM with the following code:
        /*
	hdr = genlmsg_put(msg, 0, 0, &genl_estats_family, 0, TCPE_CMD_INIT);
	if (nla_put_u32(msg, NLE_ATTR_NUM_TABLES, MAX_TABLE))
	if (nla_put_u32(msg, NLE_ATTR_NUM_VARS, TOTAL_NUM_VARS))
	for (tblnum = 0; tblnum < MAX_TABLE; tblnum++) {
		switch (tblnum) {
		case PERF_TABLE:
			nest[tblnum] = nla_nest_start(msg,
					NLE_ATTR_PERF_VARS | NLA_F_NESTED);
			break;
		case PATH_TABLE:
			nest[tblnum] = nla_nest_start(msg,
					NLE_ATTR_PATH_VARS | NLA_F_NESTED);
			break;
		case STACK_TABLE:
			nest[tblnum] = nla_nest_start(msg,
					NLE_ATTR_STACK_VARS | NLA_F_NESTED);
			break;
		case APP_TABLE:
			nest[tblnum] = nla_nest_start(msg,
					NLE_ATTR_APP_VARS | NLA_F_NESTED);
			break;
		case TUNE_TABLE:
			nest[tblnum] = nla_nest_start(msg,
					NLE_ATTR_TUNE_VARS | NLA_F_NESTED);
			break;
		case EXTRAS_TABLE:
			nest[tblnum] = nla_nest_start(msg,
					NLE_ATTR_EXTRAS_VARS | NLA_F_NESTED);
			break;
		}
		if (!nest[tblnum])
			goto nla_put_failure;

		for (i=0; i < estats_max_index[tblnum]; i++) {
			entry_nest = nla_nest_start(msg,
					NLE_ATTR_VAR | NLA_F_NESTED);

			if (nla_put_string(msg, NEA_VAR_NAME,
					estats_var_array[tblnum][i].name))
				goto nla_put_failure;

			if (nla_put_u32(msg, NEA_VAR_TYPE,
					estats_var_array[tblnum][i].vartype))
				goto nla_put_failure;

			nla_nest_end(msg, entry_nest);
		}

		nla_nest_end(msg, nest[tblnum]);
        }
	genlmsg_end(msg, hdr);
        */

	printf("web10g-getmib: DEBUG: I don't think we need anything here, as estats_get_mib should print everything out!\n");

	printf("DEBUG: operating over %d indices in data.\n", data->length);
	for (j = 0; j < data->length; j++){
	printf("DEBUG: operating on index: %d\n", j);

        // Print out headers.
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
        if (j == PERF_INDEX_MAX+PATH_INDEX_MAX+STACK_INDEX_MAX+APP_INDEX_MAX+TUNE_INDEX_MAX)
          printf("\n\n Extras Table\n\n");



        /*
          if (data->val[j].masked) continue;

          switch(estats_var_array[j].valtype) {
          case ESTATS_UNSIGNED64:
				printf("%s=%"PRIu64"\n", estats_var_array[j].name, data->val[j].uv64);
                		break;
                        case ESTATS_UNSIGNED32:
				printf("%s=%"PRIu32"\n", estats_var_array[j].name, data->val[j].uv32);
				break;
                        case ESTATS_SIGNED32:
				printf("%s=%"PRId32"\n", estats_var_array[j].name, data->val[j].sv32);
                        	break;
                        case ESTATS_UNSIGNED16:
				printf("%s=%"PRIu16"\n", estats_var_array[j].name, data->val[j].uv16);
                        	break;
                        case ESTATS_UNSIGNED8:
				printf("%s=%"PRIu8"\n", estats_var_array[j].name, data->val[j].uv8);
                        	break;
                        default:
                                break;
                }
                */
	}

 Cleanup:
	estats_val_data_free(&data);
	estats_nl_client_destroy(&cl);

	if (err != NULL) {
		PRINT_AND_FREE(err);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
