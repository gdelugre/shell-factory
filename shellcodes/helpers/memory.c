#ifndef _MEMORY_HELPER_H
#define _MEMORY_HELPER_H

#include <factory.h>
#include <sys/mman.h>

#include "cpu.c"

#define PAGE_BASE(address) ((void *)(((unsigned long) address) & ~(PAGE_SIZE - 1)))
#define NUM_PAGES(size) ((size + PAGE_SIZE - 1) / PAGE_SIZE)
#define ROUND_PAGE(size) (NUM_PAGES(size) * PAGE_SIZE)

SYSTEM_CALL
int _mprotect(void *addr, size_t len, int prot)
{
    return INTERNAL_SYSCALL(mprotect,, 3, addr, len, prot);
}

SYSTEM_CALL
void *_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
    return (void *) INTERNAL_SYSCALL(mmap,, 6, addr, len, prot, flags, fildes, off);
}

SYSTEM_CALL
int _munmap(void *addr, size_t len)
{
    return INTERNAL_SYSCALL(munmap,, 2, addr, len);
}

FUNCTION
void *_malloc(size_t size)
{
    return _mmap(NULL, size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
}

FUNCTION
void _free(void *ptr, size_t size)
{
    _munmap(ptr, size); 
}

#endif
