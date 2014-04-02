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
#ifndef ESTATS_DATA_H
#define ESTATS_DATA_H

struct estats_error* estats_val_data_new(struct estats_val_data**);
void   estats_val_data_free(struct estats_val_data**);
struct estats_error* estats_val_data_delta(struct estats_val_data*, const struct estats_val_data*, const struct estats_val_data*);

#endif /* ESTATS_DATA_H */
