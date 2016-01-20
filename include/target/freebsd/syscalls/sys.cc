#ifndef SYSCALL_SYS_H_
#define SYSCALL_SYS_H_

#include <sys/types.h>
#include <sys/sysctl.h>

/*
 * Syscalls defined in this file.
 */
namespace Syscall {

    SYSTEM_CALL int sysctl(const int *, u_int, void *, size_t *, const void *, size_t);

    SYSTEM_CALL 
    int sysctl(const int *name, u_int namelen, 
               void *oldp, size_t *oldlenp, const void *newp, size_t newlen)
    {
        return DO_SYSCALL(__sysctl, name, namelen, oldp, oldlenp, newp, newlen);
    }
}

#endif
