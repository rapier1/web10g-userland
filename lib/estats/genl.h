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

struct estats_error* estats_list_conns(estats_connection_list*, const estats_nl_client*);
struct estats_error* estats_read_vars(struct estats_val_data*, int /*cid*/, const estats_nl_client*);
struct estats_error* estats_write_var(const char* /*name*/, uint32_t /*val*/, int /*cid*/, const estats_nl_client*);

#endif /* ESTATS_GENL_H */
