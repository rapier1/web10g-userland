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
#ifndef TCPE_TYPES_H
#define TCPE_TYPES_H

enum MIB_TABLE {
        PERF_TABLE,
        PATH_TABLE,
        STACK_TABLE,
        APP_TABLE,
        TUNE_TABLE,
        __MAX_TABLE
};
#define MAX_TABLE __MAX_TABLE

enum TCPE_TYPE {
        TCPE_UNSIGNED64,
        TCPE_UNSIGNED32,
        TCPE_SIGNED32,
        TCPE_UNSIGNED16,
        TCPE_UNSIGNED8,
};

typedef enum TCPE_ERROR {
        TCPE_ERR_SUCCESS        = 0,
        TCPE_ERR_ACK            = 1,
        TCPE_ERR_RET_DATA       = 2,
        TCPE_ERR_INVAL          = 3,
        TCPE_ERR_NOMEM          = 4,
        TCPE_ERR_NOENT          = 5,
        TCPE_ERR_NOLINK         = 6,
        TCPE_ERR_LIBC           = 7,
	TCPE_ERR_EOF		= 8,
        TCPE_ERR_CHKSUM         = 9,
	TCPE_ERR_STR_CONV	= 10,
	TCPE_ERR_GENL		= 11,
        TCPE_ERR_UNKNOWN        = 12,
} TCPE_ERROR;

typedef enum TCPE_EVENT {
        TCPE_NEW_CONN           = 0,
} TCPE_EVENT;

union estats_union {
	uint64_t uv64;
	uint32_t uv32;
	int32_t  sv32;
	uint16_t uv16;
	uint8_t  uv8;
};

struct estats_val {
        union {
                uint64_t uv64;
                uint32_t uv32;
                int32_t  sv32;
                uint16_t uv16;
                uint8_t  uv8;
        };
        int mask;
};

struct estats_var {
        char *name;
        enum TCPE_TYPE type;
};

struct estats_data {
	int length;
	struct estats_val val[0];
};

struct estats_mask {
	uint64_t masks[MAX_TABLE];
	int      if_mask[MAX_TABLE];
};

enum estats_addrtype {
	TCPE_ADDRTYPE_IPV4 = 1,
	TCPE_ADDRTYPE_IPV6 = 2
};

/*
 * estats_addrtype is sent in *_addr[16], below
 */

struct estats_connection_tuple {
	uint8_t   rem_addr[17];
	uint8_t   local_addr[17];
	uint16_t  rem_port;
	uint16_t  local_port;
	int       cid;
};

typedef void (*estats_connection_func)(struct estats_connection_tuple*);

extern int max_index[];

extern struct estats_var estats_var_array[];

typedef struct estats_connection	estats_connection;
typedef struct estats_data	estats_data;
typedef struct estats_error	estats_error;
typedef struct estats_client	estats_client;

static inline int single_index(int inda, int indb)
{
	int ret = indb;
	int i;

	if (inda > 0) {
		for (i = 0; i < inda; i++) {
			ret += max_index[i];
		}
	}
	return ret;
}

typedef enum TCPE_PERF_INDEX {
	SEGSOUT                 = 0,
	DATASEGSOUT,
	DATAOCTETSOUT,
	HCDATAOCTETSOUT, 
	SEGSRETRANS,
	OCTETSRETRANS,
	SEGSIN,
	DATASEGSIN,
	DATAOCTETSIN,
	HCDATAOCTETSIN, 
	ELAPSEDSECS,
	ELAPSEDMICROSECS,
	STARTTIMESTAMP,
	CURMSS,
	PIPESIZE,
	MAXPIPESIZE,
	SMOOTHEDRTT,
	CURRTO,
	CONGSIGNALS,
	CURCWND,
	CURSSTHRESH,
	TIMEOUTS,
	CURRWINSENT,
	MAXRWINSENT,
	ZERORWINSENT,
	CURRWINRCVD,
	MAXRWINRCVD,
	ZERORWINRCVD,
	SNDLIMTRANSRWIN,
	SNDLIMTRANSCWND,
	SNDLIMTRANSSND,
	SNDLIMTIMERWIN,
	SNDLIMTIMECWND,
	SNDLIMTIMESND,
        __PERF_INDEX_MAX
} TCPE_PERF_INDEX;
#define PERF_INDEX_MAX __PERF_INDEX_MAX

