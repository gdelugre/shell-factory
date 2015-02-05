#ifndef _MEMORY_HELPER_H
#define _MEMORY_HELPER_H

#ifndef __USE_GNU
#define __USE_GNU 1
#endif

#include <factory.h>
#include <sys/mman.h>

typedef struct {
    size_t  mchunk_size;
    char    mchunk_data[0];
} mchunk_t;

#define PAGE_BASE(address) ((void *)(((unsigned long) address) & ~(PAGE_SIZE - 1)))
#define NUM_PAGES(size) ((size + PAGE_SIZE - 1) / PAGE_SIZE)
#define ROUND_PAGE(size) (NUM_PAGES(size) * PAGE_SIZE)

/* 
 * System calls defined in this file.
 */
namespace Syscall {

    SYSTEM_CALL int     mprotect(void *, size_t, int);
    SYSTEM_CALL void *  mmap(void *, size_t, int, int, int, off_t);
    SYSTEM_CALL void *  mremap(void *, size_t, size_t, int);
    SYSTEM_CALL int     munmap(void *, size_t);

    SYSTEM_CALL
    int mprotect(void *addr, size_t len, int prot)
    {
        return DO_SYSCALL(mprotect, 3, addr, len, prot);
    }

    SYSTEM_CALL
    void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
    {
    #if (defined(__arm__) && defined(__ARM_EABI__))
        return arch_sys_mmap(addr, len, prot, flags, fildes, off);
    #else
        return (void *) DO_SYSCALL(mmap, 6, addr, len, prot, flags, fildes, off);
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

#include "cpu.c"
#include "string.c"

FUNCTION 
void *allocate_memory(size_t size, int prot)
{
    return Syscall::mmap(NULL, size, prot, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
}

FUNCTION
void release_memory(void *ptr, size_t size)
{
    Syscall::munmap(ptr, size);
}

FUNCTION
void *_malloc(size_t size)
{
    size_t chunk_size = sizeof(size_t) + size;
    mchunk_t *mchunk = static_cast<mchunk_t *>(
        allocate_memory(chunk_size, PROT_READ|PROT_WRITE)
    );

    mchunk->mchunk_size = chunk_size;
    return &mchunk->mchunk_data;
}

FUNCTION 
void *_realloc(void *ptr, size_t new_size)
{
    size_t chunk_size = new_size + sizeof(size_t);
    mchunk_t *mchunk = static_cast<mchunk_t *>((void *)((char *)ptr - sizeof(size_t)));

    mchunk = static_cast<mchunk_t *>(
        Syscall::mremap(mchunk, mchunk->mchunk_size, chunk_size, MREMAP_MAYMOVE)
    );
    mchunk->mchunk_size = chunk_size;

    return &mchunk->mchunk_data;
}

FUNCTION
void _free(void *ptr)
{
    mchunk_t *mchunk = static_cast<mchunk_t *>((void *)((char *) ptr - sizeof(size_t)));
    Syscall::munmap(mchunk, mchunk->mchunk_size); 
}

#endif
