#ifndef PICOLIB_MEMORY_IMPL_H_
#define PICOLIB_MEMORY_IMPL_H_

namespace Pico {

    namespace Memory {

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
        void release(void *ptr, size_t size)
        {
            Syscall::munmap(ptr, size);
        }

        CONSTRUCTOR
        Buffer::Buffer(size_t size, int prot) : size_(size) 
        {
            ptr = Memory::allocate(size, prot);
        }

        METHOD
        void Buffer::resize(size_t new_size)
        {
            ptr = Syscall::mremap(ptr, size_, new_size, MREMAP_MAYMOVE);
            size_ = new_size;
        }

        METHOD
        void Buffer::free()
        {
            Memory::release(ptr, size_);
        }
    }
}

#endif
