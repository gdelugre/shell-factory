#ifndef ARM_SOFTFLOAT_H_
#define ARM_SOFTFLOAT_H_

#include <cstdint>

extern "C" {

    #if defined(__ARM_EABI__)
    EXPORT_ABI_FUNCTION
    int32_t __aeabi_idiv(int32_t dividend, int32_t divisor)
    {
        return __divsi3(dividend, divisor);
    }

    EXPORT_ABI_FUNCTION
    uint32_t __aeabi_uidiv(uint32_t dividend, uint32_t divisor)
    {
        return __udivsi3(dividend, divisor);
    }

    EXPORT_ABI_FUNCTION
    uint64_t __aeabi_uidivmod(uint32_t dividend, uint32_t divisor)
    {
        uint64_t q = __aeabi_uidiv(dividend, divisor);
        uint64_t r = dividend - q * divisor;

        uint64_t result = q | (r << 32);
        return result;
    }
    #endif
}

#endif
