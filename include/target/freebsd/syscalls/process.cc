#ifndef SYSCALL_PROCESS_H_
#define SYSCALL_PROCESS_H_

#include <sys/ptrace.h>
#include <sys/thr.h>

/*
 * This file defines FreeBSD specific system calls (i.e. not POSIX).
 */
namespace Syscall {

    SYSTEM_CALL int             thr_self(lwpid_t *);
    SYSTEM_CALL int             thr_set_name(lwpid_t, const char *);
    SYSTEM_CALL int             thr_new(struct thr_param *, int);
    SYSTEM_CALL long            ptrace(int, pid_t, void *, void *);
    SYSTEM_CALL pid_t           wait4(pid_t, int *, int, struct rusage *);
    SYSTEM_CALL int             thr_kill(lwpid_t, int);
    NO_RETURN SYSTEM_CALL int   thr_exit(long *state);

    SYSTEM_CALL
    int thr_self(lwpid_t *tid)
    {
        return DO_SYSCALL(thr_self, tid);
    }

    SYSTEM_CALL
    int thr_set_name(lwpid_t tid, const char *name)
    {
        return DO_SYSCALL(thr_set_name, tid, name);
    }

    SYSTEM_CALL
    int thr_new(struct thr_param *param, int param_size)
    {
        return DO_SYSCALL(thr_new, param, param_size);
    }

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

    SYSTEM_CALL
    int thr_kill(lwpid_t tid, int sig)
    {
        return DO_SYSCALL(thr_kill, tid, sig);
    }

    NO_RETURN SYSTEM_CALL
    int thr_exit(long *state)
    {
        DO_SYSCALL(thr_exit, state);
        __builtin_unreachable();
    }
}

#endif
