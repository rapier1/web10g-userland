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
#ifndef ESTATS_DEBUG_INT_H
#define ESTATS_DEBUG_INT_H

#if !defined(IFDEBUG)
# if defined(DEBUG)
#  define IFDEBUG(x) (x)
# else
#  define IFDEBUG(x)
# endif /* defined(DEBUG) */
#endif /* !defined(IFDEBUG) */

#if defined(DEBUG)
#define dbgprintf(fmt, args...) \
    do { \
        dbgprintf_no_prefix("DBG: "); \
        dbgprintf_no_prefix(fmt, ## args); \
    } while (0)
#else
# define dbgprintf(fmt, args...)
#endif

void dbgprintf_no_prefix(const char* _fmt, ...);

#endif /* ESTATS_DEBUG_INT_H */
