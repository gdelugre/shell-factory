#ifndef FREEBSD_SYSCALLS_H_
#define FREEBSD_SYSCALLS_H_

#include <sys/errno.h>

#define SYSCALL_NAME_TO_NUM(name) SYS_##name
#define SYSCALL_EXISTS(name) defined(SYS_##name)

namespace Target {

    using error_code = int;
    constexpr error_code max_error = ELAST;

    template <typename T>
    FUNCTION
    constexpr bool is_error(T err)
    {
        return Options::disable_error_checks ? false :
               (unsigned long)(err) >= (unsigned long)(-max_error);
    }

    FUNCTION
    constexpr long set_error(long err)
    {
        return -1 * err;
    }
}

#if defined(__amd64__)
#include <target/freebsd/amd64/syscall_abi.h>
#else
#error "No syscall ABI defined for this architecture."
#endif

/* Syscall definitions */
#include <sys/syscall.h>

#include "syscalls/sys.cc"
#include "syscalls/io.cc"
#include "syscalls/process.cc"
#include "syscalls/memory.cc"
#include "syscalls/socket.cc"
#include "syscalls/ipc.cc"
#include "syscalls/security.cc"

#endif
