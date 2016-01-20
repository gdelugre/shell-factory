#ifndef SYSCALL_PROCESS_H_
#define SYSCALL_PROCESS_H_

#include <factory.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/time.h>
#include <sys/thr.h>

/*
 * System calls defined in this file.
 */
namespace Syscall {

    SYSTEM_CALL int             thr_self(lwpid_t *);
    SYSTEM_CALL int             thr_set_name(lwpid_t, const char *);
    SYSTEM_CALL pid_t           getpid(void);
    SYSTEM_CALL pid_t           getppid(void);
    SYSTEM_CALL pid_t           fork(void);
    NO_RETURN SYSTEM_CALL int   execve(const char *, char *const[], char *const[]);
    SYSTEM_CALL int             thr_new(struct thr_param *, int);
    SYSTEM_CALL int             setitimer(int, const struct itimerval *, struct itimerval *);
    SYSTEM_CALL long            ptrace(int, pid_t, void *, void *);
    SYSTEM_CALL pid_t           wait4(pid_t, int *, int, struct rusage *);
    SYSTEM_CALL int             thr_kill(lwpid_t, int);
    SYSTEM_CALL int             kill(pid_t, int);
    NO_RETURN SYSTEM_CALL int   thr_exit(long *state);
    NO_RETURN SYSTEM_CALL void  exit(int);

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
    pid_t getpid(void)
    {
        return DO_SYSCALL(getpid);
    }

    SYSTEM_CALL
    pid_t getppid(void)
    {
        return DO_SYSCALL(getppid);
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
    int thr_new(struct thr_param *param, int param_size)
    {
        return DO_SYSCALL(thr_new, param, param_size);
    }

    SYSTEM_CALL
    int setitimer(int which, const struct itimerval *value, struct itimerval *ovalue)
    {
        return DO_SYSCALL(setitimer, which, value, ovalue);
    }

    SYSTEM_CALL
    int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
    {
        return DO_SYSCALL(sigaction, signum, act, oldact);
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

    SYSTEM_CALL
    int kill(pid_t pid, int sig)
    {
        return DO_SYSCALL(kill, pid, sig);
    }

    NO_RETURN SYSTEM_CALL
    int thr_exit(long *state)
    {
        DO_SYSCALL(thr_exit, state);
        __builtin_unreachable();
    }

    NO_RETURN SYSTEM_CALL
    void exit(int status)
    {
        DO_SYSCALL(exit, status);
        __builtin_unreachable();
    }
}

#endif
