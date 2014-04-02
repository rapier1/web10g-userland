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
#ifndef ESTATS_LIBCWRAP_INT_H
#define ESTATS_LIBCWRAP_INT_H

estats_error* Access(const char* _pathname, int _mode);
estats_error* Asprintf(int* _ret, char** _strp, const char* _fmt, ...);
estats_error* Calloc(void** _ptr, size_t nmemb, size_t size);
void          Closedir(DIR** _dir);
void          Fclose(FILE** _fp);
estats_error* Fgets(char* _s, int _size, FILE* _fp);
estats_error* Fopen(FILE** _fp, const char* _path, const char* _mode);
estats_error* Fprintf(int* _ret, FILE* _fp, const char* _fmt, ...);
estats_error* Fputc(int _c, FILE* _fp);
estats_error* Fread(size_t* _ret, void* _ptr, size_t _size, size_t _nmemb, FILE* _fp);
void          Free(void** _ptr);
estats_error* Fscanf(int* _ret, FILE* _stream, const char* _fmt, ...);
estats_error* Fseek(FILE* _fp, long _offset, int _whence);
estats_error* Fwrite(size_t* _ret, const void* _ptr, size_t _size, size_t _nmemb, FILE* _fp);
estats_error* Getpeername(int _s, struct sockaddr* _name, socklen_t* _namelen);
estats_error* Getsockname(int _s, struct sockaddr* _name, socklen_t* _namelen);
estats_error* Inet_ntop(int _af, const void* _src, char* _dst, size_t _cnt);
estats_error* Inet_pton(int _af, const char* _str, void* _dst);
estats_error* Malloc(void** _ptr, size_t _size);
estats_error* Opendir(DIR** _dir, const char* _name);
estats_error* Remove(const char* _pathname);
estats_error* Snprintf(int* _ret, char* _str, size_t _siz, const char* _fmt, ...);
estats_error* Sprintf(int* _ret, char* _str, const char* _fmt, ...);
estats_error* Sscanf(int* _ret, const char* _str, const char* _fmt, ...);
estats_error* Strdup(char** _ret, const char* _s);
estats_error* Strndup(char** _ret, const char* _s, size_t _n);
estats_error* Strtol(long int* _ret, const char* _nptr, char** _endptr, int _base);
estats_error* Strtoul(unsigned long int* _ret, const char* _nptr, char** _endptr, int _base);
estats_error* Strtoull(unsigned long long int* _ret, const char* _nptr, char** _endptr, int _base);
estats_error* Vasprintf(int* _ret, char** _strp, const char* _fmt, va_list _ap);
estats_error* Vfprintf(int* _ret, FILE* _fp, const char* _fmt, va_list _ap);
estats_error* Vfscanf(int* _ret, FILE* _stream, const char* _fmt, va_list _ap);
estats_error* Vsnprintf(int* _ret, char* _str, size_t _siz, const char* _fmt, va_list _ap);
estats_error* Vsprintf(int* _ret, char* _str, const char* _fmt, va_list _ap);
estats_error* Vsscanf(int* _ret, const char* _str, const char* _fmt, va_list _ap);

#endif /* ESTATS_LIBCWRAP_INT_H */
