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
#ifndef ESTATS_ERROR_H
#define ESTATS_ERROR_H

estats_error*   estats_error_new(const ESTATS_ERROR _errnum,
                             const char* _extra,
                             const char* _file,
                             const int _line,
                             const char* _function);
void          estats_error_free(estats_error** _err);
void          estats_error_print(FILE* _fp,
                                 const estats_error* _err);
ESTATS_ERROR    estats_error_get_number(const estats_error* _err);
const char*   estats_error_get_message(const estats_error* _err);
const char*   estats_error_get_extra(const estats_error* _err);
const char*   estats_error_get_file(const estats_error* _err);
int           estats_error_get_line(const estats_error* _err);
const char*   estats_error_get_function(const estats_error* _err);

#endif /* ESTATS_ERROR_H */
