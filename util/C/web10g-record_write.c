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
	printf("Usage: record_write cid\n");
}

int main(int argc, char **argv)
{

	estats_error* err = NULL;
	struct estats_nl_client* cl = NULL;
	estats_val_data* data = NULL;
	estats_record* record = NULL;
	estats_val val;
	char* str;
	int cid, i, j; 
	struct estats_connection_tuple_ascii tuple_ascii;

	if (argc < 2) {
                usage();
                exit(EXIT_FAILURE);
        }	

	cid = atoi(argv[1]);

	Chk(estats_nl_client_init(&cl));
	Chk(estats_val_data_new(&data));
	Chk(estats_record_open(&record, "./test-record", "w"));

	Chk(estats_read_vars(data, cid, cl));

	printf("Timestamp sec: %u, usec: %u\n", data->tv.sec, data->tv.usec);

	Chk(estats_connection_tuple_as_strings(&tuple_ascii, &data->tuple));

	printf("Address: %s %s %s %s\n", tuple_ascii.local_addr, tuple_ascii.local_port, tuple_ascii.rem_addr, tuple_ascii.rem_port);

	for (i = 0; i < data->length; i++) {
            Chk(estats_val_as_string(&str, &data->val[i], estats_var_array[i].valtype));

            printf("%s:  %s\n", estats_var_array[i].name, str);
	    free(str);
        }
	Chk(estats_record_write_data(record, data));

 Cleanup:
	estats_val_data_free(&data);
	estats_record_close(&record);
	estats_nl_client_destroy(&cl);

	if (err != NULL) {
		PRINT_AND_FREE(err);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
