#ifndef _ARCH_ARM_HELPER_H
#define _ARCH_ARM_HELPER_H

#define PAGE_SIZE 4096

static inline 
void *get_sp(void)
{
    void *sp;
    asm volatile("mov %0, sp\n" : "=r" (sp));
    return sp;
}

static inline 
void set_sp(void *sp)
{
    asm volatile("mov sp, %0\n" :: "r" (sp));
}

static inline
void add_sp(size_t offset)
{
    asm volatile("add sp, %0\n" :: "I" (offset));
}

static inline
void sub_sp(size_t offset)
{
    asm volatile("sub sp, %0\n" :: "I" (offset));
}

/* SYSCALLS */

/* Values defined in <linux/time.h>
 * Conflicts with <time.h> ...
 */
#define ITIMER_REAL 0
#define ITIMER_VIRTUAL 1
#define ITIMER_PROF 2

struct itimerval {
    struct timeval it_interval;
    struct timeval it_value;
};

static inline
unsigned int arch_sys_alarm(unsigned int seconds)
{
    struct itimerval val = {{0}};

    val.it_value.tv_sec = seconds;
    DO_SYSCALL(setitimer, 3, ITIMER_REAL, &val, &val);
    
    return val.it_value.tv_sec;
}

static inline
void *arch_sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    return (void *) DO_SYSCALL(mmap2, 6, addr, length, prot, flags, fd, offset / PAGE_SIZE);
}

#endif
