/*
 * Copyright (c) 2011 The Board of Trustees of the University of Illinois,
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
#include <estats/estats-int.h>


estats_error*
Access(const char* pathname, int mode)
{
    estats_error* err = NULL;
    int ret;
    
    ErrIf(pathname == NULL, TCPE_ERR_INVAL);
    if ((ret = access(pathname, mode)) == -1)
        Err2(TCPE_ERR_LIBC, pathname);

 Cleanup:
    return err;
}


estats_error*
Asprintf(int* ret, char** strp, const char* fmt, ...)
{
    estats_error* err = NULL;
    va_list ap;

    va_start(ap, fmt);
    Chk(Vasprintf(ret, strp, fmt, ap));
    va_end(ap);

 Cleanup:
    return err;
}


estats_error*
Calloc(void** ptr, size_t nmemb, size_t size)
{
    estats_error* err = NULL;

    ErrIf(ptr == NULL || size == 0 || nmemb == 0, TCPE_ERR_INVAL);
    if ((*ptr = calloc(nmemb, size)) == NULL)
        Err(TCPE_ERR_LIBC);
    
 Cleanup:
    return err;
}


void
Closedir(DIR** dir)
{
    if (dir == NULL || *dir == NULL)
        return;
   
    closedir(*dir);
    *dir = NULL;
}


void
Fclose(FILE** fp)
{
    if (fp == NULL || *fp == NULL)
        return;

    fclose(*fp);
    *fp = NULL;
}


estats_error*
Fgets(char* s, int size, FILE* fp)
{
    estats_error* err = NULL;

    if (fgets(s, size, fp) == NULL) {
        if (feof(fp))
            Err(TCPE_ERR_EOF);
        else
            Err(TCPE_ERR_LIBC);
    }

Cleanup:
    return err;
}


estats_error*
Fopen(FILE** fp, const char* path, const char* mode)
{
    estats_error* err = NULL;

    ErrIf(fp == NULL || path == NULL || mode == NULL, TCPE_ERR_INVAL);
    if ((*fp = fopen(path, mode)) == NULL)
        Err2(TCPE_ERR_LIBC, path);

 Cleanup:
    return err;
}


estats_error*
Fprintf(int* ret, FILE* fp, const char* fmt, ...)
{
    estats_error* err = NULL;
    va_list ap;

    va_start(ap, fmt);
    Chk(Vfprintf(ret, fp, fmt, ap));
    va_end(ap);

Cleanup:
    return err;
}


estats_error*
Fputc(int c, FILE* fp)
{
    estats_error* err = NULL;

    if (fputc(c, fp) == EOF)
        Err(TCPE_ERR_LIBC);

Cleanup:
    return err;
}


estats_error*
Fread(size_t* ret, void* ptr, size_t size, size_t nmemb, FILE* fp)
{
    estats_error* err = NULL;
    size_t r;

    if ((r = fread(ptr, size, nmemb, fp)) < nmemb) {
	if (feof(fp))
	    Err(TCPE_ERR_EOF);
      	else
	    Err(TCPE_ERR_LIBC);
    }
    if (ret != NULL)
        *ret = r;

Cleanup:
    return err;
}


void
Free(void** ptr)
{
    if (ptr == NULL || *ptr == NULL)
        return;

    free(*ptr);
    *ptr = NULL;
}


estats_error*
Fscanf(int* ret, FILE* fp, const char* fmt, ...)
{
    estats_error* err = NULL;
    va_list ap;

    va_start(ap, fmt);
    Chk(Vfscanf(ret, fp, fmt, ap));
    va_end(ap);

 Cleanup:
    return err;
}


estats_error*
Fseek(FILE* fp, long offset, int whence)
{
    estats_error* err = NULL;
    
    if (fseek(fp, offset, whence) == -1)
        Err(TCPE_ERR_LIBC);

Cleanup:
    return err;
}


estats_error*
Fwrite(size_t* ret, const void* ptr, size_t size, size_t nmemb, FILE* fp)
{
    estats_error* err = NULL;
    size_t r;

    if ((r = fwrite(ptr, size, nmemb, fp)) < nmemb)
        Err(TCPE_ERR_LIBC);
    if (ret != NULL)
        *ret = r;

Cleanup:
    return err;
}


estats_error*
Getpeername(int s, struct sockaddr* name, socklen_t* namelen)
{
    estats_error* err = NULL;
    int ret;

    if ((ret = getpeername(s, name, namelen)) == -1)
        Err(TCPE_ERR_LIBC);

Cleanup:
    return err;
}


estats_error*
Getsockname(int s, struct sockaddr* name, socklen_t* namelen)
{
    estats_error* err = NULL;
    int ret;

    if ((ret = getsockname(s, name, namelen)) == -1)
        Err(TCPE_ERR_LIBC);

Cleanup:
    return err;
}


estats_error*
Inet_ntop(int af, const void* src, char* dst, size_t cnt)
{
    estats_error* err = NULL;
    const char* ret;

    if ((ret = inet_ntop(af, src, dst, cnt)) == NULL)
        Err(TCPE_ERR_LIBC);

 Cleanup:
    return err;
}


estats_error*
Inet_pton(int af, const char* src, void* dst)
{
    estats_error* err = NULL;
    int ret;

    if ((ret = inet_pton(af, src, dst)) <= 0)
        Err(TCPE_ERR_LIBC);

 Cleanup:
    return err;
}


estats_error*
Malloc(void** ptr, size_t size)
{
    estats_error* err = NULL;

    ErrIf(ptr == NULL || size == 0, TCPE_ERR_INVAL);
    if ((*ptr = malloc(size)) == NULL)
        Err(TCPE_ERR_LIBC);
    
 Cleanup:
    return err;
}


estats_error*
Opendir(DIR** dir, const char* name)
{
    estats_error* err = NULL;

    if ((*dir = opendir(name)) == NULL)
        Err(TCPE_ERR_LIBC);

Cleanup:
    return err;
}


estats_error*
Remove(const char* pathname)
{
    estats_error* err = NULL;
    int ret;

    ErrIf(pathname == NULL, TCPE_ERR_INVAL);
    if ((ret = remove(pathname)) == -1)
        Err(TCPE_ERR_LIBC);

Cleanup:
    return err;
}


estats_error*
Sprintf(int* ret, char* str, const char* fmt, ...)
{
    estats_error* err = NULL;
    va_list ap;

    va_start(ap,fmt);
    Chk(Vsprintf(ret, str, fmt, ap));
    va_end(ap);

Cleanup:
    return err;
}


estats_error*
Snprintf(int* ret, char* str, size_t size, const char* fmt, ...)
{
    estats_error* err = NULL;
    va_list ap;

    va_start(ap,fmt);
    Chk(Vsnprintf(ret, str, size, fmt, ap));
    va_end(ap);

Cleanup:
    return err;
}


estats_error*
Sscanf(int* ret, const char* str, const char* fmt, ...)
{
    estats_error* err = NULL;
    va_list ap;

    va_start(ap, fmt);
    Chk(Vsscanf(ret, str, fmt, ap));
    va_end(ap);

 Cleanup:
    return err;
}


estats_error*
Strdup(char** ret, const char* s)
{
    estats_error* err = NULL;

    ErrIf(ret == NULL || s == NULL, TCPE_ERR_INVAL);
    if ((*ret = strdup(s)) == NULL)
        Err(TCPE_ERR_LIBC);
    
 Cleanup:
    return err;
}


estats_error*
Strndup(char** ret, const char* s, size_t n)
{
    estats_error* err = NULL;

    ErrIf(ret == NULL || s == NULL, TCPE_ERR_INVAL);
    if ((*ret = strndup(s, n)) == NULL)
        Err(TCPE_ERR_LIBC);
    
 Cleanup:
    return err;
}


estats_error*
Strtol(long int* ret, const char* nptr, char** endptr, int base)
{
    estats_error* err = NULL;
    char* e;

    ErrIf(ret == NULL || nptr == NULL, TCPE_ERR_INVAL);
    *ret = strtol(nptr, &e, base);
    ErrUnless(*nptr != '\0' && *e == '\0', TCPE_ERR_STR_CONV);
    if (endptr != NULL)
        *endptr = e;
    
 Cleanup:
    return err;
}


estats_error*
Strtoul(unsigned long int* ret, const char* nptr, char** endptr, int base)
{
    estats_error* err = NULL;
    char* e;

    ErrIf(ret == NULL || nptr == NULL, TCPE_ERR_INVAL);
    *ret = strtoul(nptr, &e, base);
    ErrUnless(*nptr != '\0' && *e == '\0', TCPE_ERR_STR_CONV);
    if (endptr != NULL)
        *endptr = e;
    
 Cleanup:
    return err;
}


estats_error*
Strtoull(unsigned long long int* ret, const char* nptr, char** endptr, int base)
{
    estats_error* err = NULL;
    char* e;

    ErrIf(ret == NULL || nptr == NULL, TCPE_ERR_INVAL);
    *ret = strtoull(nptr, &e, base);
    ErrUnless(*nptr != '\0' && *e == '\0', TCPE_ERR_STR_CONV);
    if (endptr != NULL)
        *endptr = e;
    
 Cleanup:
    return err;
}


estats_error*
Vasprintf(int* ret, char** strp, const char* fmt, va_list ap)
{
    estats_error* err = NULL;
    int v;

    ErrIf(strp == NULL || fmt == NULL, TCPE_ERR_INVAL);
    if ((v = vasprintf(strp, fmt, ap)) == -1)
        Err(TCPE_ERR_LIBC);
    if (ret != NULL)
        *ret = v;

 Cleanup:
    return err;
}


estats_error*
Vfprintf(int* ret, FILE* fp, const char* fmt, va_list ap)
{
    estats_error* err = NULL;
    int v;

    ErrIf(fp == NULL || fmt == NULL, TCPE_ERR_INVAL);
    if ((v = vfprintf(fp, fmt, ap)) <= 0)
        Err(TCPE_ERR_LIBC);
    if (ret != NULL)
        *ret = v;

 Cleanup:
    return err;
}


estats_error*
Vfscanf(int* ret, FILE* fp, const char* fmt, va_list ap)
{
    estats_error* err = NULL;
    int v;

    ErrIf(fp == NULL || fmt == NULL, TCPE_ERR_INVAL);
    if ((v = vfscanf(fp, fmt, ap)) == EOF)
        Err(TCPE_ERR_LIBC);
    if (ret != NULL)
        *ret = v;

 Cleanup:
    return err;
}


estats_error*
Vsnprintf(int* ret, char* str, size_t size, const char* fmt, va_list ap)
{
    estats_error* err = NULL;
    int v;

    ErrIf(str == NULL || size == 0 || fmt == NULL, TCPE_ERR_INVAL);
    if ((v = vsnprintf(str, size, fmt, ap)) <= 0)
        Err(TCPE_ERR_LIBC);
    if (ret != NULL)
        *ret = v;

 Cleanup:
    return err;
}


estats_error*
Vsprintf(int* ret, char* str, const char* fmt, va_list ap)
{
    estats_error* err = NULL;
    int v;

    ErrIf(str == NULL || fmt == NULL, TCPE_ERR_INVAL);
    if ((v = vsprintf(str, fmt, ap)) <= 0)
        Err(TCPE_ERR_LIBC);
    if (ret != NULL)
        *ret = v;

 Cleanup:
    return err;
}


estats_error*
Vsscanf(int* ret, const char* str, const char* fmt, va_list ap)
{
    estats_error* err = NULL;
    int v;

    ErrIf(str == NULL || fmt == NULL, TCPE_ERR_INVAL);
    if ((v = vsscanf(str, fmt, ap)) == EOF)
        Err(TCPE_ERR_LIBC);
    if (ret != NULL)
        *ret = v;

 Cleanup:
    return err;
}
