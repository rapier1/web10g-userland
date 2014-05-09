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
	estats_val_data* data = NULL;  // TODO(aka) currently not used

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

	Chk(estats_nl_client_init(&cl));
	Chk(estats_val_data_new(&data));
	Chk(estats_get_mib(data, cl));  // the MIB is printed out in estats_get_mib()

 Cleanup:
	estats_val_data_free(&data);
	estats_nl_client_destroy(&cl);

	if (err != NULL) {
		PRINT_AND_FREE(err);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
