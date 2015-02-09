#ifndef _LINUX_SYSCALL_ABI_H
#define _LINUX_SYSCALL_ABI_H

/* Syscall definitions */
#include <sys/syscall.h>

#define SYSCALL_NAME_TO_NUM(name) __NR_##name
#define SYSCALL_EXISTS(name) defined(__NR_##name)

#if defined(__i386__)
#include <target/linux/i386/syscall_abi.h>
#elif defined(__amd64__)
#include <target/linux/amd64/syscall_abi.h>
#elif defined(__arm__)
#include <target/linux/arm/syscall_abi.h>
#elif defined(__aarch64__)
#include <target/linux/aarch64/syscall_abi.h>
#elif defined(__mips__)
#include <target/linux/mips/syscall_abi.h>
#else
#error "No syscall ABI defined for this architecture."
#endif

#endif
