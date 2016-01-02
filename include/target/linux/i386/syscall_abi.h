#ifndef SYSCALL_ABI_H_
#define SYSCALL_ABI_H_

/*
 * Calling convention for SysV x86:
 *
 * syscall number: eax
 * arg1          : ebx
 * arg2          : ecx
 * arg3          : edx
 * arg4          : esi
 * arg5          : edi
 * arg6          : ebp
 *
 * syscall
 *
 * return value to eax
 */

#define SYSCALL_CLOBBERED_REGISTERS "ecx", "edx" 
#define SYSCALL_NUMBER_REGISTER "eax"
#define SYSCALL_RESULT_REGISTER "eax"
#define SYSCALL_INSTRUCTION     "int $0x80"

#define SYSCALL_SET_ARG_1(value) SYSCALL_ARG_BIND_REGISTER(1, "ebx", value)
#define SYSCALL_SET_ARG_2(value) SYSCALL_ARG_BIND_REGISTER(2, "ecx", value)
#define SYSCALL_SET_ARG_3(value) SYSCALL_ARG_BIND_REGISTER(3, "edx", value)
#define SYSCALL_SET_ARG_4(value) SYSCALL_ARG_BIND_REGISTER(4, "esi", value)
#define SYSCALL_SET_ARG_5(value) SYSCALL_ARG_BIND_REGISTER(5, "edi", value)
#define SYSCALL_SET_ARG_6(value) SYSCALL_ARG_BIND_REGISTER(6, "ebp", value)
#define SYSCALL_SET_ARG_7(value) static_assert(false, "Too many arguments for this architecture.")

#endif
