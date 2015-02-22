#ifndef PICOLIB_MEMORY_H_
#define PICOLIB_MEMORY_H_

typedef struct {
    size_t  mchunk_size;
    char    mchunk_data[0];
} mchunk_t;

#define PAGE_BASE(address) ((void *)(((unsigned long) address) & ~(PAGE_SIZE - 1)))
#define NUM_PAGES(size) ((size + PAGE_SIZE - 1) / PAGE_SIZE)
#define ROUND_PAGE(size) (NUM_PAGES(size) * PAGE_SIZE)

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
