#ifndef SH4_SOFTFLOAT_H_
#define SH4_SOFTFLOAT_H_

#include <cstdint>

extern "C" {

    EXPORT_ABI_FUNCTION
    uint32_t __udivsi3_i4i(uint32_t a, uint32_t b)
    {
        return __udivsi3(a, b);
    }

    EXPORT_ABI_FUNCTION
    int32_t __sdivsi3_i4i(int32_t a, int32_t b)
    {
        return __divsi3(a, b);
    }
}

#endif
