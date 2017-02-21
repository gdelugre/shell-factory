#ifndef SYSCALL_PROCESS_H_
#define SYSCALL_PROCESS_H_

#include <sys/ptrace.h>

/*
 * This file defines Darwin specific system calls (i.e. not POSIX).
 */
namespace Syscall {

    SYSTEM_CALL long            ptrace(int, pid_t, void *, void *);
    SYSTEM_CALL pid_t           wait4(pid_t, int *, int, struct rusage *);

    SYSTEM_CALL
    long ptrace(int request, pid_t pid, void *addr, void *data)
    {
        return DO_SYSCALL(ptrace, request, pid, addr, data);
    }

    SYSTEM_CALL
    pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage)
    {
        return DO_SYSCALL(wait4, pid, status, options, rusage);
    }
}

#endif
