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
        printf("writevar cid var-name value\n");
	printf("\n Write a value to one of the four writable MIB vars\n");
	printf(" Currently supported: LimRwin, LimCwnd\n");
}

int main(int argc, char **argv)
{

	estats_error* err = NULL;
	struct estats_nl_client* cl = NULL;
	estats_val_data* data = NULL;
	int cid, i, j; 
	int opt, option;
	char varname[24];
	char *endptr, *str;
	uintmax_t val;

	if (argc < 4) {
                usage();
                exit(EXIT_FAILURE);
        }	

        while ((opt = getopt(argc, argv, "h")) != -1) {
                switch (opt) {
		case 'h':
                        usage();
                        exit(EXIT_SUCCESS);
                        break;
                default:
                        exit(EXIT_FAILURE);
                        break;
                }
        }

	cid = atoi(argv[1]);
	strncpy(varname, argv[2], 24);

	str = argv[3];
	val = strtoumax(str, &endptr, 10);

	Chk(estats_nl_client_init(&cl));

	Chk(estats_write_var(varname, (uint32_t)val, cid, cl));

 Cleanup:
	estats_nl_client_destroy(&cl);

	if (err != NULL) {
		PRINT_AND_FREE(err);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;

}
