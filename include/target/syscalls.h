#ifndef GENERIC_SYSCALL_ABI_H_
#define GENERIC_SYSCALL_ABI_H_

#define SYSCALL_ARG_BIND_REGISTER(i, reg, value)                        \
    register auto __arg##i asm (reg) = value;                           \

#define EMIT_SYSCALL0(name, ...)                                        \
({                                                                      \
    int sys_num = SYSCALL_NAME_TO_NUM(name);                            \
                                                                        \
    register int __sys_num asm ( SYSCALL_NUMBER_REGISTER ) = sys_num;   \
    register long __sys_result asm ( SYSCALL_RESULT_REGISTER );         \
                                                                        \
    asm volatile (                                                      \
        SYSCALL_INSTRUCTION "\n"                                        \
        : "=r" (__sys_result)                                           \
        : [nr_args] "i"(4), "r" (__sys_num), ##__VA_ARGS__              \
        : "memory", "cc"                                                \
    );                                                                  \
    asm volatile ("" ::: SYSCALL_CLOBBERED_REGISTERS );                 \
                                                                        \
    __sys_result;                                                       \
})                                                                      \

#define EMIT_SYSCALL1(name, arg1, ...)                                  \
({                                                                      \
    SYSCALL_SET_ARG_1(arg1);                                            \
    EMIT_SYSCALL0(name,                                                 \
                  "X"(__arg1), ##__VA_ARGS__);                          \
})                                                                      \

#define EMIT_SYSCALL2(name, arg1, arg2, ...)                            \
({                                                                      \
    SYSCALL_SET_ARG_2(arg2);                                            \
    EMIT_SYSCALL1(name, arg1,                                           \
                  "X"(__arg2), ##__VA_ARGS__);                          \
})                                                                      \

#define EMIT_SYSCALL3(name, arg1, arg2, arg3, ...)                      \
({                                                                      \
    SYSCALL_SET_ARG_3(arg3);                                            \
    EMIT_SYSCALL2(name, arg1, arg2,                                     \
                  "X"(__arg3), ##__VA_ARGS__);                          \
})                                                                      \

#define EMIT_SYSCALL4(name, arg1, arg2, arg3, arg4, ...)                \
({                                                                      \
    SYSCALL_SET_ARG_4(arg4);                                            \
    EMIT_SYSCALL3(name, arg1, arg2, arg3,                               \
                  "X"(__arg4), ##__VA_ARGS__);                          \
})                                                                      \

#define EMIT_SYSCALL5(name, arg1, arg2, arg3, arg4, arg5, ...)          \
({                                                                      \
    SYSCALL_SET_ARG_5(arg5);                                            \
    EMIT_SYSCALL4(name, arg1, arg2, arg3, arg4,                         \
                  "X"(__arg5), ##__VA_ARGS__);                          \
})                                                                      \

#define EMIT_SYSCALL6(name, arg1, arg2, arg3, arg4, arg5, arg6, ...)    \
({                                                                      \
    SYSCALL_SET_ARG_6(arg6);                                            \
    EMIT_SYSCALL5(name, arg1, arg2, arg3, arg4, arg5,                   \
                  "X"(__arg6), ##__VA_ARGS__);                          \
})                                                                      \

#define EMIT_SYSCALL7(name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, ...) \
({                                                                      \
    SYSCALL_SET_ARG_7(arg7);                                            \
    EMIT_SYSCALL5(name, arg1, arg2, arg3, arg4, arg5, arg6,             \
                  "X"(__arg7), ##__VA_ARGS__);                          \
})                                                                      \

#define EMIT_SYSCALL_SWITCH(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N
#define EMIT_SYSCALL(name, args...)                                     \
        EMIT_SYSCALL_SWITCH(0, ##args,                                  \
                EMIT_SYSCALL7,                                          \
                EMIT_SYSCALL6,                                          \
                EMIT_SYSCALL5,                                          \
                EMIT_SYSCALL4,                                          \
                EMIT_SYSCALL3,                                          \
                EMIT_SYSCALL2,                                          \
                EMIT_SYSCALL1,                                          \
                EMIT_SYSCALL0)(name, ##args)                            \

#define DO_SYSCALL(name, args...) ({ \
  int err_val; \
  (void) err_val; \
  EMIT_SYSCALL(name, ##args); \
  });


#if defined(__linux__)
#include <target/linux/syscalls.h>
#else
#pragma message "No operating system detected, compiling to bare-metal."
#endif

#endif
