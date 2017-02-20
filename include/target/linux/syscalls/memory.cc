#ifndef SYSCALL_MEMORY_H_
#define SYSCALL_MEMORY_H_

/*
 * This file defines Linux specific system calls (i.e. not POSIX).
 */
namespace Syscall {

    #if SYSCALL_EXISTS(mmap2)
    SYSTEM_CALL void *  mmap2(void *, size_t, int, int, int, off_t);
    #endif
    SYSTEM_CALL void *  mremap(void *, size_t, size_t, int);

    #if SYSCALL_EXISTS(mmap2)
    SYSTEM_CALL
    void *mmap2(void *addr, size_t len, int prot, int flags, int filedes, off_t pgoff)
    {
        return (void *) DO_SYSCALL(mmap2, addr, len, prot, flags, filedes, pgoff);
    }
    #endif

    #if !SYSCALL_EXISTS(mmap) && SYSCALL_EXISTS(mmap2)
    SYSTEM_CALL
    void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
    {
        return mmap2(addr, len, prot, flags, fildes, off / Target::SIZEOF_PAGE);
    }
    #endif

    SYSTEM_CALL
    void *mremap(void *old_address, size_t old_size, size_t new_size, int flags)
    {
        return (void *) DO_SYSCALL(mremap, old_address, old_size, new_size, flags);
    }
}

#endif