typedef enum TCPE_PATH_INDEX {
        RETRANTHRESH,
        NONRECOVDAEPISODES,
        SUMOCTETSREORDERED,
        NONRECOVDA,
        SAMPLERTT,
        RTTVAR,
        MAXRTT,
        MINRTT,
        SUMRTT,
        HCSUMRTT,
        COUNTRTT,
        MAXRTO,
        MINRTO,
        IPTTL,
        IPTOSIN,
        IPTOSOUT,
        PRECONGSUMCWND,
        PRECONGSUMRTT,
        POSTCONGSUMRTT,
        POSTCONGCOUNTRTT,
        ECNSIGNALS,
        DUPACKEPISODES,
        RCVRTT,
        DUPACKSOUT,
        CERCVD,
        ECESENT,
        __PATH_INDEX_MAX
} TCPE_PATH_INDEX;
#define PATH_INDEX_MAX __PATH_INDEX_MAX

typedef enum TCPE_STACK_INDEX {
	ACTIVEOPEN,
	MSSSENT,
	MSSRCVD,
	WINSCALESENT,
	WINSCALERCVD,
	TIMESTAMPS,
	ECN,
	WILLSENDSACK,
	WILLUSESACK,
	STATE,
	NAGLE,
	MAXSSCWND,
	MAXCACWND,
	MAXSSTHRESH,
	MINSSTHRESH,
	INRECOVERY,
	DUPACKSIN,
	SPURIOUSFRDETECTED,
	SPURIOUSRTODETECTED,
	SOFTERRORS,
	SOFTERRORREASON,
	SLOWSTART,
	CONGAVOID,
	OTHERREDUCTIONS,
	CONGOVERCOUNT,
	FASTRETRAN,
	SUBSEQUENTTIMEOUTS,
	CURTIMEOUTCOUNT,
	ABRUPTTIMEOUTS,
	SACKSRCVD,
	SACKBLOCKSRCVD,
	SENDSTALL,
	DSACKDUPS,
	MAXMSS,
	MINMSS,
	SNDINITIAL,
	RECINITIAL,
	CURRETXQUEUE,
	MAXRETXQUEUE,
	CURREASMQUEUE,
	MAXREASMQUEUE,
        __STACK_INDEX_MAX
} TCPE_STACK_INDEX;
#define STACK_INDEX_MAX __STACK_INDEX_MAX

typedef enum TCPE_APP_INDEX {
        SNDUNA,
        SNDNXT,
        SNDMAX,
        THRUOCTETSACKED,
        HCTHRUOCTETSACKED, 
        RCVNXT,
        THRUOCTETSRECEIVED,
        HCTHRUOCTETSRECEIVED, 
        CURAPPWQUEUE,
        MAXAPPWQUEUE,
        CURAPPRQUEUE,
        MAXAPPRQUEUE,
        __APP_INDEX_MAX
} TCPE_APP_INDEX;
#define APP_INDEX_MAX __APP_INDEX_MAX

typedef enum TCPE_TUNE_INDEX { 
        LIMCWND,
        LIMSSTHRESH,
        LIMRWIN,
        LIMMSS,
        __TUNE_INDEX_MAX
} TCPE_TUNE_INDEX;
#define TUNE_INDEX_MAX __TUNE_INDEX_MAX

#define TOTAL_INDEX_MAX PERF_INDEX_MAX+PATH_INDEX_MAX+STACK_INDEX_MAX+APP_INDEX_MAX+TUNE_INDEX_MAX

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

#endif /* TCPE_TYPES_H */
