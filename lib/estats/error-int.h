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
#ifndef ESTATS_ERROR_INT_H
#define ESTATS_ERROR_INT_H

#define Chk(x) \
    do { \
        err = (x); \
        if (err != NULL) { \
            dbgprintf("   ... saw error \"%s\" (error code %d) at %s:%d in function %s (\"%s\")\n", \
                      estats_error_get_message(err), \
                      estats_error_get_number(err), \
                      __FILE__, \
                      __LINE__, \
                      __FUNCTION__, \
                      estats_error_get_extra(err)); \
            goto Cleanup; \
        } \
    } while (0)

#define Err(x) Err2(x, NULL)

#if defined(DEBUG)
# define Err2(x, xtra) \
    do { \
        err = estats_error_new(x, xtra, __FILE__, __LINE__, __FUNCTION__); \
        dbgprintf("Throwing error \"%s\" (error code %d) at %s:%d in function %s (\"%s\")\n", \
                  estats_error_get_message(err), \
                  estats_error_get_number(err), \
                  estats_error_get_file(err), \
                  estats_error_get_line(err), \
                  estats_error_get_function(err), \
                  estats_error_get_extra(err)); \
        goto Cleanup; \
    } while (0)
#else
# define Err2(x, xtra) \
    do { \
        err = estats_error_new(x, xtra, NULL, -1, NULL); \
        goto Cleanup; \
    } while (0)
#endif /* defined(DEBUG) */

#define ErrIf(f, newErr) Err2If(f, newErr, NULL)
#define Err2If(f, newErr, xtra) \
    do { \
        if (f) { \
            Err2(newErr, xtra); \
        } \
    } while (0)
#define ErrUnless(f, newErr) ErrIf(!(f), newErr)
#define Err2Unless(f, newErr, xtra) Err2If(!(f), newErr, xtra)

#endif /* ESTATS_ERROR_INT_H */
