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
#ifndef ESTATS_GENL_H
#define ESTATS_GENL_H

/* retrieves timestamp representing <time now> minus ms_delta milliseconds */
struct estats_error*
estats_get_timestamp(uint64_t* /*timestamp*/, uint32_t /*ms_delta*/,
			const estats_nl_client*);

/* frees connection_list entries, then
	fills connection_list with list of all existing connections */
struct estats_error*
estats_list_conns(estats_connection_list*, const estats_nl_client*);

/* frees connection_list entries, then
	fills connection_list with list of all existing connections
	which have seen activity since the given timestamp */
struct estats_error*
estats_list_conns_active_since(
	estats_connection_list*, uint64_t /*timestamp*/,
	const estats_nl_client*);

/* frees connection_list entries, then
	fills connection_list with list of all existing connections
	which have seen activity within ms_delta (in ms) of <now> */
struct estats_error*
estats_list_conns_active_within(
	estats_connection_list*, uint32_t /*ms_delta*/,
	const estats_nl_client*);

/* frees connection_vars_list entries, then
	fills connection_vars_list with
	list of all existing connections (with vars) */
struct estats_error*
estats_list_conns_vars(estats_connection_vars_list*, const estats_nl_client*);

/* frees connection_vars_list entries, then
	fills connection_vars_list with
	list of all existing connections (with vars)
	which have seen activity since the given timestamp */
struct estats_error*
estats_list_conns_vars_active_since(
	estats_connection_vars_list*, uint64_t /*timestamp*/,
	const estats_nl_client*);

/* frees connection_vars_list entries, then
	fills connection_vars_list with
	list of all existing connections (with vars)
	which have seen activity within ms_delta (in ms) of <now> */
struct estats_error*
estats_list_conns_vars_active_within(
	estats_connection_vars_list*, uint32_t /*ms_delta*/,
	const estats_nl_client*);

struct estats_error* estats_read_vars(struct estats_val_data*, int /*cid*/, const estats_nl_client*);
struct estats_error* estats_write_var(const char* /*name*/, uint32_t /*val*/, int /*cid*/, const estats_nl_client*);

/* NOTE - data is unused.  Should it be removed?? */
struct estats_error* estats_get_mib(struct estats_val_data*, const estats_nl_client*);

#endif /* ESTATS_GENL_H */
