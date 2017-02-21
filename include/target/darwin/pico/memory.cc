#ifndef PICOLIB_MEMORY_IMPL_H_
#define PICOLIB_MEMORY_IMPL_H_

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
            // XXX: Is there an equivalent to MAP_STACK on OS X?
            int flags = MAP_ANONYMOUS | MAP_PRIVATE;

            void *ptr = Syscall::mmap(base, size, mmap_prot(prot), flags, -1, 0);
            if ( Target::is_error(ptr) )
                return nullptr;
            else
                return ptr;
        }

        METHOD
        void *resize(void *ptr, size_t old_size, size_t new_size, bool can_move)
        {
            void *base;
            size_t size;

            // Round up sizes to a page boundary.
            old_size = round_up_page_size(old_size);
            new_size = round_up_page_size(new_size);

            // No change needed.
            if ( old_size == new_size )
                return ptr;

            // Shrinking the area.
            if ( new_size < old_size ) {
                void *free_base = static_cast<uint8_t *>(ptr) + new_size;

                release(free_base, old_size - new_size);
                return ptr;
            }

            // Expanding the area.
            if ( can_move ) {
                base = nullptr; 
                size = new_size;
            }
            else {
                base = static_cast<uint8_t *>(ptr) + old_size;
                size = new_size - old_size;
            }
                
            // TODO: handle prot parameter.
            void *new_ptr = allocate(base, size, Memory::READ | Memory::WRITE);
            if ( Target::is_error(new_ptr) )
                return nullptr;

            if ( new_ptr != ptr )
                Memory::copy(new_ptr, ptr, old_size);

            release(ptr, old_size); 
            return new_ptr;
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
