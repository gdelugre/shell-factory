#ifndef PICOLIB_MEMORY_IMPL_H_
#define PICOLIB_MEMORY_IMPL_H_

namespace Pico {

    namespace Memory {

        METHOD
        void *resize(void *ptr, size_t old_size, size_t new_size, bool can_move)
        {
            int flags = 0;

            if ( can_move )
                flags |= MREMAP_MAYMOVE;

            ptr = Syscall::mremap(ptr, old_size, new_size, flags);
            if ( Target::is_error(ptr) )
                return nullptr;
            else
                return ptr;
        }

    }
}

#endif
