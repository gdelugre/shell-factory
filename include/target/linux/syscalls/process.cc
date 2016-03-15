#ifndef SYSCALL_PROCESS_H_
#define SYSCALL_PROCESS_H_

#include <sys/wait.h>
#include <linux/sched.h>
#include <linux/limits.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <sys/prctl.h>

/*
 * This file defines Linux specific system calls (i.e. not POSIX).
 */
namespace Syscall {

    SYSTEM_CALL pid_t           gettid(void);
    #if SYSCALL_EXISTS(execveat)
    NO_RETURN SYSTEM_CALL int   execveat(int, const char *, char *const[], char *const[], int);
    #endif
    SYSTEM_CALL_INLINE long     clone(unsigned long, void *, void *, void *, void *);
    SYSTEM_CALL int             prctl(int, unsigned long, unsigned long, unsigned long, unsigned long);
    SYSTEM_CALL long            ptrace(enum __ptrace_request, pid_t, void *, void *);
    SYSTEM_CALL pid_t           wait4(pid_t, int *, int, struct rusage *);
    SYSTEM_CALL int             waitid(idtype_t, id_t, siginfo_t *, int, struct rusage *);
    SYSTEM_CALL int             tkill(pid_t, int);
    SYSTEM_CALL int             rt_sigaction(int, const struct sigaction *, struct sigaction *, size_t);
    SYSTEM_CALL int             sigaction(int, const struct sigaction *, struct sigaction *);
    NO_RETURN SYSTEM_CALL void  exit_group(int);

    SYSTEM_CALL
    pid_t gettid(void)
    {
        return DO_SYSCALL(gettid);
    }

    #if SYSCALL_EXISTS(execveat)
    NO_RETURN SYSTEM_CALL
    int execveat(int dirfd, const char *pathname, char *const argv[], char *const envp[], int flags)
    {
        DO_SYSCALL(execveat, dirfd, pathname, argv, envp, flags);
        __builtin_unreachable();
    }
    #endif

    //
    // clone has the ability to change the child stack.
    // It must be always be inlined since it cannot return from another stack.
    //
    SYSTEM_CALL_INLINE
    long clone(unsigned long flags, void *child_stack, void *ptid, void *tls, void *ctid)
    {
        return DO_SYSCALL(clone, flags, child_stack, ptid, tls, ctid);
    }

    SYSTEM_CALL
    int prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5)
    {
        return DO_SYSCALL(prctl, option, arg2, arg3, arg4, arg5);
    }

    SYSTEM_CALL
    long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data)
    {
        return DO_SYSCALL(ptrace, request, pid, addr, data);
    }

    SYSTEM_CALL
    pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage)
    {
        return DO_SYSCALL(wait4, pid, status, options, rusage);
    }

    SYSTEM_CALL
    int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options, struct rusage *rusage)
    {
        return DO_SYSCALL(waitid, idtype, id, infop, options, rusage);
    }

    SYSTEM_CALL
    int tkill(pid_t tid, int sig)
    {
        return DO_SYSCALL(tkill, tid, sig);
    }

    SYSTEM_CALL
    int rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oact, size_t sigsetsize)
    {
        return DO_SYSCALL(rt_sigaction, signum, act, oact, sigsetsize);
    }

    // POSIX wrapper.
    SYSTEM_CALL
    int sigaction(int signum, const struct sigaction *act, struct sigaction *oact)
    {
        return rt_sigaction(signum, act, oact, sizeof(sigset_t));
    }

    NO_RETURN SYSTEM_CALL
    void exit_group(int status)
    {
        DO_SYSCALL(exit_group, status);
        __builtin_unreachable();
    }
}

#endif
