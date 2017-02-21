#ifndef PICOLIB_PROCESS_IMPL_H_
#define PICOLIB_PROCESS_IMPL_H_

#include <sys/user.h>

namespace Pico {

    constexpr int COMM_MAX = MAXCOMLEN;

    // METHOD
    // Thread Thread::current()
    // {
    // }

    // TODO
    // METHOD
    // void Thread::set_name(const char *comm)
    // {
    // }


    // TODO
    // template <typename Func>
    // METHOD 
    // int Process::each(Func cb)
    // {
    // }

    // TODO
    // METHOD
    // Process Process::find_by_name(char *proc_name)
    // {
    // }

    // TODO
    // METHOD
    // Process Process::find_by_path(char *exe_path)
    // {
    // }

    // TODO
    // NO_RETURN METHOD
    // void Thread::exit(int UNUSED status)
    // {
    //     long state;
    //     Syscall::thr_exit(&state);
    // }

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

    // No implementation defined.
    // METHOD
    // int Thread::wait(int *status)
    // {
    // }

    // METHOD
    // int Thread::signal(int signal)
    // {
    // }

    METHOD
    int Thread::kill()
    {
        return signal(SIGKILL);
    }
}

#endif
