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
#ifndef TCPE_DATA_H
#define TCPE_DATA_H

struct estats_error* estats_data_new(struct estats_data**);
struct estats_error* estats_data_sized_new(struct estats_data**, int /*length*/);
struct estats_error* estats_data_free(struct estats_data**);
struct estats_error* estats_data_delta(struct estats_data*, const struct estats_data*, const struct estats_data*);

#endif /* TCPE_DATA_H */
