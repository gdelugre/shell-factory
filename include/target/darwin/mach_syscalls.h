#ifndef DARWIN_MACH_SYSCALLS_H_
#define DARWIN_MACH_SYSCALLS_H_

#include <mach/error.h>

#define DO_MACH_SYSCALL(name, args...) DO_SYSCALL_WITH_CLASS(MACH, name, ##args)

namespace Target {

    FUNCTION
    constexpr bool is_mach_error(kern_return_t err)
    {
        return Options::disable_error_checks ? false :
            err != ERR_SUCCESS;
    }
}

//
// Definitions taken from osfmk/kern/syscall_sw.c in XNU kernel.
//
#define SYS__kernelrpc_mach_vm_allocate_trap            10
#define SYS__kernelrpc_mach_vm_deallocate_trap          11
#define SYS__kernelrpc_mach_vm_protect_trap             14
#define SYS__kernelrpc_mach_vm_map_trap                 15
#define SYS__kernelrpc_mach_port_allocate_trap          16
#define SYS__kernelrpc_mach_port_destroy_trap           17
#define SYS__kernelrpc_mach_port_deallocate_trap        18
#define SYS__kernelrpc_mach_port_mod_refs_trap          19
#define SYS__kernelrpc_mach_port_move_member_trap       20
#define SYS__kernelrpc_mach_port_insert_right_trap      21
#define SYS__kernelrpc_mach_port_insert_member_trap     22
#define SYS__kernelrpc_mach_port_extract_member_trap    23
#define SYS__kernelrpc_mach_port_construct_trap         24
#define SYS__kernelrpc_mach_port_destruct_trap          25
#define SYS_mach_reply_port                             26
#define SYS_thread_self_trap                            27
#define SYS_task_self_trap                              28
#define SYS_host_self_trap                              29
#define SYS_mach_msg_trap                               31
#define SYS_mach_msg_overwrite_trap                     32
#define SYS_semaphore_signal_trap                       33
#define SYS_semaphore_signal_all_trap                   34
#define SYS_semaphore_signal_thread_trap                35
#define SYS_semaphore_wait_trap                         36
#define SYS_semaphore_wait_signal_trap                  37
#define SYS_semaphore_timedwait_trap                    38
#define SYS_semaphore_timedwait_signal_trap             39
#define SYS__kernelrpc_mach_port_guard_trap             41
#define SYS__kernelrpc_mach_port_unguard_trap           42
#define SYS_task_name_for_pid                           44
#define SYS_task_for_pid                                45
#define SYS_pid_for_task                                46
#define SYS_macx_swapon                                 48
#define SYS_macx_swapoff                                49
#define SYS_macx_triggers                               51
#define SYS_macx_backing_store_suspend                  52
#define SYS_macx_backing_store_recovery                 53
#define SYS_pfz_exit                                    58
#define SYS_swtch_pri                                   59
#define SYS_swtch                                       60
#define SYS_thread_switch                               61
#define SYS_clock_sleep_trap                            62
#define SYS_mach_timebase_info_trap                     89
#define SYS_mach_wait_until_trap                        90
#define SYS_mk_timer_create_trap                        91
#define SYS_mk_timer_destroy_trap                       92
#define SYS_mk_timer_arm_trap                           93
#define SYS_mk_timer_cancel_trap                        94
#define SYS_iokit_user_client_trap                      100

#endif
