#ifndef SIZES_H_
#define SIZES_H_

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

#endif 
