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

estats_error*
estats_val_as_string(char** str, const estats_val* val, enum ESTATS_VAL_TYPE type)
{
	estats_error* err = NULL;

	ErrIf(str == NULL || val == NULL, ESTATS_ERR_INVAL);
 	
	Malloc((void**)str, 21);
	
	switch(type) {
	case ESTATS_UNSIGNED64:
		snprintf(*str, 21, "%"PRIu64"\0", val->uv64);
               	break;
	case ESTATS_UNSIGNED32:
		snprintf(*str, 21, "%"PRIu32"\0", val->uv32);
               	break;
	case ESTATS_SIGNED32:
		snprintf(*str, 21, "%"PRId32"\0", val->sv32);
               	break;
	case ESTATS_UNSIGNED16:
		snprintf(*str, 21, "%"PRIu16"\0", val->uv16);
               	break;
	case ESTATS_UNSIGNED8:
		snprintf(*str, 21, "%"PRIu8"\0", val->uv8);
               	break;
	default:
		snprintf(*str, 21, "\0");
		break;
	}
		
 Cleanup:
	return err;
}
