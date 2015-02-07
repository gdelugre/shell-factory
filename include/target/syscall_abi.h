#ifndef _GENERIC_SYSCALL_ABI_H
#define _GENERIC_SYSCALL_ABI_H

#if defined(__linux__)
#include <target/linux/syscall_abi.h>
#else
#error "No syscall ABI defined for this operating system."
#endif

#endif
