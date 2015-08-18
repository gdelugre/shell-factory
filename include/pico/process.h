#ifndef PICOLIB_PROCESS_H_
#define PICOLIB_PROCESS_H_

namespace Pico {

    class Process {
        constexpr static unsigned THREAD_STACK_SIZE = 0x10000;
        typedef int (* thread_routine)(void *);
        typedef void (* sighandler_t)(int);

        public:
            FUNCTION Process        current();
            FUNCTION void           set_current_thread_name(const char *comm);
            FUNCTION Process        find_process_by_name(const char *proc_name);
            FUNCTION Process        find_process_by_path(const char *exe_path);
            FUNCTION Process        create_thread(thread_routine thread_entry, void *arg);
            FUNCTION sighandler_t   set_signal_handler(int signal, sighandler_t handler);

            template <enum channel_mode M>
            NO_RETURN FUNCTION void execute(const char *filename, char *const argv[], char *const envp[], Channel<M> channel) {
                channel.dup_to_stdio();
                execute(filename, argv, envp);
            }

            template <enum channel_mode M>
            NO_RETURN FUNCTION void execute(const char *filename, char *const argv[], Channel<M> channel) {
                execute(filename, argv, nullptr, channel);
            }

            template <enum channel_mode M>
            NO_RETURN FUNCTION void execute(const char *filename, Channel<M> channel) {
                execute(filename, nullptr, channel);
            }

            NO_RETURN FUNCTION void execute(const char *filename, char *const argv[], char *const envp[]);
            NO_RETURN FUNCTION void execute(const char *filename, char *const argv[]) {
                execute(filename, argv, nullptr);
            }

            NO_RETURN FUNCTION void execute(const char *filename) {
                execute(filename, nullptr);
            }

            template <enum channel_mode M>
            FUNCTION Process        spawn(const char *filename, char *const argv[], char *const envp[], Channel<M> channel);

            template <enum channel_mode M>
            FUNCTION Process        spawn(const char *filename, char *const argv[], Channel<M> channel) {
                return spawn(filename, argv, nullptr, channel);
            }

            template <enum channel_mode M>
            FUNCTION Process        spawn(const char *filename, Channel<M> channel) {
                return spawn(filename, nullptr, channel);
            }

            FUNCTION Process        spawn(const char *filename, char *const argv[], char *const envp[]);
            FUNCTION Process        spawn(const char *filename, char *const argv[]) {
                return spawn(filename, argv, nullptr);
            }

            FUNCTION Process        spawn(const char *filename) {
                return spawn(filename, nullptr);
            }

            NO_RETURN FUNCTION void terminate_thread(int status);
            NO_RETURN FUNCTION void exit(int status);

            CONSTRUCTOR             Process(pid_t pid) : pid(pid) {}
            METHOD pid_t            process_id() const { return pid; };
            METHOD int              send_signal(int signal);
            METHOD int              wait(int *status);
            METHOD int              kill();

        private:
            pid_t pid;
    };
}

#endif
