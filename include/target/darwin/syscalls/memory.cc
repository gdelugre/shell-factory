#ifndef SYSCALL_MEMORY_H_
#define SYSCALL_MEMORY_H_

#include <sys/mman.h>

/*
 * System calls defined in this file.
 */
namespace Syscall {

    SYSTEM_CALL int     mprotect(void *, size_t, int);
    SYSTEM_CALL void *  mmap(void *, size_t, int, int, int, off_t);
    SYSTEM_CALL int     munmap(void *, size_t);

    SYSTEM_CALL
    int mprotect(void *addr, size_t len, int prot)
    {
        return DO_SYSCALL(mprotect, addr, len, prot);
    }

    SYSTEM_CALL
    void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
    {
        return (void *) DO_SYSCALL(mmap, addr, len, prot, flags, fildes, off);
    }

    SYSTEM_CALL
    int munmap(void *addr, size_t len)
    {
        return DO_SYSCALL(munmap, addr, len);
    }
}

#endif
