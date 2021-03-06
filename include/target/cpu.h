#ifndef GENERIC_CPU_DEFS_H_
#define GENERIC_CPU_DEFS_H_

#include <limits>

#if defined(__i386__)
#include <target/cpu/i386/cpu_defs.h>
#elif defined(__amd64__)
#include <target/cpu/amd64/cpu_defs.h>
#elif defined(__arm__)
#include <target/cpu/arm/aeabi.h>
#include <target/cpu/arm/cpu_defs.h>
#elif defined(__aarch64__)
#include <target/cpu/aarch64/cpu_defs.h>
#elif defined(__mips__)
#include <target/cpu/mips/cpu_defs.h>
#elif defined(__powerpc__)
#include <target/cpu/ppc/cpu_defs.h>
#elif defined(__SH4__)
#include <target/cpu/sh4/softfloat.h>
#include <target/cpu/sh4/cpu_defs.h>
#else
#error "No definitions for this CPU."
#endif

#define FLUSH_VARIABLE(var) \
    asm("" :: "r" (var));


namespace Stack {

    register CPU::reg_t __reg_ptr asm ( SP_REGISTER );

    FUNCTION_INLINE
    void *pointer()
    {
        return reinterpret_cast<void *>(Stack::__reg_ptr);
    }

    FUNCTION_INLINE
    void reset(void *ptr)
    {
        Stack::__reg_ptr = reinterpret_cast<CPU::reg_t>(ptr);
        FLUSH_VARIABLE(Stack::__reg_ptr);
    }

    FUNCTION_INLINE
    void *shift(long offset)
    {
        Stack::__reg_ptr += offset;
        FLUSH_VARIABLE(Stack::__reg_ptr);

        return Stack::pointer();
    }
}

namespace CPU {

    static constexpr bool big_endian = (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__);
    static constexpr bool little_endian = !big_endian;

    FUNCTION
    uint8_t bswap(uint8_t b)
    {
        return b;
    }

    FUNCTION
    uint16_t bswap(uint16_t h)
    {
        return BUILTIN(bswap16)(h);
    }

    FUNCTION
    uint32_t bswap(uint32_t d)
    {
        return BUILTIN(bswap32)(d);
    }

    FUNCTION
    uint64_t bswap(uint64_t q)
    {
        return BUILTIN(bswap64)(q);
    }

    template <typename T>
    FUNCTION
    T byte_swap(T num)
    {
        constexpr size_t num_size = sizeof(T) * 8;

        static_assert(std::numeric_limits<T>::is_integer, "Cannot use to_big_endian with non-integer types.");
        static_assert(num_size == 8 || num_size == 16 || num_size == 32 || num_size == 64,
                      "Invalid integer size in byte_swap");

        switch ( num_size ) {
            case 8:     return num;
            case 16:    return BUILTIN(bswap16)(num);
            case 32:    return BUILTIN(bswap32)(num);
            case 64:    return BUILTIN(bswap64)(num);
        }
    }

    template <typename T>
    FUNCTION
    T to_big_endian(T num)
    {
        static_assert(std::numeric_limits<T>::is_integer, "Cannot use to_big_endian with non-integer types.");

        if ( CPU::big_endian )
            return num;

        return byte_swap(num);
    }

    template <typename T>
    FUNCTION
    T to_little_endian(T num)
    {
        static_assert(std::numeric_limits<T>::is_integer, "Cannot use to_big_endian with non-integer types.");

        if ( CPU::little_endian )
            return num;

        return byte_swap(num);
    }
}

#endif
