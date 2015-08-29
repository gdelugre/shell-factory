#ifndef SOFTFLOAT_H_
#define SOFTFLOAT_H_

#include <limits>
#include <type_traits>
#include <stdint.h>

namespace Math {

    template <typename T>
    FUNCTION
    unsigned clz(T w)
    {
        static_assert(std::numeric_limits<T>::is_integer, "Cannot use clz with non-integer types.");
        static_assert(sizeof(T) <= sizeof(unsigned int) || 
                      sizeof(T) == sizeof(unsigned long) ||
                      sizeof(T) == sizeof(unsigned long long), "clz not supported for this type size.");

        if ( sizeof(T) <= sizeof(unsigned int) )
            return __builtin_clz(static_cast<unsigned int>(w));
        else if ( sizeof(T) == sizeof(unsigned long) )
            return __builtin_clzl(static_cast<unsigned long>(w));
        else if ( sizeof(T) == sizeof(unsigned long long) )
            return __builtin_clzll(static_cast<unsigned long long>(w));
    }

    //
    // Algorithm taken from LLVM arm softfloat implementation.
   //
    template <typename T>
    FUNCTION
    T soft_udiv(T a, T b)
    {
        static_assert(std::numeric_limits<T>::is_integer, "Cannot use soft_udiv with non-integer types.");
        static_assert(!std::numeric_limits<T>::is_signed, "Cannot use soft_udiv with a signed integer type");
        assert(b != 0);

        unsigned clz_a = clz(a), clz_b = clz(b);

        // If MSB of b is greater than MSB of a, then a / b = 0.
        if ( clz_a > clz_b )
            return 0;

        T q = 0;
        int i = clz_b - clz_a;
        do {
            if ( a >= b << i )
            {
                a -= b << i;
                q |= 1 << i; 

                if ( a == 0 )
                    break;
            }
        } while ( --i >= 0 );

        return q;
    }

    template <typename T1, typename T2>
    FUNCTION
    auto soft_div(T1 a, T2 b)
    {
        static_assert(std::numeric_limits<T1>::is_integer && 
                      std::numeric_limits<T2>::is_integer, "Cannot use soft_div with non-integer types.");

        char sign = 1;
        if ( a < 0 )
            sign *= -1;
        if ( b < 0 )
            sign *= -1;
        
        using result_type = decltype(a / b);
        using unsigned_type = typename std::make_unsigned<result_type>::type;

        unsigned_type n = (a < 0) ? -a : a;
        unsigned_type d = (b < 0) ? -b : b;

        result_type q = soft_udiv(n, d);

        return q * sign;
    }

    template <typename T1, typename T2>
    FUNCTION
    auto soft_mod(T1 a, T2 b)
    {
        static_assert(std::numeric_limits<T1>::is_integer && 
                      std::numeric_limits<T2>::is_integer, "Cannot use soft_mod with non-integer types.");

        using result_type = decltype(a % b);
        result_type r = a - soft_div(a, b) * b;

        return r;
    }
}

#endif
