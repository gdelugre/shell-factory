#ifndef FREEBSD_SYSCALL_ABI_H_
#define FREEBSD_SYSCALL_ABI_H_

#define SYSCALL_NAME_TO_NUM(name) SYS_##name
#define SYSCALL_EXISTS(name) defined(SYS_##name)

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
