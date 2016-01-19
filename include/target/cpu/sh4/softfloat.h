#ifndef SH4_SOFTFLOAT_H_
#define SH4_SOFTFLOAT_H_

extern "C" {

    EXPORT_ABI_FUNCTION
    uint32_t __clzsi2(uint32_t n)
    {
        return Bits::soft_clz(n);
    }

    EXPORT_ABI_FUNCTION
    uint32_t __udivsi3_i4i(uint32_t a, uint32_t b)
    {
        return Math::soft_udiv(a, b);
    }

    EXPORT_ABI_FUNCTION
    int32_t __sdivsi3_i4i(int32_t a, int32_t b)
    {
        return Math::soft_div(a, b);
    }
}

#endif
