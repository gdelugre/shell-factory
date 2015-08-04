#ifndef PICOLIB_PROCESS_H_
#define PICOLIB_PROCESS_H_

#include "channel.cc"
#include "string.cc"
#include "memory.cc"
#include "fs.cc"

using namespace Pico;
using namespace Pico::Filesystem;

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
    pid_t result = 0;

    Directory::each("/proc", [proc_name,&result](const char *filename) -> int {
        char comm_path[PATH_MAX];
        char comm[COMM_MAX + 1];

        pid_t pid = _atoi(filename);
        if ( pid == 0 )
            return 0;

        _sprintf(comm_path, "/proc/%s/comm", filename);
        int fd = Syscall::open(comm_path, O_RDONLY);
        ssize_t n = Syscall::read(fd, comm, sizeof(comm));
        comm[n - 1] = '\0';

        Syscall::close(fd);

        if ( _strcmp(proc_name, comm) == 0 )
        {
            result = pid;
            return 1;
        }
    });

    return result;
}

FUNCTION
pid_t find_process_by_path(const char *exe_path)
{
    pid_t result = 0;

    Directory::each("/proc", [exe_path,&result](const char *filename) {
        char link_path[PATH_MAX];
        char exe[PATH_MAX + 1];
        pid_t pid = _atoi(filename);

        if ( pid == 0 )
            return 0;

        _sprintf(link_path, "/proc/%s/exe", filename);
        ssize_t n = Syscall::readlink(link_path, exe, sizeof(exe));
        if ( n < 0 )
            return 0;

        exe[n] = '\0';
        if ( _strcmp(exe_path, exe) == 0 )
        {
            result = pid;
            return 1;
        }
    });

    return result;
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
