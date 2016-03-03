#ifndef MEMOPS_H_
#define MEMOPS_H_

#include <cstddef>

//
// User-defined literals helpers for representing standard sizes.
//

constexpr size_t operator ""_KB(unsigned long long kbs)
{
    return 1024 * kbs;
}

constexpr size_t operator ""_MB(unsigned long long mbs)
{
    return 1024 * 1024 * mbs;
}

constexpr size_t operator ""_GB(unsigned long long gbs)
{
    return 1024 * 1024 * 1024 * gbs;
}

//
// Generic functions.
//
template <typename T>
FUNCTION PURE
size_t tstrlen(T *s)
{
    size_t len = 0;

    while ( s[len] )
        len++;

    return len;
}

template <typename T>
FUNCTION
T *tstrcpy(T *__restrict__ dest, const T *__restrict__ src)
{
    T c;
    size_t i = 0;

    while ( (c = src[i]) != 0 )
        dest[i++] = c;

    dest[i] = 0;
    return dest;
}

template <typename T>
FUNCTION
T *tstrncpy(T *__restrict__ dest, const T *__restrict__ src, size_t n)
{
    T c;
    size_t i = 0;

    while ( i < n && (c = src[i]) != 0 )
        dest[i++] = c;

    while ( i < n )
        dest[i++] = 0;

    return dest;
}

template <typename T>
FUNCTION
T *tmemcpy(T *__restrict__ dest, const T *__restrict__ src, size_t n)
{
    T *dptr = dest;
    const T *sptr = src;

    for ( size_t i = 0; i < n; i++ )
        dptr[i] = sptr[i];

    return dest;
}

template <typename T>
FUNCTION
T *tmemmove(T *dest, const T *src, size_t n)
{
    if ( src > dest )
        return tmemcpy(dest, src, n);

    T *dptr = dest;
    const T *sptr = src;

    for ( size_t i = 0; i < n; i++ )
        dptr[n - 1 - i] = sptr[n - 1 - i];

    return dest;
}

template <typename T>
FUNCTION
T *tmemset(T *s, T c, size_t n)
{
    T *dptr = s;

    for ( size_t i = 0; i < n; i++ )
        dptr[i] = c;

    return s;
}

template <typename T>
FUNCTION PURE
int tmemcmp(const T *s1, const T *s2, size_t n)
{
    for ( size_t i = 0; i < n; i++ )
    {
        if ( s1[i] < s2[i] )
            return -1;
        else if ( s1[i] > s2[i] )
            return +1;
    }

    return 0;
}

//
// Exported implementation of standard memory functions.
// The compiler may generate external call to them in different circumstances
// (e.g copying function parameters, initializing data...).
//
// The following implementations are not optimized for speed.
//
extern "C" {

    EXPORT_ABI_FUNCTION
    void *memcpy(void *__restrict__ dest, const void *__restrict__ src, size_t n)
    {
        return tmemcpy(
            static_cast<char *>(dest),
            static_cast<const char *>(src),
            n
        );
    }

    EXPORT_ABI_FUNCTION
    wchar_t *wmemcpy(wchar_t *__restrict__ dest, const wchar_t *__restrict__ src, size_t n)
    {
        return tmemcpy(dest, src, n);
    }

    EXPORT_ABI_FUNCTION
    void *memmove(void *dest, const void *src, size_t n)
    {
        return tmemmove(
            static_cast<wchar_t *>(dest),
            static_cast<const wchar_t *>(src),
            n
        );
    }

    EXPORT_ABI_FUNCTION
    wchar_t *wmemmove(wchar_t *dest, const wchar_t *src, size_t n)
    {
        return tmemmove(dest, src, n);
    }

    EXPORT_ABI_FUNCTION
    void *memset(void *s, int c, size_t n)
    {
        return tmemset(static_cast<char *>(s), (char) c, n);
    }

    EXPORT_ABI_FUNCTION
    wchar_t *wmemset(wchar_t *wcs, wchar_t wc, size_t n)
    {
        return tmemset(wcs, wc, n);
    }

    EXPORT_ABI_FUNCTION PURE
    int memcmp(const void *s1, const void *s2, size_t n)
    {
        return tmemcmp(
            static_cast<const char *>(s1),
            static_cast<const char *>(s2),
            n
        );
    }

    EXPORT_ABI_FUNCTION PURE
    int wmemcmp(const wchar_t *s1, const wchar_t *s2, size_t n)
    {
        return tmemcmp(s1, s2, n);
    }

    EXPORT_ABI_FUNCTION PURE
    size_t strlen(const char *s)
    {
        return tstrlen(s);
    }

    EXPORT_ABI_FUNCTION PURE
    size_t wcslen(const wchar_t *s)
    {
        return tstrlen(s);
    }

    EXPORT_ABI_FUNCTION
    char *strcpy(char *__restrict__ dest, const char *__restrict__ src)
    {
        return tstrcpy(dest, src);
    }

    EXPORT_ABI_FUNCTION
    wchar_t *wcscpy(wchar_t *__restrict__ dest, const wchar_t *src)
    {
        return tstrcpy(dest, src);
    }

    EXPORT_ABI_FUNCTION
    char *strncpy(char *__restrict__ dest, const char *__restrict__ src, size_t n)
    {
        return tstrncpy(dest, src, n);
    }

    EXPORT_ABI_FUNCTION
    wchar_t *wcsncpy(wchar_t *__restrict__ dest, const wchar_t *__restrict__ src, size_t n)
    {
        return tstrncpy(dest, src, n);
    }
}

#endif
