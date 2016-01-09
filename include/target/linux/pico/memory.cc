#ifndef PICOLIB_MEMORY_IMPL_H_
#define PICOLIB_MEMORY_IMPL_H_

#include <sys/user.h>

namespace Pico {

    namespace Memory {

        FUNCTION
        size_t page_size()
        {
            return PAGE_SIZE;
        }

        // Converts pico memory protection to target protection.
        // TODO: would be better with a nicely formatted constexpr, but keep it as a single return for gcc <=4.9 for now.
        constexpr int mmap_prot(int pico_prot)
        {
            return (pico_prot & Memory::READ ? PROT_READ : 0) |
                   (pico_prot & Memory::WRITE ? PROT_WRITE : 0) |
                   (pico_prot & Memory::EXEC ? PROT_EXEC : 0);
        }

        METHOD
        void *allocate(void *base, size_t size, int prot)
        {
            int flags = MAP_ANONYMOUS | MAP_PRIVATE;
            if ( prot & Memory::STACK )
                flags |= MAP_GROWSDOWN;

            return Syscall::mmap(base, size, mmap_prot(prot), flags, 0, 0);
        }

        METHOD
        void *resize(void *ptr, size_t old_size, size_t new_size)
        {
            return Syscall::mremap(ptr, old_size, new_size, MREMAP_MAYMOVE);
        }

        METHOD
        void release(void *ptr, size_t size)
        {
            Syscall::munmap(ptr, size);
        }
    }
}

#endif
