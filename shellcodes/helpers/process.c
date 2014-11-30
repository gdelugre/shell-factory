#ifndef _PROCESS_HELPER_H
#define _PROCESS_HELPER_H

#include <factory.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <linux/sched.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>

#include "socket.c"
#include "string.c"
#include "memory.c"
#include "io.c"

typedef void (* sighandler_t)(int);
typedef int (* thread_routine)(void *);

#define THREAD_STACK_SIZE 0x10000

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
long _clone(unsigned long flags, void *child_stack, void *ptid, void *tls, void *ctid)
{
    return INTERNAL_SYSCALL(clone,, 5, flags, child_stack, ptid, tls, ctid);
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
void _exit_thread(int status)
{
    INTERNAL_SYSCALL(exit,, 1, status);
    for(;;);
}

SYSTEM_CALL NO_RETURN
void _exit_process(int status)
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

FUNCTION
pid_t create_thread(thread_routine thread_entry, void *arg)
{
    void *child_stack;
    size_t stack_size = THREAD_STACK_SIZE;
    pid_t tid;

    child_stack = _mmap(NULL, stack_size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, 0, 0);

    tid = _clone(
        CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM,
        child_stack + stack_size,
        NULL, NULL, NULL
    );

    if ( !tid )
        _exit_thread(thread_entry(arg));
    else
        return tid;
}

#endif
