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
    void set_current_thread_name(const char *comm)
    {
        Syscall::prctl(PR_SET_NAME,  (unsigned long) comm, 0, 0, 0);
    } 

    METHOD
    Process Process::find_process_by_name(const char *proc_name)
    {
        pid_t result = 0;

        Filesystem::Directory::each("/proc", [proc_name,&result](const char *filename) -> int {
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

            return 0;
        });

        return Process(result);
    }

    METHOD
    Process Process::find_process_by_path(const char *exe_path)
    {
        pid_t result = 0;

        Filesystem::Directory::each("/proc", [exe_path,&result](const char *filename) {
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

            return 0;
        });

        return Process(result);
    }

    NO_RETURN METHOD
    void Process::terminate_thread(int status)
    {
        Syscall::exit_thread(status);
    }

    NO_RETURN METHOD
    void Process::exit(int status)
    {
        Syscall::exit_process(status);
    }
    
    METHOD
    Process Process::create_thread(thread_routine thread_entry, void *arg)
    {
        void *child_stack;
        size_t stack_size = Process::THREAD_STACK_SIZE;
        pid_t tid;

        child_stack = Memory::allocate(stack_size, PROT_READ|PROT_WRITE);

        tid = Syscall::clone(
            CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM,
            (char *) child_stack + stack_size,
            NULL, NULL, NULL
        );

        if ( !tid )
            Process::terminate_thread(thread_entry(arg));
        else
            return Process(tid);
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
    Process Process::spawn(const char *filename, char *const argv[], char *const envp[], Channel<M> channel)
    {
        pid_t pid = Syscall::fork();
        if ( pid == 0 )
        {
            execute(filename, argv, envp, channel);
        }
        else
            return Process(pid);
    }

    METHOD
    sighandler_t Process::set_signal_handler(int signal, sighandler_t handler)
    {
        struct sigaction act, old_act;

        act.sa_handler = handler;
        _memset(&act.sa_mask, 0, sizeof(sigset_t));
        act.sa_flags = SA_RESETHAND;

        Syscall::sigaction(signal, &act, &old_act);

        return (sighandler_t) old_act.sa_restorer;
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
}

#endif
