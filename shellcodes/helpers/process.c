#ifndef _PROCESS_HELPER_H
#define _PROCESS_HELPER_H

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

#include "socket.c"
#include "string.c"
#include "memory.c"
#include "io.c"

typedef void (* sighandler_t)(int);
typedef int (* thread_routine)(void *);

#define THREAD_STACK_SIZE 0x10000
#define COMM_MAX 16

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
int _prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5)
{
    return INTERNAL_SYSCALL(prctl,, 5, option, arg2, arg3, arg4, arg5);
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
    while ( true );
}

SYSTEM_CALL NO_RETURN
void _exit_process(int status)
{
    INTERNAL_SYSCALL(exit_group,, 1, status);
    while ( true );
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
void set_current_thread_name(const char *comm)
{
    _prctl(PR_SET_NAME, (unsigned long) comm, 0, 0, 0);
}

FUNCTION
pid_t find_process_by_name(const char *proc_name)
{
    size_t dsize, off, n;
    struct linux_dirent *dirents, *current;
    pid_t pid;
    char comm_path[PATH_MAX]; 
    char comm[COMM_MAX + 1];
    int fd;

    read_directory("/proc", &dirents, &dsize);

    foreach_dirent(dirents, current, off, dsize)
    {
        pid = _atoi(dirent_name(current));
        if ( pid == 0 )
            continue;

        _sprintf(comm_path, "/proc/%s/comm", dirent_name(current));
        fd = _open(comm_path, O_RDONLY); 
        n = _read(fd, comm, sizeof(comm));
        comm[n - 1] = '\0';
        _close(fd);

        if ( _strcmp(proc_name, comm) == 0 )
        {
            _free(dirents);
            return pid;
        }
    }

    _free(dirents);
    return 0;
}

FUNCTION
pid_t find_process_by_path(const char *exe_path)
{
    size_t dsize, off;
    ssize_t n;
    struct linux_dirent *dirents, *current;
    pid_t pid;
    char link_path[PATH_MAX]; 
    char exe[PATH_MAX + 1];

    read_directory("/proc", &dirents, &dsize);

    foreach_dirent(dirents, current, off, dsize)
    {
        pid = _atoi(dirent_name(current));
        if ( pid == 0 )
            continue;

        _sprintf(link_path, "/proc/%s/exe", dirent_name(current));
        n = _readlink(link_path, exe, sizeof(exe));
        if ( n < 0 )
            continue;

        exe[n] = '\0';
        if ( _strcmp(exe_path, exe) == 0 )
        {
            _free(dirents);
            return pid;
        }
    }

    _free(dirents);
    return 0;
}

FUNCTION
void execute(const char *filename, char *const argv[], char *const envp[], struct channel chan)
{
    _dup2(chan.rx, stdin);
    _dup2(chan.tx, stdout);
    _dup2(chan.tx, stdout);

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
