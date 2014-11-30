#ifndef _PROCESS_HELPER_H
#define _PROCESS_HELPER_H

#include <factory.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include "socket.c"
#include "string.c"
#include "io.c"

typedef void (* sighandler_t)(int);

SYSTEM_CALL
pid_t _fork(void)
{
    return INTERNAL_SYSCALL(fork,, 0);
}

SYSTEM_CALL
int _execve(const char *filename, char *const argv[], char *const envp[])
{
    return INTERNAL_SYSCALL(execve,, 3, filename, argv, envp);
}

SYSTEM_CALL
unsigned int _alarm(unsigned int seconds)
{
    return INTERNAL_SYSCALL(alarm,, 1, seconds);
}

SYSTEM_CALL
int _sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
    return INTERNAL_SYSCALL(rt_sigaction,, 4, signum, act, oldact, 8);
}

SYSTEM_CALL
int _kill(pid_t pid, int sig)
{
    return INTERNAL_SYSCALL(kill,, 2, pid, sig);
}

SYSTEM_CALL NO_RETURN
void __exit(int status)
{
    INTERNAL_SYSCALL(exit,, 1, status);
    for(;;);
}

SYSTEM_CALL NO_RETURN
void _exit_group(int status)
{
    INTERNAL_SYSCALL(exit_group,, 1, status);
    for (;;);
}

FUNCTION
sighandler_t _signal(int sig, sighandler_t handler)
{
    struct sigaction act, old_act;

    act.sa_handler = handler;
    _memset(&act.sa_mask, 0, sizeof(sigset_t));
    act.sa_flags = SA_RESETHAND;

    _sigaction(sig, &act, &old_act);

    return (sighandler_t) old_act.sa_restorer;
}

FUNCTION
void execute(const char *filename, char *const argv[], char *const envp[], struct channel chan)
{
#if (CHANNEL != NO_CHANNEL) && (CHANNEL != USE_STDOUT)
    _dup2(chan.rx, stdin)
    _dup2(chan.tx, stdout)
    _dup2(chan.tx, stdout)
#endif

    _execve(filename, argv, envp);
}

#endif
