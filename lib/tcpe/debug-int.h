/*
 * Copyright (c) 2012 The Board of Trustees of the University of Illinois,
 *                    Carnegie Mellon University.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 */
#ifndef TCPE_DEBUG_INT_H
#define TCPE_DEBUG_INT_H

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

#endif /* TCPE_DEBUG_INT_H */
