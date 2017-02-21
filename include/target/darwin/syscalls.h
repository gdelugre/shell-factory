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

#define DO_SYSCALL_WITH_CLASS(class, name, args...) EMIT_SYSCALL(SYSCALL_CLASS_##class, name, ##args)
#define DO_MACH_SYSCALL(name, args...) DO_SYSCALL_WITH_CLASS(MACH, name, ##args)
#define DO_BSD_SYSCALL(name, args...) DO_SYSCALL_WITH_CLASS(UNIX, name, ##args)
#define DO_SYSCALL(name, args...) DO_BSD_SYSCALL(name, ##args)
#define SYSCALL_NAME_TO_NUM(class, name) SYS_##name | (class << SYSCALL_CLASS_SHIFT)
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
#include <target/darwin/amd64/syscall_abi.h>
#else
#error "No syscall ABI defined for this architecture."
#endif

// Error definitions.
#include <errno.h>

// Syscall definitions.
#include <sys/syscall.h>

// Common POSIX system calls.
#include <target/posix/syscalls.h>

// Darwin specific system calls.
#include "syscalls/sys.cc"
#include "syscalls/io.cc"
#include "syscalls/process.cc"
#include "syscalls/ipc.cc"

#endif
