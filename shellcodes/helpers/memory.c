#ifndef _MEMORY_HELPER_H
#define _MEMORY_HELPER_H

#define __USE_GNU

#include <factory.h>
#include <sys/mman.h>

#include "cpu.c"
#include "string.c"

typedef struct {
    size_t  mchunk_size;
    char    mchunk_data[0];
} mchunk_t;

#define PAGE_BASE(address) ((void *)(((unsigned long) address) & ~(PAGE_SIZE - 1)))
#define NUM_PAGES(size) ((size + PAGE_SIZE - 1) / PAGE_SIZE)
#define ROUND_PAGE(size) (NUM_PAGES(size) * PAGE_SIZE)

SYSTEM_CALL
int _mprotect(void *addr, size_t len, int prot)
{
    return DO_SYSCALL(mprotect, 3, addr, len, prot);
}

SYSTEM_CALL
void *_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
    return (void *) DO_SYSCALL(mmap, 6, addr, len, prot, flags, fildes, off);
}

SYSTEM_CALL
void *_mremap(void *old_address, size_t old_size, size_t new_size, int flags)
{
    return (void *) DO_SYSCALL(mremap, 4, old_address, old_size, new_size, flags);
}

SYSTEM_CALL
int _munmap(void *addr, size_t len)
{
    return DO_SYSCALL(munmap, 2, addr, len);
}

FUNCTION 
void *allocate_memory(size_t size, int prot)
{
    return _mmap(NULL, size, prot, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
}

FUNCTION
void release_memory(void *ptr, size_t size)
{
    _munmap(ptr, size);
}

FUNCTION
void *_malloc(size_t size)
{
    size_t chunk_size = sizeof(size_t) + size;
    mchunk_t *mchunk = _mmap(NULL, 
                        chunk_size,
                        PROT_READ|PROT_WRITE, 
                        MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);

    mchunk->mchunk_size = chunk_size;
    return &mchunk->mchunk_data;
}

FUNCTION 
void *_realloc(void *ptr, size_t new_size)
{
    size_t chunk_size = new_size + sizeof(size_t);
    mchunk_t *mchunk = ptr - sizeof(size_t);

    mchunk = _mremap(mchunk, mchunk->mchunk_size, chunk_size, MREMAP_MAYMOVE);
    mchunk->mchunk_size = chunk_size;

    return &mchunk->mchunk_data;
}

FUNCTION
void _free(void *ptr)
{
    mchunk_t *mchunk = ptr - sizeof(size_t);
    _munmap(mchunk, mchunk->mchunk_size); 
}

#endif
