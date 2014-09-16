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
#include <estats/estats-int.h>

static estats_error* _estats_record_new(estats_record** _record);
static estats_error* _estats_record_open_read(estats_record* _record, const char* _path);
static estats_error* _estats_record_open_write(estats_record* _record, const char* _path);
static estats_error* _estats_record_calculate_checksum(estats_record* _record, uint16_t* _sum);
static void _estats_swap_val(estats_val* _val, ESTATS_VAL_TYPE _valtype);
static estats_error* _estats_record_write_checksum(estats_record* _record);
static void          _estats_record_free(estats_record** _record);

#include <endian.h>
#include <byteswap.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define RECORD_HOST_ORDER 0
#else
#define RECORD_HOST_ORDER 1
#endif

estats_error*
estats_record_open(estats_record** record, const char* path, const char* mode)
{
    estats_error* err = NULL;

    Chk(_estats_record_new(record));

    switch (mode[0]) {
       	case 'r':
	    Chk(_estats_record_open_read(*record, path));
	    break;
       	case 'w':
	    Chk(_estats_record_open_write(*record, path));
	    break;
       	default:
	    Err(ESTATS_ERR_INVAL);
	    break; /* not reached */
    }

Cleanup:
    if (err != NULL) {
        _estats_record_free(record);
    }

    return err;
}

void
estats_record_close(estats_record** record)
{
    if (record == NULL || *record == NULL) return;

    if ((*record)->mode == W_MODE) {
        if (_estats_record_write_checksum(*record) != NULL)
	    dbgprintf("_estats_record_write_checksum");
    }

    _estats_record_free(record);
}

estats_error*
estats_record_read_data(estats_val_data** data, estats_record* record)
{
    estats_error* err = NULL;
    int i;
    
    ErrIf(record == NULL, ESTATS_ERR_INVAL);
    ErrIf(record->mode != R_MODE, ESTATS_ERR_ACCESS);
    ErrIf(record->fp == NULL, ESTATS_ERR_FILE);
    ErrIf(data == NULL, ESTATS_ERR_INVAL);

    *data = NULL;

    Chk(estats_val_data_new(data));

    Chk(Fread(NULL, &(*data)->tv.sec, 4, 1, record->fp));
    Chk(Fread(NULL, &(*data)->tv.usec, 4, 1, record->fp));
    for (i =0; i < 16; i++) {
	    Chk(Fread(NULL, &(*data)->tuple.rem_addr[i], 1, 1, record->fp));
    }
    for (i =0; i < 16; i++) {
	    Chk(Fread(NULL, &(*data)->tuple.local_addr[i], 1, 1, record->fp));
    }
    Chk(Fread(NULL, &(*data)->tuple.rem_port, 2, 1, record->fp));
    Chk(Fread(NULL, &(*data)->tuple.local_port, 2, 1, record->fp));
    
    Chk(Fread(NULL, (*data)->val, (size_t)(((*data)->length)*sizeof(estats_val)), 1, record->fp));

    if (record->swap) {
	(*data)->tv.sec = bswap_32((*data)->tv.sec);
	(*data)->tv.usec = bswap_32((*data)->tv.usec);
	(*data)->tuple.rem_port = bswap_16((*data)->tuple.rem_port);
	(*data)->tuple.local_port = bswap_16((*data)->tuple.local_port);
	for (i=0; i < (*data)->length; i++) {
	    bswap_64((*data)->val[i].masked);
	    _estats_swap_val(&(*data)->val[i], estats_var_array[i].valtype);
	}
    }

Cleanup:
    if (err != NULL) {
        estats_val_data_free(data);
    }

    return err;
}

estats_error*
estats_record_write_data(estats_record* record, estats_val_data* data)
{
    estats_error* err = NULL;
    int i;

    ErrIf(record == NULL || data == NULL, ESTATS_ERR_INVAL);
    ErrIf(record->mode != W_MODE, ESTATS_ERR_ACCESS);
    ErrIf(record->fp == NULL, ESTATS_ERR_FILE);

    Chk(Fwrite(NULL, &data->tv.sec, 4, 1, record->fp));
    Chk(Fwrite(NULL, &data->tv.usec, 4, 1, record->fp));
    for (i =0; i < 16; i++) {
	    Chk(Fwrite(NULL, &data->tuple.rem_addr[i], 1, 1, record->fp));
    }
    for (i =0; i < 16; i++) {
	    Chk(Fwrite(NULL, &data->tuple.local_addr[i], 1, 1, record->fp));
    }
    Chk(Fwrite(NULL, &data->tuple.rem_port, 2, 1, record->fp));
    Chk(Fwrite(NULL, &data->tuple.local_port, 2, 1, record->fp));
    
    Chk(Fwrite(NULL, data->val, (size_t)((data->length)*sizeof(estats_val)), 1, record->fp));

Cleanup:

    return err;
}

static void
_estats_swap_val(estats_val* val, ESTATS_VAL_TYPE valtype)
{
    switch (valtype) {

    case ESTATS_SIGNED32:
        val->sv32 = bswap_32(val->sv32);
        break;
    case ESTATS_UNSIGNED32:
        val->uv32 = bswap_32(val->uv32);
        break;
    case ESTATS_UNSIGNED64:
        val->uv64 = bswap_64(val->uv64);
        break;
    case ESTATS_UNSIGNED16:
        val->uv16 = bswap_16(val->uv16);
        break;
    default:
        break;
    }

    return;
}
static estats_error*
_estats_record_new(estats_record** record)
{
    estats_error* err = NULL;

    ErrIf(record == NULL, ESTATS_ERR_INVAL);
    *record = NULL;

    Chk(Malloc((void**) record, sizeof(estats_record)));
    memset((void*) *record, 0, sizeof(estats_record));

//    *record->bufsize = len*sizeof(struct estats_val);

Cleanup:
    return err;
}

