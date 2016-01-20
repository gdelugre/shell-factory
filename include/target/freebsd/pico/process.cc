#ifndef PICOLIB_PROCESS_IMPL_H_
#define PICOLIB_PROCESS_IMPL_H_

namespace Pico {

    constexpr int COMM_MAX = 16;

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
    // void set_current_thread_name(const char *comm)
    // {
    // } 

    // TODO
    //
    // METHOD
    // Process Process::find_process_by_name(char *proc_name)
    // {
    // }

    // TODO
    // METHOD
    // Process Process::find_process_by_path(char *exe_path)
    // {
    // }

    // TODO
    // NO_RETURN METHOD
    // void Process::terminate_thread(int status)
    // {
    //    Syscall::exit_thread(status);
    // }

    NO_RETURN METHOD
    void Process::exit(int status)
    {
        Syscall::exit_process(status);
    }
    
    // TODO
    // METHOD
    // Process Process::create_thread(thread_routine thread_entry, void *arg)
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
    int Process::send_signal(int signal)
    {
        return Syscall::kill(pid, signal);
    }
    
    METHOD
    int Process::kill()
    {
        return send_signal(SIGKILL);
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
