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
        char *dptr = static_cast<char *>(dest);
        const char *sptr = static_cast<const char *>(src);

        for ( size_t i = 0; i < n; i++ )
            dptr[i] = sptr[i];

        return dest;
    }

    EXPORT_ABI_FUNCTION
    void *memmove(void *dest, const void *src, size_t n)
    {
        if ( src > dest )
            return memcpy(dest, src, n);

        char *dptr = static_cast<char *>(dest);
        const char *sptr = static_cast<const char *>(src);

        for ( size_t i = 0; i < n; i++ )
            dptr[n - 1 - i] = sptr[n - 1 - i];

        return dest;
    }

    EXPORT_ABI_FUNCTION
    void *memset(void *s, int c, size_t n)
    {
        char *dptr = static_cast<char *>(s);

        for ( size_t i = 0; i < n; i++ )
            dptr[i] = c;

        return s;
    }

    EXPORT_ABI_FUNCTION
    size_t strlen(const char *s)
    {
        size_t len = 0;

        while ( s[len] )
            len++;

        return len;
    }
}

#endif
