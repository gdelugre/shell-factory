#ifndef PICOLIB_PROCESS_H_
#define PICOLIB_PROCESS_H_

namespace Pico {

    class Process {

        constexpr static unsigned THREAD_STACK_SIZE = 0x10000;
        typedef int (* thread_routine)(void *);
        typedef void (* sighandler_t)(int);

        public:
            using process_id = Target::process_id;

            FUNCTION Process        current();
            FUNCTION Process        parent();
            FUNCTION void           set_current_thread_name(const char *comm);
            FUNCTION Process        find_process_by_name(char *proc_name);
            FUNCTION Process        find_process_by_path(char *exe_path);
            FUNCTION Process        create_thread(thread_routine thread_entry, void *arg);
            FUNCTION sighandler_t   set_signal_handler(int signal, sighandler_t handler);

            template <enum channel_mode M>
            NO_RETURN FUNCTION void execute(Channel<M> channel, const char *filename,
                                            char *const argv[] = nullptr, char *const envp[] = nullptr)
            {
                channel.dup_to_stdio();
                execute(filename, argv, envp);
            }

            NO_RETURN FUNCTION void execute(const char *filename,
                                            char *const argv[] = nullptr, char *const envp[] = nullptr);

            template <enum channel_mode M>
            FUNCTION Process        spawn(Channel<M> channel, const char *filename,
                                          char *const argv[] = nullptr, char *const envp[] = nullptr);

            FUNCTION Process        spawn(const char *filename,
                                          char *const argv[] = nullptr, char *const envp[] = nullptr);

            NO_RETURN FUNCTION void terminate_thread(int status);
            NO_RETURN FUNCTION void exit(int status);

            CONSTRUCTOR             Process(process_id pid) : pid(pid) {}
            METHOD process_id       id() const { return pid; };
            METHOD int              send_signal(int signal);
            METHOD int              wait(int *status);
            METHOD int              kill();

        protected:
            process_id pid;
    };
}

#endif
