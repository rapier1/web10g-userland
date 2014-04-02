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
#ifndef ESTATS_RECORD_H
#define ESTATS_RECORD_H


estats_error* estats_record_open( estats_record **,
                         const char * /* path */,
                         const char * /* mode */);

void          estats_record_close( estats_record **);

estats_error* estats_record_read_data( estats_val_data **,
                                       estats_record *);

estats_error* estats_record_write_data( estats_record *,
                                        estats_val_data *);

#endif /* ESTATS_RECORD_H */
