#ifndef PICOLIB_PROCESS_IMPL_H_
#define PICOLIB_PROCESS_IMPL_H_

namespace Pico {

    constexpr int COMM_MAX = 16;

    METHOD
    Thread Thread::current()
    {
        return Thread( Syscall::gettid() );
    }

    METHOD
    void Thread::set_name(const char *comm)
    {
        Syscall::prctl(PR_SET_NAME,  (unsigned long) comm, 0, 0, 0);
    }

    template <typename Func>
    METHOD
    int Process::each(Func cb)
    {
        return Filesystem::Directory::each("/proc", [cb](const char *filename) -> int {
            pid_t pid = atoi(filename);
            if ( pid == 0 )
                return 0;

            Process proc(pid);
            return cb(proc);
        });
    }

    METHOD
    Process Process::find_by_name(char *proc_name)
    {
        pid_t result = -1;

        Process::each([proc_name, &result](Process proc) -> int {
            char comm_path[PATH_MAX];
            char comm[COMM_MAX + 1];

            sprintf(comm_path, "/proc/%d/comm", proc.id());

            Filesystem::File comm_file(comm_path, Filesystem::File::READ);
            ssize_t n = comm_file.read(comm, sizeof(comm));
            comm[n - 1] = '\0';
            comm_file.close();

            if ( String::equals(proc_name, comm) )
            {
                result = proc.id();
                return 1;
            }

            return 0;
        });

        return Process(result);
    }

    METHOD
    Process Process::find_by_path(char *exe_path)
    {
        pid_t result = -1;

        Process::each([exe_path, &result](Process proc) -> int {
            char link_path[PATH_MAX];
            char exe[PATH_MAX + 1];

            sprintf(link_path, "/proc/%d/exe", proc.id());
            ssize_t n = Syscall::readlink(link_path, exe, sizeof(exe));
            if ( n < 0 )
                return 0;

            exe[n] = '\0';
            if ( String::equals(exe_path, exe) )
            {
                result = proc.id();
                return 1;
            }

            return 0;
        });

        return Process(result);
    }

    NO_RETURN METHOD
    void Thread::exit(int status)
    {
        Syscall::exit(status);
    }

    NO_RETURN METHOD
    void Process::exit(int status)
    {
        Syscall::exit_group(status);
    }

    METHOD
    Thread Thread::create(thread_routine thread_entry, void *arg)
    {
        void *child_stack;
        size_t stack_size = STACK_SIZE;
        pid_t tid;

        child_stack = Memory::allocate(stack_size, Memory::READ | Memory::WRITE | Memory::STACK);

        tid = Syscall::clone(
            CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM,
            static_cast<char *>(child_stack) + stack_size,
            NULL, NULL, NULL
        );

        if ( !tid )
            Thread::exit(thread_entry(arg));
        else
            return Thread(tid);
    }

    //
    // XXX: Doesn't work.
    //
    METHOD
    int Thread::wait(int *status)
    {
        return Syscall::wait4(tid, status, __WCLONE, nullptr);
    }

    METHOD
    int Thread::signal(int signal)
    {
        return Syscall::tkill(tid, signal);
    }

    METHOD
    int Thread::kill()
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
