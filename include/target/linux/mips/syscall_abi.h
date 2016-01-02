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

#error "This architecture is not supported."

#define SYSCALL_CLOBBERED_REGISTERS "$1", "$3", \
    "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "hi", "lo"
#define SYSCALL_NUMBER_REGISTER "$2"
#define SYSCALL_RESULT_REGISTER "$2"
#define SYSCALL_INSTRUCTION     "syscall"

#endif
