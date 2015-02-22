#ifndef SYSCALL_MEMORY_H_
#define SYSCALL_MEMORY_H_

#ifndef __USE_GNU
#define __USE_GNU 1
#endif

#include <sys/mman.h>

/* 
 * System calls defined in this file.
 */
namespace Syscall {

    SYSTEM_CALL int     mprotect(void *, size_t, int);
    SYSTEM_CALL void *  mmap(void *, size_t, int, int, int, off_t);
    #if SYSCALL_EXISTS(mmap2)
    SYSTEM_CALL void *  mmap2(void *, size_t, int, int, int, off_t);
    #endif
    SYSTEM_CALL void *  mremap(void *, size_t, size_t, int);
    SYSTEM_CALL int     munmap(void *, size_t);

    SYSTEM_CALL
    int mprotect(void *addr, size_t len, int prot)
    {
        return DO_SYSCALL(mprotect, 3, addr, len, prot);
    }

    #if SYSCALL_EXISTS(mmap2)
    SYSTEM_CALL
    void *mmap2(void *addr, size_t len, int prot, int flags, int filedes, off_t pgoff)
    {
        return (void *) DO_SYSCALL(mmap2, 6, addr, len, prot, flags, filedes, pgoff);
    }
    #endif

    SYSTEM_CALL
    void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
    {
    #if SYSCALL_EXISTS(mmap)
        return (void *) DO_SYSCALL(mmap, 6, addr, len, prot, flags, fildes, off);
    #else
        return Syscall::mmap2(addr, len, prot, flags, fildes, off / PAGE_SIZE);
    #endif
    }

    SYSTEM_CALL
    void *mremap(void *old_address, size_t old_size, size_t new_size, int flags)
    {
        return (void *) DO_SYSCALL(mremap, 4, old_address, old_size, new_size, flags);
    }

    SYSTEM_CALL
    int munmap(void *addr, size_t len)
    {
        return DO_SYSCALL(munmap, 2, addr, len);
    }
}

#endif
