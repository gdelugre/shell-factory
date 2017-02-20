#ifndef POSIX_PICO_MEMORY_H_
#define POSIX_PICO_MEMORY_H_

namespace Pico {

    namespace Memory {

        FUNCTION
        size_t page_size()
        {
            return Target::SIZEOF_PAGE;
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
                flags |= Target::Flags::mmap_stack_flag;

            void *ptr = Syscall::mmap(base, size, mmap_prot(prot), flags, -1, 0);
            if ( Target::is_error(ptr) )
                return nullptr;
            else
                return ptr;
        }

        METHOD
        void release(void *ptr, size_t size)
        {
            Syscall::munmap(ptr, size);
        }

        METHOD
        int set_protection(void *ptr, size_t size, int prot)
        {
            return Syscall::mprotect(ptr, size, mmap_prot(prot));
        }
    }
}

#endif
