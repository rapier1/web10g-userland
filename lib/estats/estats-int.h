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
#ifndef ESTATS_INT_H
#define ESTATS_INT_H

/* System headers (use config.h's results to determine whether or not to
   include them) */
#include "config.h"
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include <estats/types.h>
#include <estats/types-int.h>

/* estats public headers */
#include <estats/error.h>
#include <estats/connection.h>
#include <estats/data.h>
#include <estats/genl.h>
#include <estats/estats.h>
#include <estats/list.h>
#include <estats/tcp_estats_nl.h>
#include <estats/nl-client.h>
#include <estats/val.h>

/* estats internal headers */
#include <estats/debug-int.h>
#include <estats/error-int.h>
#include <estats/libcwrap-int.h>
#include <estats/resolve-int.h>

#endif /* ESTATS_INT_H */
