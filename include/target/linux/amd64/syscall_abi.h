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

#define SYS_BIND_ARG_REGISTER(n, reg) \
    register long int __arg##n asm (#reg); \
    if ( nr_args >= n ) \
        __arg##n = sys_args[n - 1];

#define VA_FOREACH_ARG0(fn, ...)
#define VA_FOREACH_ARG1(fn, x, ...) fn(1, x)
#define VA_FOREACH_ARG2(fn, x, ...) fn(2, x) VA_FOREACH_ARG1(fn, __VA_ARGS__)
#define VA_FOREACH_ARG3(fn, x, ...) fn(3, x) VA_FOREACH_ARG2(fn, __VA_ARGS__)
#define VA_FOREACH_ARG4(fn, x, ...) fn(4, x) VA_FOREACH_ARG3(fn, __VA_ARGS__)
#define VA_FOREACH_ARG5(fn, x, ...) fn(5, x) VA_FOREACH_ARG4(fn, __VA_ARGS__)
#define VA_FOREACH_ARG6(fn, x, ...) fn(6, x) VA_FOREACH_ARG5(fn, __VA_ARGS__)
#define VA_FOREACH_ARG7(fn, x, ...) fn(7, x) VA_FOREACH_ARG6(fn, __VA_ARGS__)

#define VA_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define VA_FOREACH_ARG(fn, ...) \
    VA_NTH_ARG(DUMMY, ##__VA_ARGS__, \
            VA_FOREACH_ARG7, \
            VA_FOREACH_ARG6, \
            VA_FOREACH_ARG5, \
            VA_FOREACH_ARG4, \
            VA_FOREACH_ARG3, \
            VA_FOREACH_ARG2, \
            VA_FOREACH_ARG1, \
            VA_FOREACH_ARG0)(fn, ##__VA_ARGS__)

#define SET_SYS_ARG(i, value) \
    sys_args[nr_args - i] = (long int)(value); \

/* 
 * Yes, I know the assert is ugly. But it works.
 * Move along, nothing to see here.
 */
#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, N, args...) ({ \
    _Static_assert(sizeof(#args) <= sizeof("6, 5, 4, 3, 2, 1, 0"), "Too many arguments for syscall, maximum of 7 allowed"); \
    N; \
});
#define VA_NARGS(...) VA_NARGS_IMPL(DUMMY, ##__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1, 0)

#define EMIT_SYSCALL(name, ...)                 \
({                                              \
    const unsigned int nr_args = VA_NARGS(__VA_ARGS__); \
    long int sys_args[7];            \
                                                \
    VA_FOREACH_ARG(SET_SYS_ARG, __VA_ARGS__);   \
                                                \
    SYS_BIND_ARG_REGISTER(1, rdi);              \
    SYS_BIND_ARG_REGISTER(2, rsi);              \
    SYS_BIND_ARG_REGISTER(3, rdx);              \
    SYS_BIND_ARG_REGISTER(4, rcx);              \
    SYS_BIND_ARG_REGISTER(5, r8);               \
    SYS_BIND_ARG_REGISTER(6, r9);               \
                                                \
    asm volatile (                              \
        "syscall\n"                             \
            :: "r" (__arg1), "r" (__arg2), "r" (__arg3), "r" (__arg4), "r" (__arg5), "r" (__arg6) : "memory" \
    );                                          \
})

#endif
