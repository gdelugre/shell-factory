#ifndef POSIX_SYSCALL_MEMORY_H_
#define POSIX_SYSCALL_MEMORY_H_

#include <sys/mman.h>

namespace Syscall {

    SYSTEM_CALL int     mprotect(void *, size_t, int);
    SYSTEM_CALL void *  mmap(void *, size_t, int, int, int, off_t);
    SYSTEM_CALL int     munmap(void *, size_t);
    SYSTEM_CALL int     msync(void *, size_t, int);
    SYSTEM_CALL int     mlock(const void *, size_t);
    SYSTEM_CALL int     munlock(const void *, size_t);
    SYSTEM_CALL int     mlockall(int);
    SYSTEM_CALL int     munlockall();

    SYSTEM_CALL
    int mprotect(void *addr, size_t len, int prot)
    {
        return DO_SYSCALL(mprotect, addr, len, prot);
    }

    // Linux sometimes uses another system call named mmap2.
    #if SYSCALL_EXISTS(mmap)
    SYSTEM_CALL
    void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
    {
        return (void *) DO_SYSCALL(mmap, addr, len, prot, flags, fildes, off);
    }
    #endif
    
    SYSTEM_CALL
    int munmap(void *addr, size_t len)
    {
        return DO_SYSCALL(munmap, addr, len);
    }

    SYSTEM_CALL
    int msync(void *addr, size_t len, int flags)
    {
        return DO_SYSCALL(msync, addr, len, flags);
    }

    SYSTEM_CALL
    int mlock(const void *addr, size_t len)
    {
        return DO_SYSCALL(mlock, addr, len);
    }

    SYSTEM_CALL
    int munlock(const void *addr, size_t len)
    {
        return DO_SYSCALL(munlock, addr, len);
    }

    SYSTEM_CALL
    int mlockall(int flags)
    {
        return DO_SYSCALL(mlockall, flags);
    }

    SYSTEM_CALL
    int munlockall()
    {
        return DO_SYSCALL(munlockall);
    }
}

#endif
