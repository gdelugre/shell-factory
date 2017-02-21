#ifndef DARWIN_SYSCALLS_H_
#define DARWIN_SYSCALLS_H_

#include <sys/errno.h>

// Constants taken from official XNU source.
// http://www.opensource.apple.com/source/xnu/xnu-792.13.8/osfmk/mach/i386/syscall_sw.h
#define SYSCALL_CLASS_SHIFT 24

enum DarwinSyscallClass {
    SYSCALL_CLASS_NONE = 0,
    SYSCALL_CLASS_MACH = 1,
    SYSCALL_CLASS_UNIX = 2,
    SYSCALL_CLASS_MDEP = 3,
    SYSCALL_CLASS_DIAG = 4,
};

// XXX: How to handle mach syscalls?
// Only assume BSD syscalls for now.
#define SYSCALL_NAME_TO_NUM(name) SYS_##name | (SYSCALL_CLASS_UNIX << SYSCALL_CLASS_SHIFT)
#define SYSCALL_EXISTS(name) defined(SYS_##name)

namespace Target {

    using error_code = int;
    constexpr error_code max_error = ELAST;

    template <typename T>
    FUNCTION
    constexpr bool is_error(T err)
    {
        return Options::disable_error_checks ? false :
               (unsigned long)(err) > 0 &&
               (unsigned long)(err) <= (unsigned long)(max_error);
    }
}

#if defined(__amd64__)
#include <target/darwin/amd64/syscall_abi.h>
#else
#error "No syscall ABI defined for this architecture."
#endif

/* Syscall definitions */
#include <sys/syscall.h>

/*
#include "syscalls/sys.cc"
#include "syscalls/io.cc"
#include "syscalls/process.cc"
#include "syscalls/memory.cc"
#include "syscalls/socket.cc"
#include "syscalls/ipc.cc"
#include "syscalls/security.cc"
*/

#endif
