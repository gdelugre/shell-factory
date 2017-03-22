#ifndef BOOTSTRAP_H_ 
#define BOOTSTRAP_H_

//
// This pattern is used to chain method calls in during initialization.
// Routines declared by the BOOTSTRAP_RUN macro will be run automatically
// before executing the entry point.
//
namespace {
    struct Bootstrap {
        CONSTRUCTOR Bootstrap() = delete;

        template <unsigned N>
        FUNCTION_NOINLINE
        void init() {}

        template <unsigned N>
        FUNCTION_NOINLINE
        void run()
        {
            run<N-1>();
            init<N>();
        }
    };

    template <>
    METHOD
    void Bootstrap::run<__COUNTER__>()
    {
    } 
}

#define BOOTSTRAP_RUN                   \
    template<>                          \
    METHOD                              \
    void Bootstrap::init<__COUNTER__>()

#define BOOTSTRAP_RUN_ALL()             \
    Bootstrap::run<__COUNTER__>()

#endif
