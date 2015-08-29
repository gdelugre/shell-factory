#ifndef SYSCALL_ABI_H_
#define SYSCALL_ABI_H_

/*
 * Calling convention for MIPS ABI:
 *
 * syscall number: v0
 * arg1          : a0
 * arg2          : a1
 * arg3          : a2
 * arg4          : a3
 * arg5          : [sp + 16]
 * arg6          : [sp + 20]
 * arg7          : [sp + 24]
 *
 * syscall
 *
 * return value to v0
 */

#define SYSCALL_CLOBBERED_REGISTERS "$1", "$3", \
    "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "hi", "lo"
#define SYSCALL_NUMBER_REGISTER "$2"
#define SYSCALL_RESULT_REGISTER "$2"
#define SYSCALL_INSTRUCTION     "syscall"
#define SYSCALL_PUSH_ARGUMENTS(...) \
({  \
    size_t nr_args = SyscallArgumentsLength(__VA_ARGS__); \
    void **stack_args = (void **) __builtin_alloca(sizeof(long) * 8); \
    SYSCALL_ARG_BIND_REGISTER(0, "$4", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(1, "$5", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(2, "$6", __VA_ARGS__); \
    SYSCALL_ARG_BIND_REGISTER(3, "$7", __VA_ARGS__); \
    if ( nr_args > 4 ) \
        stack_args[4] = const_cast<void *>(reinterpret_cast<const void *>(GetArgumentByIndex<4>(__VA_ARGS__))); \
})

#endif
