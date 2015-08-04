#ifndef PICOLIB_MEMORY_IMPL_H_
#define PICOLIB_MEMORY_IMPL_H_

namespace Pico {

    namespace Memory {

        // Converts pico memory protection to target protection.
        constexpr int mmap_prot(int pico_prot)
        {
            unsigned int prot = 0;

            if ( pico_prot & Memory::READ )
                prot |= PROT_READ;
            if ( pico_prot & Memory::WRITE )
                prot |= PROT_WRITE;
            if ( pico_prot & Memory::EXEC )
                prot |= PROT_EXEC;

            return prot;
        }

        METHOD
        void *allocate(size_t size, int prot)
        {
            return Syscall::mmap(NULL, size, mmap_prot(prot),
                                 MAP_ANONYMOUS | MAP_PRIVATE,
                                 0, 0);
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