static estats_error* _estats_record_parse_header(estats_record* _record, FILE* _fp);

static estats_error*
_estats_record_open_read(estats_record* record, const char* path)
{
    estats_error* err = NULL;
    FILE* header = NULL;
    uint16_t h_siz;
    uint16_t sum;
    int c;
    uint8_t end;

    ErrIf(record == NULL, ESTATS_ERR_INVAL);

    Chk(Fopen(&(record->fp), path, "r"));

    Chk(_estats_record_calculate_checksum(record, &sum));
    ErrIf(sum != 0, ESTATS_ERR_CHKSUM);

    Chk(Fread(NULL, &end, 1, 1, record->fp));

    Chk(Fread(NULL, &h_siz, 2, 1, record->fp));
    if (end != RECORD_HOST_ORDER) {
        record->swap = 1;
        h_siz = bswap_16(h_siz);
    }

    Chk(Fopen(&header, "./record_header", "w+"));

    /* position filepos past header, while copying header to tmp file */
    while (h_siz-- > 0) {
        c = fgetc(record->fp);
       	Chk(Fputc(c, header));
    }

    /* rewind and check the tmp file (header) for correctness */
    Chk(Fseek(header, 0, SEEK_SET)); // rewind

    Chk(_estats_record_parse_header(record, header));

    record->mode = R_MODE;

Cleanup: 
    Fclose(&header);
    Remove("./record_header");

    return err;
}

static estats_error*
_estats_record_open_write(estats_record* record, const char* path)
{
    estats_error* err = NULL; 
    FILE* header = NULL;
    int c;
    uint16_t h_siz = 0;
    uint8_t end = RECORD_HOST_ORDER;
    fpos_t pos;

    ErrIf(record == NULL, ESTATS_ERR_INVAL);

    Chk(Fopen(&record->fp, path, "w+")); // need to read back for checksum calc 

    Chk(Fwrite(NULL, &end, 1, 1, record->fp)); // indicate endianness of write

    Chk(Fwrite(NULL, &h_siz, 2, 1, record->fp)); // place holder for size of header

    ErrIf(fgetpos(record->fp, &pos) != 0, ESTATS_ERR_LIBC);

    Chk(Fseek(record->fp, 1, SEEK_SET)); // rewind to write size of header

    Chk(Fwrite(NULL, &h_siz, 2, 1, record->fp));

    ErrIf(fsetpos(record->fp, &pos) != 0, ESTATS_ERR_LIBC);

    record->mode = W_MODE;

Cleanup:
    Fclose(&header);

    if (err != NULL) {
        _estats_record_free(&record);
    }

    return err;
}

static estats_error*
_estats_record_calculate_checksum(estats_record* record, uint16_t* csum)
{
    estats_error* err = NULL;
    uint8_t* buf = NULL;
    uint16_t word16;
    uint32_t sum;
    int pad = 0;
    struct stat st;
    off_t size;
    int i;

    Chk(Fseek(record->fp, 0, SEEK_SET));

    fstat(fileno(record->fp), &st);
    size = st.st_size;

    Chk(Calloc((void**) &buf, size+1, sizeof(uint8_t)));

    Chk(Fread(NULL, buf, size, 1, record->fp));

    if ((size | 1) == size) {
	pad = 1;
    }

    sum = 0;
    for (i=0; i<size+pad; i=i+2) {
       	word16 =((buf[i] << 8) & 0xFF00) + (buf[i+1] & 0x00FF); 
	sum = sum + (uint32_t) word16;
    }

    while (sum>>16) {
       	sum = (sum & 0xFFFF) + (sum >> 16);
    }

    word16 = sum;
    if (pad) word16 = ((word16 & 0x00FF) << 8) + ((word16 & 0xFF00) >> 8);

    *csum = (~word16) & 0xFFFF;

Cleanup:
    Free((void**) &buf); 
    Fseek(record->fp, 0, SEEK_SET);

    return err;
}


static estats_error*
_estats_record_write_checksum(estats_record* record)
{
    estats_error* err = NULL;
    uint16_t sum;
    uint8_t sum1, sum2;

    ErrIf(record == NULL || record->fp == NULL || record->mode != W_MODE, ESTATS_ERR_INVAL);

    Chk(_estats_record_calculate_checksum(record, &sum));

    sum1 = (sum & 0xFF00) >> 8;
    sum2 = sum & 0xFF;

    Chk(Fseek(record->fp, 0, SEEK_END));

    Chk(Fwrite(NULL, &sum1, sizeof(uint8_t), 1, record->fp));
    Chk(Fwrite(NULL, &sum2, sizeof(uint8_t), 1, record->fp));

Cleanup:
    
    return err;
}

static void
_estats_record_free(estats_record** record)
{
    if (record == NULL || *record == NULL) return;

    Fclose(&(*record)->fp);
    Free((void**)record);
}

estats_error*
_estats_record_parse_header(estats_record* record, FILE* fp)
{
    estats_error* err = NULL;

    char linebuf[256];
    int have_len = 0;

    ErrIf(record == NULL || fp == NULL, ESTATS_ERR_INVAL);


Cleanup:

    return err;
}


