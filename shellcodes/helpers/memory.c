#ifndef _MEMORY_HELPER_H
#define _MEMORY_HELPER_H

#include <factory.h>

#include <sys/mman.h>

static inline
void *_malloc(size_t size)
{
    return (void *) INTERNAL_SYSCALL(mmap,, 6, NULL, size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
}

static inline
void _free(void *ptr, size_t size)
{
    INTERNAL_SYSCALL(munmap,, 2, ptr, size);
}

#endif
