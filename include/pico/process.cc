#ifndef PICOLIB_PROCESS_H_
#define PICOLIB_PROCESS_H_

#include "channel.cc"
#include "string.cc"
#include "memory.cc"
#include "io.cc"

using namespace Pico;

typedef void (* sighandler_t)(int);
typedef int (* thread_routine)(void *);

#define THREAD_STACK_SIZE 0x10000
#define COMM_MAX 16

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
    Memory::Buffer buffer;
    pid_t pid;
    char comm_path[PATH_MAX]; 
    char comm[COMM_MAX + 1];
    int fd;

    read_directory("/proc", buffer, &dsize);
    struct linux_dirent *dirents = buffer.as<struct linux_dirent *>(), *current;

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
            buffer.free();
            return pid;
        }
    }

    buffer.free();
    return 0;
}

FUNCTION
pid_t find_process_by_path(const char *exe_path)
{
    size_t dsize, off;
    ssize_t n;
    Memory::Buffer buffer;
    pid_t pid;
    char link_path[PATH_MAX]; 
    char exe[PATH_MAX + 1];

    read_directory("/proc", buffer, &dsize);
    struct linux_dirent *dirents = buffer.as<struct linux_dirent *>(), *current;

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
            buffer.free();
            return pid;
        }
    }

    buffer.free();
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

    child_stack = Memory::allocate(stack_size, PROT_READ|PROT_WRITE);

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
