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
#ifndef ESTATS_SOCKINFO_H
#define ESTATS_SOCKINFO_H


typedef struct estats_sockinfo estats_sockinfo;
typedef struct estats_sockinfo_item estats_sockinfo_item;

estats_error* estats_sockinfo_get_list_head( estats_sockinfo_item **,
                                             estats_sockinfo *);

estats_error* estats_sockinfo_item_next( estats_sockinfo_item **,
                                   const estats_sockinfo_item* _prev);

#define ESTATS_SOCKINFO_FOREACH(pos, head) \
    for (pos = head; pos != NULL; pos = estats_sockinfo_item_return_next(pos))

estats_sockinfo_item* estats_sockinfo_item_return_next(const estats_sockinfo_item* _prev);

estats_error* estats_sockinfo_new(estats_sockinfo **, estats_agent *);

void          estats_sockinfo_free(estats_sockinfo** _sockinfo);

estats_error* estats_sockinfo_get_cid(int* _cid,
                                  const estats_sockinfo_item* _sockinfo);

estats_error* estats_sockinfo_get_pid(int* _pid,
                                  const estats_sockinfo_item* _sockinfo);

estats_error* estats_sockinfo_get_uid(int* _uid,
                                  const estats_sockinfo_item* _sockinfo);

estats_error* estats_sockinfo_get_state(int* _state,
                                    const estats_sockinfo_item* _sockinfo);

estats_error* estats_sockinfo_get_cmdline(char** _cmdline,
                                      const estats_sockinfo_item* _sockinfo);
estats_error* estats_sockinfo_get_connection_spec( struct estats_connection_spec *,
                                                    const estats_sockinfo_item *);

#endif /* ESTATS_SOCKINFO_H */
