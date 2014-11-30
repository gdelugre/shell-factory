#ifndef _STRING_HELPER_H
#define _STRING_HELPER_H

FUNCTION
void *_memcpy(void *dest, const void *src, size_t n)
{
    int i;
    for ( i = 0; i < n; i++ )
        ((unsigned char *)dest)[i] = ((unsigned char *)src)[i];
    return dest;
}

FUNCTION
void *_memset(void *s, int c, size_t n)
{
    int i;
    for ( i = 0; i < n; i++ )
        ((unsigned char *)s)[i] = c;
    return s;
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

#endif
