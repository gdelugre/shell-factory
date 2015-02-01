#ifndef _CPU_HELPER_H
#define _CPU_HELPER_H

#include <stdint.h>

#if defined(__arm__)
#include "arch/arm.c"
#elif defined(__amd64__)
#include "arch/amd64.c"
#elif defined(__386__)
#include "arch/i386.c"
#elif defined(__mips__) && (_MIPS_SZPTR == 32)
#include "arch/mips.c"
#endif

#define STACK_SHIFT(delta) set_sp(get_sp() + delta)

FUNCTION
uint16_t cpu_to_be16(uint16_t v)
{
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    return v;
#else
    return (((v) << 8) | ((v) >> 8));
#endif
}

FUNCTION
uint16_t cpu_to_le16(uint16_t v)
{
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    return v;
#else
    return (((v) << 8) | ((v) >> 8));
#endif
}

FUNCTION
uint32_t cpu_to_be32(uint32_t v)
{
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    return v;
#else
    return (((v) << 24) | (((v) & 0xff00) << 8) | (((v) & 0xff0000) >> 8) | ((v) >> 24));
#endif
}

FUNCTION
uint32_t cpu_to_le32(uint32_t v)
{
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    return v;
#else
    return (((v) << 24) | (((v) & 0xff00) << 8) | (((v) & 0xff0000) >> 8) | ((v) >> 24));
#endif
}

#endif
