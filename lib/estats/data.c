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
estats_val_data_new(struct estats_val_data** data)
{
	estats_error* err = NULL;
	int len = TOTAL_NUM_VARS;

	ErrIf(data == NULL, ESTATS_ERR_INVAL);
	*data = NULL;

	Chk(Malloc((void**) data, sizeof(estats_val_data) + len*sizeof(struct estats_val)));
	memset((void*) *data, 0, sizeof(estats_val_data) + len*sizeof(struct estats_val));
	(*data)->var_data = NULL;
	(*data)->length = len;

 Cleanup:
 	return err;
}

void
estats_val_data_free(struct estats_val_data** data)
{
	if (data == NULL || *data == NULL)
		return;

	free(*data);
	*data = NULL;
}

struct estats_error* estats_val_data_delta(struct estats_val_data* data, const struct estats_val_data* data2, const struct estats_val_data* data1)
{
	estats_error* err = NULL;
	int i;

	ErrIf(data == NULL || data2 == NULL || data1 == NULL, ESTATS_ERR_INVAL);

	for (i = 0; i < data->length; i++) {

		if (data2->val[i].masked || data1->val[i].masked) {
			data->val[i].masked = 1;
			continue;
		}
		switch(estats_var_array[i].valtype) {
		case ESTATS_UNSIGNED64:
			data->val[i].uv64 =
				data2->val[i].uv64 - data1->val[i].uv64;
			break;
		case ESTATS_UNSIGNED32:
			data->val[i].uv32 =
				data2->val[i].uv32 - data1->val[i].uv32;
			break;
		case ESTATS_SIGNED32:
			data->val[i].sv32 =
				data2->val[i].sv32 - data1->val[i].sv32;
			break;
		case ESTATS_UNSIGNED16:
			data->val[i].uv16 =
				data2->val[i].uv16 - data1->val[i].uv16;
			break;
		case ESTATS_UNSIGNED8:
			data->val[i].uv8 =
				data2->val[i].uv8 - data1->val[i].uv8;
			break;
		default:
			break;
		}
	}

 Cleanup:
 	return err;
}
