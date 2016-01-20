#ifndef SOFTFLOAT_H_
#define SOFTFLOAT_H_

#include <limits>
#include <type_traits>
#include <cstdint>

namespace Bits {

    // Find most-significant bit set.
    template <typename T>
    FUNCTION
    unsigned soft_fls(T w)
    {
        static_assert(std::numeric_limits<T>::is_integer, "Cannot use soft_fls with non-integer types.");
        static_assert(sizeof(T) <= sizeof(unsigned int) ||
                      sizeof(T) == sizeof(unsigned long) ||
                      sizeof(T) == sizeof(unsigned long long), "fls not supported for this type size.");

        using value_type = typename std::make_unsigned<T>::type;
        constexpr size_t nr_bits = sizeof(value_type) * 8;

        unsigned count = sizeof(value_type) * 8;
        value_type value = w;

        if ( value == 0 )
            return 0;

        // Unrolled version of:
        //
        //  size_t shift = nr_bits / 2;
        //  while ( shift != 0 ) {
        //      if ( value >> (nr_bits - shift) == 0 ) {
        //          value <<= shift;
        //          count -= shift;
        //      }
        //
        //      shift /= 2;
        //  }
        //
        //  Compact version not used here because some architectures do not like arbitrary sized shifts.

        if ( nr_bits == 64 && (value >> (nr_bits - 32)) == 0) {
            value <<= 32;
            count -= 32;
        }

        if ( nr_bits >= 32 && (value >> (nr_bits - 16)) == 0) {
            value <<= 16;
            count -= 16;
        }

        if ( nr_bits >= 16 && (value >> (nr_bits - 8)) == 0) {
            value <<= 8;
            count -= 8;
        }

        if ( value >> (nr_bits - 4) == 0 ) {
            value <<= 4;
            count -= 4;
        }

        if ( value >> (nr_bits - 2) == 0 ) {
            value <<= 2;
            count -= 2;
        }

        if ( value >> (nr_bits - 1) == 0 ) {
            value <<= 1;
            count -= 1;
        }

        return count;
    }

    // Count leading zeros.
    template <typename T>
    FUNCTION
    unsigned soft_clz(T w)
    {
        return (sizeof(T) * 8) - soft_fls(w);
    }

    template <typename T>
    FUNCTION
    unsigned count_leading_zeros(T w)
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
}

namespace Math {

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

        unsigned clz_a = Bits::count_leading_zeros(a);
        unsigned clz_b = Bits::count_leading_zeros(b);

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

//
// Common softfloat arithmetic functions.
// See https://gcc.gnu.org/onlinedocs/gccint/Integer-library-routines.html
//
extern "C" {

    EXPORT_ABI_FUNCTION
    int __clzsi2(unsigned int a)
    {
        return Bits::soft_clz(a);
    }

    EXPORT_ABI_FUNCTION
    int __divsi3(int a, int b)
    {
        return Math::soft_div(a, b);
    }

    EXPORT_ABI_FUNCTION
    unsigned int __udivsi3(unsigned int a, unsigned int b)
    {
        return Math::soft_udiv(a, b);
    }

    EXPORT_ABI_FUNCTION
    int __modsi3(int a, int b)
    {
        return Math::soft_mod(a, b);
    }

    EXPORT_ABI_FUNCTION
    unsigned int __umodsi3(unsigned int a, unsigned int b)
    {
        return Math::soft_mod(a, b);
    }
}

#endif
