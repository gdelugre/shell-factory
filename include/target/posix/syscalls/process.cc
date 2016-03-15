#ifndef POSIX_SYSCALL_PROCESS_H_
#define POSIX_SYSCALL_PROCESS_H_

#include <signal.h>
#include <sys/time.h>

namespace Syscall {

    SYSTEM_CALL pid_t           getpid(void);
    SYSTEM_CALL pid_t           getppid(void);
    SYSTEM_CALL pid_t           fork(void);
    NO_RETURN SYSTEM_CALL int   execve(const char *, char *const[], char *const[]);
    #if SYSCALL_EXISTS(fexecve)
    NO_RETURN SYSTEM_CALL int   fexecve(int, char *const[], char *const[]);
    #endif
    #if SYSCALL_EXISTS(setitimer)
    SYSTEM_CALL int             setitimer(int, const struct itimerval *, struct itimerval *);
    #endif
    SYSTEM_CALL unsigned        alarm(unsigned);
    #if SYSCALL_EXISTS(sigaction)
    SYSTEM_CALL int             sigaction(int, const struct sigaction *, struct sigaction *);
    #endif
    SYSTEM_CALL int             sigaltstack(const stack_t *, stack_t *);
    SYSTEM_CALL int             kill(pid_t, int);
    NO_RETURN SYSTEM_CALL void  exit(int);

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

    #if SYSCALL_EXISTS(fexecve)
    NO_RETURN SYSTEM_CALL
    int execve(int fd, char *const argv[], char *const envp[])
    {
        DO_SYSCALL(fexecve, fd, argv, envp);
        __builtin_unreachable();
    }
    #endif

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
        //
        // Some systems deprecated alarm in favor of setitimer.
        // We wrap to setitimer for them here.
        //
        struct itimerval itv = { {0,0}, {0,0} };

        itv.it_value.tv_sec = seconds;
        return Syscall::setitimer(ITIMER_REAL, &itv, &itv);
        #endif
    }

    #if SYSCALL_EXISTS(sigaction)
    SYSTEM_CALL
    int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
    {
        return DO_SYSCALL(sigaction, signum, act, oldact);
    }
    #endif

    SYSTEM_CALL
    int sigaltstack(const stack_t *ss, stack_t *oss)
    {
        return DO_SYSCALL(sigaltstack, ss, oss);
    }

    SYSTEM_CALL
    int kill(pid_t pid, int sig)
    {
        return DO_SYSCALL(kill, pid, sig);
    }

    NO_RETURN SYSTEM_CALL
    void exit(int status)
    {
        DO_SYSCALL(exit, status);
        __builtin_unreachable();
    }
} 

#endif
