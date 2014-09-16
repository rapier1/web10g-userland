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
#ifndef ESTATS_CONNECTION_H
#define ESTATS_CONNECTION_H
#include "uthash.h"
#include "lookup3.h"

struct	estats_error* estats_connection_list_new( struct estats_connection_list** );

void	estats_connection_list_free( struct estats_connection_list** );

struct	estats_error* estats_connection_vars_list_new( struct estats_connection_vars_list** );

void	estats_connection_vars_list_free( struct estats_connection_vars_list** );

struct	estats_error* estats_connection_vars_new( struct estats_connection_vars** );

void	estats_connection_vars_free( struct estats_connection_vars** );

struct	estats_error* estats_connection_info_new( struct estats_connection_info** );

void	estats_connection_info_free( struct estats_connection_info** );

struct	estats_error* estats_connection_tuple_as_strings(
				struct estats_connection_tuple_ascii*,
				struct estats_connection_tuple* );

struct estats_error* estats_connection_list_add_info(struct estats_connection_list*);

estats_error* estats_connection_info_get_cid(int* /*cid*/, const estats_connection_info*);

estats_error* estats_connection_info_get_pid(int* /*pid*/, const estats_connection_info*);

estats_error* estats_connection_info_get_uid(int* /*uid*/, const estats_connection_info*);

estats_error* estats_connection_info_get_cmdline(char** /*str*/, const estats_connection_info*);

estats_error* estats_connection_info_get_tuple(struct estats_connection_tuple*, const estats_connection_info*);

// the following struct and functions defines are private
struct s_tcp_hash {
	uint32_t tcpkey;
	struct estats_connection_info* info;
	UT_hash_handle hh;
};

struct s_ino_hash {
	int ino;
	uint32_t inokey;
	struct estats_connection_info* info;
	UT_hash_handle hh;
};

struct s_pid_hash {
	int ino;
	struct estats_connection_info* info;
	UT_hash_handle hh;
};


#endif /* ESTATS_CONNECTION_H */
