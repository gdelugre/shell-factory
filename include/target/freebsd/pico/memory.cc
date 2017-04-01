#ifndef PICOLIB_MEMORY_IMPL_H_
#define PICOLIB_MEMORY_IMPL_H_

namespace Pico {

    namespace Memory {

        METHOD
        void *resize(void *ptr, size_t old_size, size_t new_size, int old_prot, bool can_move)
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
                
            void *new_ptr = allocate(base, size, old_prot);
            if ( Target::is_error(new_ptr) )
                return nullptr;

            if ( new_ptr != ptr )
                Memory::copy(new_ptr, ptr, old_size);

            release(ptr, old_size); 
            return new_ptr;
        }
    }
}

#endif
