#ifndef LINUX_SYSCALL_ABI_H_
#define LINUX_SYSCALL_ABI_H_

#define SYSCALL_NAME_TO_NUM(name) __NR_##name
#define SYSCALL_EXISTS(name) defined(__NR_##name)

namespace Target {

    using error_code = int;
    constexpr error_code max_error = 4095;

    template <typename T>
    FUNCTION
    constexpr bool is_error(T err)
    {
        return Options::disable_error_checks ? false :
               (unsigned long)(err) >= (unsigned long)(-max_error);
    }
}

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
#elif defined(__powerpc__)
#include <target/linux/ppc/syscall_abi.h>
#elif defined(__SH4__)
#include <target/linux/sh4/syscall_abi.h>
#else
#error "No syscall ABI defined for this architecture."
#endif

/* Syscall definitions */
#include <sys/syscall.h>

#include "syscalls/io.cc"
#include "syscalls/process.cc"
#include "syscalls/memory.cc"
#include "syscalls/socket.cc"
#include "syscalls/ipc.cc"
#include "syscalls/security.cc"

#endif
