#ifndef PICOLIB_PROCESS_IMPL_H_
#define PICOLIB_PROCESS_IMPL_H_

namespace Pico {

    constexpr int COMM_MAX = 16;

    METHOD
    Thread Thread::current()
    {
        lwpid_t tid;

        Syscall::thr_self(&tid);
        return Thread(tid);
    }

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

    // XXX: Probably not possible.
    // Could be implemented for setting the process name though, as in setproctitle.
    //
    // METHOD
    // void Thread::set_name(const char *comm)
    // {
    // } 

    // TODO
    //
    // METHOD
    // Process Process::find_by_name(char *proc_name)
    // {
    // }

    // TODO
    // METHOD
    // Process Process::find_by_path(char *exe_path)
    // {
    // }

    NO_RETURN METHOD
    void Thread::exit(int UNUSED status)
    {
        long state;
        Syscall::thr_exit(&state);
    }

    NO_RETURN METHOD
    void Process::exit(int status)
    {
        Syscall::exit(status);
    }
    
    // TODO
    // METHOD
    // Thread Thread::create(thread_routine thread_entry, void *arg)
    // {
    // }

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

    template <enum channel_mode M>
    METHOD
    Process Process::spawn(Channel<M> channel, const char *filename, char *const argv[], char *const envp[])
    {
        pid_t pid = Syscall::fork();
        if ( pid == 0 )
        {
            execute(channel, filename, argv, envp);
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

    namespace Filesystem {

        #if SYSCALL_EXISTS(execveat)
        NO_RETURN METHOD
        void File::execute(char *const argv[], char *const envp[])
        {
            Syscall::execveat(this->file_desc(), "", argv, envp, AT_EMPTY_PATH);
        }
        #endif
    }
}

#endif
