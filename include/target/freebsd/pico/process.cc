#ifndef PICOLIB_PROCESS_IMPL_H_
#define PICOLIB_PROCESS_IMPL_H_

namespace Pico {

    constexpr int COMM_MAX = MAXCOMLEN;

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

    METHOD
    void Thread::set_name(const char *comm)
    {
        Syscall::thr_set_name(Thread::current().id(), comm);
    }

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

    METHOD
    Thread Thread::create(thread_routine thread_entry, void *arg)
    {
        void *child_stack;
        size_t stack_size = STACK_SIZE;
        struct thr_param param;
        long tid;

        child_stack = Memory::allocate(stack_size, Memory::READ | Memory::WRITE | Memory::STACK);
        param.start_func = thread_entry;
        param.arg = arg;
        param.stack_base = static_cast<char *>(child_stack);
        param.stack_size = stack_size;
        param.tls_base = nullptr;
        param.tls_size = 0;
        param.child_tid = nullptr;
        param.parent_tid = &tid;
        param.flags = 0;
        param.rtp = nullptr;

        Syscall::thr_new(&param, sizeof(param));
        return Thread(static_cast<lwpid_t>(tid));
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

    // No implementation defined.
    // METHOD
    // int Thread::wait(int *status)
    // {
    // }

    METHOD
    int Process::wait(int *status)
    {
        return Syscall::wait4(pid, status, 0, nullptr);
    }

    METHOD
    int Thread::signal(int signal)
    {
        return Syscall::thr_kill(tid, signal);
    }

    METHOD
    int Process::signal(int signal)
    {
        return Syscall::kill(pid, signal);
    }

    METHOD
    int Thread::kill()
    {
        return signal(SIGKILL);
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
