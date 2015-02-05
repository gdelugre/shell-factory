#ifndef _STRING_HELPER_H
#define _STRING_HELPER_H

#include <cstdarg>

#include "io.c"

FUNCTION
void *_memcpy(void *dest, const void *src, size_t n)
{
    if ( NO_BUILTIN )
    {
        unsigned int i;
        for ( i = 0; i < n; i++ )
            ((unsigned char *)dest)[i] = ((unsigned char *)src)[i];
        return dest;
    }
    else
        return BUILTIN(memcpy)(dest, src, n);
}

FUNCTION
void *_memset(void *s, int c, size_t n)
{
    if ( NO_BUILTIN )
    {
        unsigned int i;
        for ( i = 0; i < n; i++ )
            ((unsigned char *)s)[i] = c;
        return s;
    }
    else
        return BUILTIN(memset)(s, c, n);
}

FUNCTION
int _memcmp(const void *s1, const void *s2, size_t n)
{
    if ( NO_BUILTIN )
    {
        unsigned int i;
        for ( i = 0; i < n; i++ )
            if ( ((unsigned char *)s1)[i] < ((unsigned char *)s2)[i] )
                return -1;
            else
                return +1;
        return 0;
    }
    else
        return BUILTIN(memcmp)(s1, s2, n);
}

FUNCTION
void _bzero(void *s, size_t n)
{
    _memset(s, 0, n);
}

FUNCTION
size_t _strlen(const char *s)
{
    size_t len = 0;
    while ( s[len] )
        len++;
    return len;
}

FUNCTION
char *_strcpy(char *dest, const char *src)
{
    char c;
    char *out = dest;

    while ( (c = *src++) != '\0' )
        *out++ = c;

    *out = '\0';
    return dest;
}

FUNCTION
char *_strcat(char *dest, const char *src)
{
    _strcpy(&dest[_strlen(dest)], src);
    return dest;
}

FUNCTION
char *_strncpy(char *dest, const char *src, size_t n)
{
    char c;
    char *out = dest;

    while ( n != 0 && (c = *src++) != '\0' )
    {
        *out++ = c;
        n--;
    }

    if ( n > 0 )
        *out = '\0';

    return dest;
}

FUNCTION
int _strcmp(const char *s1, const char *s2)
{
    char c1 = 0, c2 = 0;
    
    do
    {
        c1 = *s1++;
        c2 = *s2++;

        if ( c1 != c2 )
            break; 
    } while ( c1 && c2 );

    if ( c1 == c2 )
        return 0;
    else
        return (c1 < c2 ? -1 : +1);
}

FUNCTION
char *_strstr(const char *haystack, const char *needle)
{
    char *ptr = (char *) haystack;
    while ( *ptr )
    {
        if ( _strcmp(ptr, needle) == 0 )
            return ptr;
        ptr++;
    }

    return NULL; 
}

FUNCTION
int _isdigit(int c)
{
    return (c >= '0' && c <= '9');
}

FUNCTION int _printf(const char *format, ...);


FUNCTION
unsigned int _atoi(const char *nptr)
{
    char *ptr = (char *) nptr;
    unsigned int num_digits = 0;
    unsigned int res = 0;
    unsigned int pow = 1;
    unsigned int i;

    while ( _isdigit(*ptr++) )
        num_digits++;

    for ( i = 0; i < num_digits; i++ )
    {
        res += (nptr[num_digits - i - 1] - '0') * pow;
        pow *= 10;
    }
    
    return res;
}

FUNCTION
size_t word_to_hex_str(char *str, unsigned long w)
{
    char hex_chars[] = "0123456789abcdef";
    const size_t bitsize = sizeof(w) * 8;
    int off = bitsize - 4;
    char *out = str;

    while ( off != 0 && w >> off == 0 )
        off -= 4;

    if ( off == 0 )
    {
        *out++ = '0';
    }
    else
    {
        while ( off >= 0 )
        {
            *out++ = hex_chars[(w >> off) & 0xF];
            off -= 4;
        }
    }

    return (out - str);
}

FUNCTION
size_t word_to_hex_fd(int fd, unsigned long w)
{
    char hex_chars[] = "0123456789abcdef";
    const size_t bitsize = sizeof(w) * 8;
    int off = bitsize - 4;
    size_t count = 0;

    while ( off != 0 && w >> off == 0 )
        off -= 4;

    if ( off == 0 )
    {
        Syscall::write(fd, &hex_chars[0], 1);
        count++;
    }
    else
    {
        while ( off >= 0 )
        {
            Syscall::write(fd, &hex_chars[(w >> off) & 0xF], 1);
            off -= 4;
            count++;
        }
    }

    return count; 
}

FUNCTION
int _vsprintf(char *str, const char *format, va_list ap)
{
    int escape = 0;
    char c;
    char *out = str;
    char *param_str;
    size_t param_str_sz;
    unsigned long param_word;

    while ( *format )
    {
        c = *format++;
        if ( !escape && c != '%' )
        {
            *out++ = c;
        }
        else if ( !escape && c == '%' )
        {
            escape = 1;
        }
        else
        {
            switch ( c )
            {
                case '%':
                    *out++ = '%'; break;

                case 's':
                    param_str = va_arg(ap, char *);
                    param_str_sz = _strlen(param_str);
                    _memcpy(out, param_str, param_str_sz);
                    out += param_str_sz;
                    escape = 0;
                    break;

                case 'p':
                case 'x':
                    param_word = va_arg(ap, unsigned long);
                    out += word_to_hex_str(out, param_word);
                    break;

                default:
                    break;
            }

            escape = 0;
        }
    }

    *out = '\0';
    return (out - str);
}

FUNCTION
int _vdprintf(int fd, const char *format, va_list ap)
{
    int escape = 0;
    char c;
    char *param_str;
    size_t param_str_sz;
    unsigned long param_word;
    size_t count = 0;

    while ( *format )
    {
        c = *format++;
        if ( !escape && c != '%' )
        {
            Syscall::write(fd, &c, 1);
            count++;
        }
        else if ( !escape && c == '%' )
        {
            escape = 1;
        }
        else
        {
            switch ( c )
            {
                case '%':
                    Syscall::write(fd, &c, 1); 
                    count++;
                    break;

                case 's':
                    param_str = va_arg(ap, char *);
                    param_str_sz = _strlen(param_str);
                    Syscall::write(fd, param_str, param_str_sz);
                    count += param_str_sz;
                    escape = 0;
                    break;

                case 'p':
                case 'x':
                    param_word = va_arg(ap, unsigned long);
                    count += word_to_hex_fd(fd, param_word);
                    break;

                default:
                    break;
            }

            escape = 0;
        }
    }

    Syscall::fsync(fd);
    return count;
}

FUNCTION
int _sprintf(char *str, const char *format, ...)
{
    int cnt;
    va_list ap;

    va_start(ap, format);
    cnt = _vsprintf(str, format, ap);
    va_end(ap);

    return cnt;
}

FUNCTION
int _dprintf(int fd, const char *format, ...)
{
    int cnt;
    va_list ap;

    va_start(ap, format);
    cnt = _vdprintf(fd, format, ap);
    va_end(ap);

    return cnt;
}

FUNCTION
int _printf(const char *format, ...)
{
    int cnt;
    va_list ap;

    va_start(ap, format);
    cnt = _vdprintf(stdout, format, ap);
    va_end(ap);

    return cnt;
}

#endif
