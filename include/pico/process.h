#ifndef PICOLIB_PROCESS_H_
#define PICOLIB_PROCESS_H_

namespace Pico {

    class Thread {
        constexpr static unsigned STACK_SIZE = 1_MB;

        public:
            using thread_routine = Target::Process::thread_routine;
            using thread_id = Target::Process::thread_id;

            FUNCTION Thread         current();
            FUNCTION void           set_name(const char *comm);
            FUNCTION Thread         create(thread_routine thread_entry, void *arg);
            NO_RETURN FUNCTION void exit(int status);

            CONSTRUCTOR             Thread(thread_id tid) : tid(tid) {}
            METHOD thread_id        id() const { return tid; }
            METHOD int              signal(int signal);
            METHOD int              wait(int *status);
            METHOD int              kill();

        protected:
            thread_id tid;
    };

    class Process {

        public:
            using process_id = Target::Process::process_id;
            using signal_handler = Target::Process::signal_handler;

            FUNCTION Process        current();
            FUNCTION Process        parent();
            FUNCTION Process        find_by_name(char *proc_name);
            FUNCTION Process        find_by_path(char *exe_path);
            FUNCTION signal_handler set_signal_handler(int signal, signal_handler handler);

            template <typename Func>
            FUNCTION int each(Func cb);

            template <typename T>
            NO_RETURN FUNCTION void execute(BiStream<T>& stm, const char *filename,
                                            char *const argv[] = nullptr, char *const envp[] = nullptr)
            {
                BasicStream std_in = Stdio::input();
                BasicStream std_out = Stdio::output();
                BasicStream std_err = Stdio::error();

                auto rstream = stm.read_stream();
                auto wstream = stm.write_stream();

                // Avoid remapping standard IO onto itself.
                if (rstream != std_in)
                    rstream.duplicate(std_in);

                if (wstream != std_out && wstream != std_err) {
                    wstream.duplicate(std_out);
                    wstream.duplicate(std_err);
                }

                execute(filename, argv, envp);
            }

            template <typename T>
            NO_RETURN FUNCTION void execute(Stream<T>& stm, const char *filename,
                                            char *const argv[] = nullptr, char *const envp[] = nullptr)
            {
                BasicStream std_in = Stdio::input();
                BasicStream std_out = Stdio::output();
                BasicStream std_err = Stdio::error();

                stm.duplicate(std_in);
                stm.duplicate(std_out);
                stm.duplicate(std_err);

                execute(filename, argv, envp);
            }

            NO_RETURN FUNCTION void execute(const char *filename,
                                            char *const argv[] = nullptr, char *const envp[] = nullptr);

            template <typename T>
            FUNCTION Process        spawn(Stream<T>& stm, const char *filename,
                                          char *const argv[] = nullptr, char *const envp[] = nullptr);

            FUNCTION Process        spawn(const char *filename,
                                          char *const argv[] = nullptr, char *const envp[] = nullptr);

            NO_RETURN FUNCTION void exit(int status);

            CONSTRUCTOR             Process(process_id pid) : pid(pid) {}
            METHOD process_id       id() const { return pid; };
            METHOD int              signal(int signal);
            METHOD int              wait(int *status);
            METHOD int              kill();

        protected:
            process_id pid;
    };
}

#endif
