#ifndef _PROCESS_HELPER_H
#define _PROCESS_HELPER_H

#include <factory.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

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
int _kill(pid_t pid, int sig)
{
    return INTERNAL_SYSCALL(kill,, 2, pid, sig);
}

static inline
void __exit(int status)
{
    INTERNAL_SYSCALL(exit,, 1, status);
}

static inline
void _exit_group(int status)
{
    INTERNAL_SYSCALL(exit_group,, 1, status);
}

#endif
