#ifndef GENERIC_SYSCALL_ABI_H_
#define GENERIC_SYSCALL_ABI_H_

#ifndef SYSCALL_CLOBBERED_REGISTERS
#define SYSCALL_CLOBBERED_REGISTERS "memory"
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
#pragma GCC diagnostic ignored "-Wunused-parameter"
template <typename ...T>
size_t SyscallArgumentsLength(T ...args)
{
    static_assert(sizeof...(args) < 8, "Too many arguments for syscall");

    return sizeof...(args);
}

template <int N, typename ...Args> struct GetArgumentByIndexHelper;

template <typename ArgType, typename ...Rest>
struct GetArgumentByIndexHelper<0, ArgType, Rest...>
{
    static auto value(ArgType arg, Rest ...args)
    {
        return arg;
    }
};

template <int N, typename ArgType, typename ...Rest>
struct GetArgumentByIndexHelper<N, ArgType, Rest...>
{
    static auto value(ArgType arg, Rest ...args)
    {
        (void) arg;
        return GetArgumentByIndexHelper<N-1, Rest...>::value(args...);
    }
};

template <int N>
struct GetArgumentByIndexHelper<N>
{
    static auto value(void)
    {
        return -1;
    }
};

template <int N, typename ...T>
auto GetArgumentByIndex(T ...args)
{
    return GetArgumentByIndexHelper<N, T...>::value(args...);
}
#pragma GCC diagnostic pop

#define SYSCALL_ARG_BIND_REGISTER(n, reg, ...)                                  \
    if ( nr_args > n )                                                          \
    {                                                                           \
        register auto __arg##n asm (reg) = GetArgumentByIndex<n>(__VA_ARGS__);  \
        asm ("" :: "r" (__arg##n));                                             \
    }

#define EMIT_SYSCALL(name, ...)                                         \
({                                                                      \
    size_t nr_args = SyscallArgumentsLength(__VA_ARGS__);               \
    short sys_num = SYSCALL_NAME_TO_NUM(name);                          \
                                                                        \
    SYSCALL_PUSH_ARGUMENTS(__VA_ARGS__);                                \
                                                                        \
    register short __sys_num asm ( SYSCALL_NUMBER_REGISTER ) = sys_num; \
    register long __sys_result asm ( SYSCALL_RESULT_REGISTER );         \
                                                                        \
    asm volatile (                                                      \
        SYSCALL_INSTRUCTION "\n"                                        \
            : "=r" (__sys_result)                                       \
            : "r" (__sys_num)                                           \
            : "memory", "cc", SYSCALL_CLOBBERED_REGISTERS               \
    );                                                                  \
                                                                        \
    __sys_result;                                                       \
})

#define DO_SYSCALL(name, ...) ({ \
  int err_val; \
  (void) err_val; \
  EMIT_SYSCALL(name, __VA_ARGS__); \
  });


#if defined(__linux__)
#include <target/linux/syscalls.h>
#else
#error "No syscall ABI defined for this operating system."
#endif

#endif
