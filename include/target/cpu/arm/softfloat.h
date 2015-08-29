#ifndef ARM_SOFTFLOAT_H_
#define ARM_SOFTFLOAT_H_

extern "C" {

    uint32_t __udivsi3(uint32_t dividend, uint32_t divisor)
    {
        return Math::soft_div(dividend, divisor);
    }

    uint32_t __umodsi3(uint32_t dividend, uint32_t divisor)
    {
        return Math::soft_mod(dividend, divisor);
    }

#if defined(__ARM_EABI__)
    uint32_t __aeabi_uidiv(uint32_t dividend, uint32_t divisor)
    {
        return __udivsi3(dividend, divisor);
    }

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
