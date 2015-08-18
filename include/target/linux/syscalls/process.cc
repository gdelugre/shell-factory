#ifndef SYSCALL_PROCESS_H_
#define SYSCALL_PROCESS_H_

#include <factory.h>

#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <linux/sched.h>
#include <linux/limits.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <sys/prctl.h>
#include <sys/time.h>

/*
 * System calls defined in this file.
 */
namespace Syscall {

    SYSTEM_CALL pid_t           getpid(void);
    SYSTEM_CALL pid_t           fork(void);
    NO_RETURN SYSTEM_CALL int   execve(const char *, char *const[], char *const[]);
    SYSTEM_CALL long            clone(unsigned long, void *, void *, void *, void *);
    SYSTEM_CALL int             prctl(int, unsigned long, unsigned long, unsigned long, unsigned long);
    #if SYSCALL_EXISTS(setitimer)
    SYSTEM_CALL int             setitimer(int, const struct itimerval *, struct itimerval *);
    #endif
    SYSTEM_CALL long            ptrace(enum __ptrace_request, pid_t, void *, void *);
    SYSTEM_CALL pid_t           wait4(pid_t, int *, int, struct rusage *);
    SYSTEM_CALL int             kill(pid_t, int);
    NO_RETURN SYSTEM_CALL void  exit_thread(int);
    NO_RETURN SYSTEM_CALL void  exit_process(int);

    SYSTEM_CALL
    pid_t getpid(void)
    {
        return DO_SYSCALL(getpid);
    }

    SYSTEM_CALL
    pid_t fork(void)
    {
        return DO_SYSCALL(fork);
    }

    NO_RETURN SYSTEM_CALL
    int execve(const char *filename, char *const argv[], char *const envp[])
    {
        DO_SYSCALL(execve, filename, argv, envp);
        __builtin_unreachable();
    }

    SYSTEM_CALL
    long clone(unsigned long flags, void *child_stack, void *ptid, void *tls, void *ctid)
    {
        return DO_SYSCALL(clone, flags, child_stack, ptid, tls, ctid);
    }

    SYSTEM_CALL
    int prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5)
    {
        return DO_SYSCALL(prctl, option, arg2, arg3, arg4, arg5);
    }

    #if SYSCALL_EXISTS(setitimer)
    SYSTEM_CALL
    int setitimer(int which, const struct itimerval *value, struct itimerval *ovalue)
    {
        return DO_SYSCALL(setitimer, which, value, ovalue);
    }
    #endif

    SYSTEM_CALL
    unsigned int alarm(unsigned int seconds)
    {
        #if SYSCALL_EXISTS(alarm)
        return DO_SYSCALL(alarm, seconds);
        #else
        struct itimerval itv = { {0,0}, {0,0} };

        itv.it_value.tv_sec = seconds;
        return Syscall::setitimer(ITIMER_REAL, &itv, &itv);
        #endif
    }

    SYSTEM_CALL
    int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
    {
        return DO_SYSCALL(rt_sigaction, signum, act, oldact, 8);
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
    int kill(pid_t pid, int sig)
    {
        return DO_SYSCALL(kill, pid, sig);
    }

    NO_RETURN SYSTEM_CALL
    void exit_thread(int status)
    {
        DO_SYSCALL(exit, status);
        __builtin_unreachable();
    }

    NO_RETURN SYSTEM_CALL
    void exit_process(int status)
    {
        DO_SYSCALL(exit_group, status);
        __builtin_unreachable();
    }
}

#endif
