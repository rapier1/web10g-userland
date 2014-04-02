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
#ifndef ESTATS_CLIENT_H
#define ESTATS_CLIENT_H

struct estats_error* estats_nl_client_init(struct estats_nl_client**);
void                 estats_nl_client_destroy(struct estats_nl_client**);
struct estats_error* estats_nl_client_get_sock(struct mnl_socket**, const estats_nl_client*);
struct estats_error* estats_nl_client_get_fam_id(int*, const estats_nl_client*);
struct estats_error* estats_nl_client_set_mask(struct estats_nl_client*, struct estats_mask*);

#endif /* ESTATS_CLIENT_H */
