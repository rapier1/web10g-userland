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
#ifndef TCPE_GENL_H
#define TCPE_GENL_H

struct estats_error* estats_list_conns(struct estats_client*, estats_connection_func);
struct estats_error* estats_read_vars(struct estats_data*, int /*cid*/, const estats_client*);
struct estats_error* estats_write_var(const char* /*name*/, uint32_t /*val*/, int /*cid*/, const estats_client*);

#endif /* TCPE_GENL_H */
