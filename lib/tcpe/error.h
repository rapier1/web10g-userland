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
#ifndef TCPE_ERROR_H
#define TCPE_ERROR_H

estats_error*   estats_error_new(const TCPE_ERROR _errnum,
                             const char* _extra,
                             const char* _file,
                             const int _line,
                             const char* _function);
void          estats_error_free(estats_error** _err);
void          estats_error_print(FILE* _fp,
                                 const estats_error* _err);
TCPE_ERROR    estats_error_get_number(const estats_error* _err);
const char*   estats_error_get_message(const estats_error* _err);
const char*   estats_error_get_extra(const estats_error* _err);
const char*   estats_error_get_file(const estats_error* _err);
int           estats_error_get_line(const estats_error* _err);
const char*   estats_error_get_function(const estats_error* _err);

#endif /* TCPE_ERROR_H */
