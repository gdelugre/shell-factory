#ifndef CPU_HELPER_H_
#define CPU_HELPER_H_

#include <stdint.h>

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
