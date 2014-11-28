#ifndef _PROCESS_HELPER_H
#define _PROCESS_HELPER_H

#include <factory.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include "string.h"

typedef void (* sighandler_t)(int);

static inline
pid_t _fork(void)
{
    return INTERNAL_SYSCALL(fork,, 0);
}

static inline
int _execve(const char *filename, char *const argv[], char *const envp[])
{
    return INTERNAL_SYSCALL(execve,, 3, filename, argv, envp);
}

static inline
unsigned int _alarm(unsigned int seconds)
{
    return INTERNAL_SYSCALL(alarm,, 1, seconds);
}

static inline
int _sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
    return INTERNAL_SYSCALL(rt_sigaction,, 4, signum, act, oldact, 8);
}

static inline
int _kill(pid_t pid, int sig)
{
    return INTERNAL_SYSCALL(kill,, 2, pid, sig);
}

static inline __attribute__((noreturn))
void __exit(int status)
{
    INTERNAL_SYSCALL(exit,, 1, status);
    for(;;);
}

static inline __attribute__((noreturn))
void _exit_group(int status)
{
    INTERNAL_SYSCALL(exit_group,, 1, status);
    for (;;);
}

static inline
sighandler_t _signal(int sig, sighandler_t handler)
{
    struct sigaction act, old_act;

    act.sa_handler = handler;
    memset(&act.sa_mask, 0, sizeof(sigset_t));
    act.sa_flags = SA_RESETHAND;

    _sigaction(sig, &act, &old_act);

    return (sighandler_t) old_act.sa_restorer;
}

#endif
