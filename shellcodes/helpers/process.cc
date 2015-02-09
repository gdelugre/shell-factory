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
#include <sys/time.h>

typedef void (* sighandler_t)(int);
typedef int (* thread_routine)(void *);

#define THREAD_STACK_SIZE 0x10000
#define COMM_MAX 16

/* 
 * System calls defined in this file. 
 */
namespace Syscall {

    SYSTEM_CALL pid_t           fork(void);
    SYSTEM_CALL int             execve(const char *, char *const[], char *const[]);
    SYSTEM_CALL long            clone(unsigned long, void *, void *, void *, void *);
    SYSTEM_CALL int             prctl(int, unsigned long, unsigned long, unsigned long, unsigned long);
    #if SYSCALL_EXISTS(setitimer)
    SYSTEM_CALL int             setitimer(int, const struct itimerval *, struct itimerval *);
    #endif
    SYSTEM_CALL int             kill(pid_t, int);
    NO_RETURN SYSTEM_CALL void  exit_thread(int);
    NO_RETURN SYSTEM_CALL void  exit_process(int);

    SYSTEM_CALL
    pid_t fork(void)
    {
        return DO_SYSCALL(fork, 0);
    }

    SYSTEM_CALL
    int execve(const char *filename, char *const argv[], char *const envp[])
    {
        return DO_SYSCALL(execve, 3, filename, argv, envp);
    }

    SYSTEM_CALL
    long clone(unsigned long flags, void *child_stack, void *ptid, void *tls, void *ctid)
    {
        return DO_SYSCALL(clone, 5, flags, child_stack, ptid, tls, ctid);
    }

    SYSTEM_CALL
    int prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5)
    {
        return DO_SYSCALL(prctl, 5, option, arg2, arg3, arg4, arg5);
    }

    #if SYSCALL_EXISTS(setitimer)
    SYSTEM_CALL
    int setitimer(int which, const struct itimerval *value, struct itimerval *ovalue)
    {
        return DO_SYSCALL(setitimer, 3, which, value, ovalue);
    }
    #endif

    SYSTEM_CALL
    unsigned int alarm(unsigned int seconds)
    {
        #if SYSCALL_EXISTS(alarm)
        return DO_SYSCALL(alarm, 1, seconds);
        #else
        struct itimerval itv = { {0,0}, {0,0} };

        itv.it_value.tv_sec = seconds;
        return Syscall::setitimer(ITIMER_REAL, &itv, &itv);
        #endif
    }

    SYSTEM_CALL
    int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
    {
        return DO_SYSCALL(rt_sigaction, 4, signum, act, oldact, 8);
    }

    SYSTEM_CALL
    int kill(pid_t pid, int sig)
    {
        return DO_SYSCALL(kill, 2, pid, sig);
    }

    NO_RETURN SYSTEM_CALL
    void exit_thread(int status)
    {
        DO_SYSCALL(exit, 1, status);
        while ( true );
    }

    NO_RETURN SYSTEM_CALL
    void exit_process(int status)
    {
        DO_SYSCALL(exit_group, 1, status);
        while ( true );
    }
}

#include "channel.cc"
#include "string.cc"
#include "memory.cc"
#include "io.cc"

FUNCTION
sighandler_t _signal(int sig, sighandler_t handler)
{
    struct sigaction act, old_act;

    act.sa_handler = handler;
    _memset(&act.sa_mask, 0, sizeof(sigset_t));
    act.sa_flags = SA_RESETHAND;

    Syscall::sigaction(sig, &act, &old_act);

    return (sighandler_t) old_act.sa_restorer;
}

FUNCTION
void set_current_thread_name(const char *comm)
{
    Syscall::prctl(PR_SET_NAME, (unsigned long) comm, 0, 0, 0);
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
        fd = Syscall::open(comm_path, O_RDONLY); 
        n = Syscall::read(fd, comm, sizeof(comm));
        comm[n - 1] = '\0';

        Syscall::close(fd);

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
        n = Syscall::readlink(link_path, exe, sizeof(exe));
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
void execute(const char *filename, char *const argv[], char *const envp[], Channel channel)
{
    if ( channel.dupable_to_stdout )
    {
        Syscall::dup2(channel.rx, stdin);
        Syscall::dup2(channel.tx, stdout);
        Syscall::dup2(channel.tx, stdout);
    }

    Syscall::execve(filename, argv, envp);
}

FUNCTION
pid_t create_thread(thread_routine thread_entry, void *arg)
{
    void *child_stack;
    size_t stack_size = THREAD_STACK_SIZE;
    pid_t tid;

    child_stack = allocate_memory(stack_size, PROT_READ|PROT_WRITE);

    tid = Syscall::clone(
        CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM,
        (char *) child_stack + stack_size,
        NULL, NULL, NULL
    );

    if ( !tid )
        Syscall::exit_thread(thread_entry(arg));
    else
        return tid;
}

#endif
