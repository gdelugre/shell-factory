#ifndef _SYSCALL_ABI_H
#define _SYSCALL_ABI_H

/*
 * Calling convention for SysV amd64:
 *
 * syscall number: rax
 * arg1          : rdi
 * arg2          : rsi
 * arg3          : rdx
 * arg4          : rcx
 * arg5          : r8
 * arg6          : r9
 *
 * syscall
 *
 */

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

#define SYSCALL_NUMBER_REGISTER rax
#define SYSCALL_INSTRUCTION syscall
#define SYSCALL_PUSH_ARGUMENTS(...) \
    SYSCALL_ARG_BIND_REGISTER(0, rdi, __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(1, rsi, __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(2, rdx, __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(3, rcx, __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(4, r8, __VA_ARGS__);  \
    SYSCALL_ARG_BIND_REGISTER(5, r9, __VA_ARGS__);

static inline
void system_call(unsigned short number)
{
    register unsigned short __sysnum asm ( TO_STRING(SYSCALL_NUMBER_REGISTER) ) = number;

    asm volatile ( TO_STRING(SYSCALL_INSTRUCTION)
            :: "r" (__sysnum) : "memory"
    );
}

#define SYSCALL_ARG_BIND_REGISTER(n, reg, ...)                                  \
    if ( nr_args > n )                                                          \
    {                                                                           \
        register auto __arg##n asm (#reg) = GetArgumentByIndex<n>(__VA_ARGS__); \
        asm ("" :: "r" (__arg##n));                                             \
    }

#define SYSCALL_NAME_TO_NUM(name) __NR_##name

#define EMIT_SYSCALL(name, ...)                             \
({                                                          \
    size_t nr_args = SyscallArgumentsLength(__VA_ARGS__);   \
    int sys_num = SYSCALL_NAME_TO_NUM(name);                \
                                                            \
    SYSCALL_PUSH_ARGUMENTS(__VA_ARGS__);                    \
    system_call(sys_num);                                   \
})

#endif
