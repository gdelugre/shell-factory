#ifndef POSIX_PICO_PROCESS_H_
#define POSIX_PICO_PROCESS_H_

namespace Pico {

    METHOD
    Process Process::current()
    {
        return Process( Syscall::getpid() );
    }

    METHOD
    Process Process::parent()
    {
        return Process( Syscall::getppid() );
    }

    NO_RETURN METHOD
    void Process::execute(const char *filename, char *const argv[], char *const envp[])
    {
        Syscall::execve(filename, argv, envp);
    }

    METHOD
    Process Process::spawn(const char *filename, char *const argv[], char *const envp[])
    {
        pid_t pid = Syscall::fork();
        if ( pid == 0 )
            execute(filename, argv, envp);
        else
            return Process(pid);
    }

    template <typename T>
    METHOD
    Process Process::spawn(Stream<T>& stm, const char *filename, char *const argv[], char *const envp[])
    {
        pid_t pid = Syscall::fork();
        if ( pid == 0 )
        {
            execute(stm, filename, argv, envp);
        }
        else
            return Process(pid);
    }

    template <typename... T>
    METHOD
    Process Process::spawn(const char *filename, T... args)
    {
        pid_t pid = Syscall::fork();
        if ( pid == 0 )
        {
            execute(filename, args...);
        }
        else
            return Process(pid);
    }

    METHOD
    Process::signal_handler Process::set_signal_handler(int signal, Process::signal_handler handler)
    {
        struct sigaction act, old_act;

        act.sa_handler = handler;
        Memory::zero(&act.sa_mask, sizeof(sigset_t));
        act.sa_flags = SA_RESETHAND;

        Syscall::sigaction(signal, &act, &old_act);

        return old_act.sa_handler;
    }

    METHOD
    int Process::wait(int *status)
    {
        return Syscall::wait4(pid, status, 0, nullptr);
    }

    METHOD
    int Process::signal(int signal)
    {
        return Syscall::kill(pid, signal);
    }

    METHOD
    int Process::kill()
    {
        return signal(SIGKILL);
    }
}

#endif
