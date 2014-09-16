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
#ifndef ESTATS_TYPES_H
#define ESTATS_TYPES_H

#include <linux/types.h>
#include <estats/list.h>

typedef enum ESTATS_VAL_TYPE {
        ESTATS_UNSIGNED64,
        ESTATS_UNSIGNED32,
        ESTATS_SIGNED32,
        ESTATS_UNSIGNED16,
        ESTATS_UNSIGNED8,
} ESTATS_VAL_TYPE;

typedef enum ESTATS_TYPE {
	ESTATS_TYPE_INTEGER,
	ESTATS_TYPE_INTEGER32,
	ESTATS_TYPE_INET_ADDRESS_IPV4,
	ESTATS_TYPE_COUNTER32,
	ESTATS_TYPE_GAUGE32,
	ESTATS_TYPE_UNSIGNED32,
	ESTATS_TYPE_TIME_TICKS,
	ESTATS_TYPE_COUNTER64,
	ESTATS_TYPE_INET_PORT_NUMBER,
	ESTATS_TYPE_INET_ADDRESS,
	ESTATS_TYPE_INET_ADDRESS_IPV6,
	ESTATS_TYPE_OCTET,
} ESTATS_TYPE;

typedef enum ESTATS_ADDRTYPE {
	ESTATS_ADDRTYPE_IPV4 = 1,
	ESTATS_ADDRTYPE_IPV6 = 2
} ESTATS_ADDRTYPE;

typedef enum ESTATS_ERROR {
        ESTATS_ERR_SUCCESS	= 0,
        ESTATS_ERR_ACK		= 1,
        ESTATS_ERR_RET_DATA	= 2,
        ESTATS_ERR_INVAL	= 3,
        ESTATS_ERR_NOMEM	= 4,
        ESTATS_ERR_NOENT	= 5,
        ESTATS_ERR_NOLINK	= 6,
        ESTATS_ERR_LIBC		= 7,
	ESTATS_ERR_EOF		= 8,
        ESTATS_ERR_CHKSUM	= 9,
	ESTATS_ERR_STR_CONV	= 10,
	ESTATS_ERR_GENL		= 11,
	ESTATS_ERR_FILE		= 12,
	ESTATS_ERR_ACCESS	= 13,
	ESTATS_ERR_ADDR_TYPE	= 14,
        ESTATS_ERR_UNKNOWN	= 15,
} ESTATS_ERROR;

typedef enum ESTATS_EVENT {
        ESTATS_NEW_CONN           = 0,
} ESTATS_EVENT;
/*
union estats_union {
	uint64_t uv64;
	uint32_t uv32;
	int32_t  sv32;
	uint16_t uv16;
	uint8_t  uv8;
};
*/

struct estats_connection_tuple {
	uint8_t   rem_addr[16];
	uint8_t   local_addr[16];
	uint16_t  rem_port;
	uint16_t  local_port;
	uint8_t   addr_type;
	int       cid;
};

struct estats_connection_tuple_ascii {
	char rem_addr[INET6_ADDRSTRLEN];
	char local_addr[INET6_ADDRSTRLEN];
	char rem_port[6];
	char local_port[6];
	char addr_type[5];
	char cid[11];
};

#define ESTATS_CMDLINE_LEN_MAX 16

struct estats_connection {
	struct list_node  list;
	uint8_t   rem_addr[16];
	uint8_t   local_addr[16];
	uint16_t  rem_port;
	uint16_t  local_port;
	uint8_t   addr_type;
	int       cid;
};

struct estats_connection_vars {
	struct list_node  list;
	struct estats_val_data *data;
};

struct estats_connection_info {
	struct list_node  list;
	struct estats_connection_tuple  tuple;
	char             cmdline[ESTATS_CMDLINE_LEN_MAX];
	pid_t            pid;
	uid_t            uid;
	ino_t            ino;
	int              state;
	int		 cid;
};

struct estats_connection_list {
	struct list_head  connection_head;
	struct list_head  connection_info_head;
};

struct estats_connection_vars_list {
	struct list_head  connection_vars_head;
	struct list_head  connection_info_head;
};

struct estats_var {
        char *name;
        enum ESTATS_VAL_TYPE valtype;
	enum ESTATS_TYPE type;
};

struct estats_val {
        uint64_t masked; /* uint64_t for alignment when r/w to records */
        union {
                uint64_t uv64;
                uint32_t uv32;
                int32_t  sv32;
                uint16_t uv16;
                uint8_t  uv8;
        };
};

struct estats_var_data {
	int num_tables;
	int *max_index;
	int length;
	struct estats_var var[0];
};

struct estats_timeval {
	uint32_t sec;
	uint32_t usec;
};

struct estats_val_data {
	struct estats_connection_tuple tuple;
	struct estats_timeval tv;
	/* why is this here?? it doesn't appear to be used... */
	struct estats_var_data *var_data;
	int length;
	struct estats_val val[0];
};

typedef enum ESTATS_RECORD_MODE {
	R_MODE,
	W_MODE
} ESTATS_RECORD_MODE;

struct estats_record {
	FILE*  fp;
	int    swap;
	int    bufsize;
	int    nvars;
	ESTATS_RECORD_MODE  mode;
};

enum MIB_TABLE {
        PERF_TABLE,
        PATH_TABLE,
        STACK_TABLE,
        APP_TABLE,
        TUNE_TABLE,
	EXTRAS_TABLE,
        __MAX_TABLE
};
#define MAX_TABLE __MAX_TABLE

struct estats_mask {
	uint64_t masks[MAX_TABLE];
	int      if_mask[MAX_TABLE];
};

extern struct estats_var estats_var_array[];

typedef struct estats_connection		estats_connection;
typedef struct estats_connection_vars		estats_connection_vars;
typedef struct estats_connection_info		estats_connection_info;
typedef struct estats_connection_list		estats_connection_list;
typedef struct estats_connection_vars_list	estats_connection_vars_list;
typedef struct estats_val_data			estats_val_data;
typedef struct estats_error			estats_error;
typedef struct estats_nl_client			estats_nl_client;
typedef struct estats_record			estats_record;
typedef struct estats_timeval			estats_timeval;
typedef struct estats_val			estats_val;

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

#endif /* ESTATS_TYPES_H */
