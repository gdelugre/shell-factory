#ifndef PICOLIB_MEMORY_H_
#define PICOLIB_MEMORY_H_

#include <utility>

namespace Pico {

    namespace Memory {

        template <typename T>
        FUNCTION
        constexpr bool is_size_aligned(size_t n)
        {
            return (n % sizeof(T) == 0);
        }

        template <typename T>
        FUNCTION
        constexpr T expand_char(char c)
        {
            static_assert(sizeof(T) <= sizeof(uint64_t), "Word size too big for expand_char");
            return (c << 0) |
                   (sizeof(T) > 1 ? (static_cast<T>(c) << 8) :  0) |
                   (sizeof(T) > 2 ? (static_cast<T>(c) << 16) : 0) |
                   (sizeof(T) > 3 ? (static_cast<T>(c) << 24) : 0) |
                   (sizeof(T) > 4 ? (static_cast<T>(c) << 32) : 0) |
                   (sizeof(T) > 5 ? (static_cast<T>(c) << 40) : 0) |
                   (sizeof(T) > 6 ? (static_cast<T>(c) << 48) : 0) |
                   (sizeof(T) > 7 ? (static_cast<T>(c) << 56) : 0);
        }

        template <typename T>
        FUNCTION
        void *copy_block(void *dest, const void *src, size_t n)
        {
            T *bdest = static_cast<T *>(dest);
            const T *bsrc = static_cast<const T *>(src);

            return tmemcpy(bdest, bsrc, n / sizeof(T));
        }

        template <typename T>
        FUNCTION
        void *set_block(void *dest, int c, size_t n)
        {
            T *bdest = static_cast<T *>(dest);
            T word = expand_char<T>(c);

            return tmemset(bdest, word, n / sizeof(T));
        }

        template <typename T>
        FUNCTION PURE
        int compare_block(const void *s1, const void *s2, size_t n)
        {
            const T *bs1 = static_cast<const T *>(s1);
            const T *bs2 = static_cast<const T *>(s2);

            return tmemcmp(bs1, bs2, n / sizeof(T));
        }

        FUNCTION
        void *copy(void *dest, const void *src, size_t n)
        {

            if ( Options::use_builtins )
                return BUILTIN(memcpy)(dest, src, n);

            return copy_block<uint8_t>(dest, src, n);
        }

        FUNCTION
        void *set(void *s, int c, size_t n)
        {
            if ( Options::use_builtins )
                return BUILTIN(memset)(s, c, n);

            return set_block<uint8_t>(s, c, n);
        }

        FUNCTION PURE
        int compare(const void *s1, const void *s2, size_t n)
        {
            if ( Options::use_builtins )
                return BUILTIN(memcmp)(s1, s2, n);

            return compare_block<uint8_t>(s1, s2, n);
        }

        FUNCTION
        void zero(void *s, size_t n)
        {
            set(s, 0, n);
        }

        //
        // Memory protection constants.
        //
        constexpr int READ   = (1 << 0);
        constexpr int WRITE  = (1 << 1);
        constexpr int EXEC   = (1 << 2);
        constexpr int STACK  = (1 << 3);

        FUNCTION PURE size_t page_size();
        FUNCTION void * allocate(void *base, size_t size, int prot);
        FUNCTION void * allocate(size_t size, int prot) {
            return allocate(nullptr, size, prot);
        }
        FUNCTION void * resize(void *ptr, size_t old_size, size_t new_size, bool can_move = true);
        FUNCTION void   release(void *ptr, size_t size);
        FUNCTION int    set_protection(void *ptr, size_t, int prot);

        FUNCTION PURE size_t round_up_page_size(size_t size)
        {
            size_t page_size = Memory::page_size();
            size_t nr_pages = (size + page_size - 1) / page_size;

            return nr_pages * page_size;
        }

        class BaseRegion
        {
            public:
                CONSTRUCTOR BaseRegion(void *address, size_t size, int prot = READ | WRITE) {
                    ptr = address;
                    region_size = round_up_page_size(size);
                    mem_prot = prot;
                }
                CONSTRUCTOR BaseRegion(BaseRegion&& o) : ptr(o.ptr), region_size(o.region_size), mem_prot(o.mem_prot) {
                    o.ptr = nullptr;
                    o.region_size = 0;
                }
                METHOD void *   pointer() const { return ptr; }
                METHOD size_t   size() const { return region_size; }

                METHOD int      set_protection(int prot) {
                    int ret = Memory::set_protection(ptr, region_size, prot);
                    if ( ret == 0 )
                        mem_prot = prot;
                    return ret;
                }

                // Automatic pointer cast.
                template <typename T>
                METHOD operator T *() const { return static_cast<T *>(ptr); }

                // Compare with pointer.
                METHOD bool operator ==(std::nullptr_t) {
                    return ptr == nullptr;
                }
                template <typename T>
                METHOD bool operator ==(T* p) {
                    return p == ptr;
                }

            protected:
                void *ptr;
                size_t region_size;
                int mem_prot;
        };

        class Region : public BaseRegion
        {
            public:
                CONSTRUCTOR Region(size_t size = page_size(), int prot = READ | WRITE)
                    : BaseRegion(Memory::allocate(size, prot), size) {}
                CONSTRUCTOR Region(void *base, size_t size = page_size(), int prot = READ | WRITE)
                    : BaseRegion(Memory::allocate(base, size, prot), size) {}
                CONSTRUCTOR Region(Region const& o) : Region(o.region_size, o.mem_prot) {
                    if ( ptr != nullptr )
                        Memory::copy(ptr, o.ptr, o.region_size);
                }
                CONSTRUCTOR Region(Region&& o) : BaseRegion(std::move(o)) {}

                DESTRUCTOR ~Region() {
                    if ( ptr != nullptr )
                        unmap();
                }

                // returns a Region object from an already existing mapped range.
                FUNCTION Region from(void *ptr, size_t size) {
                    return Region(NoAlloc{}, ptr, size);
                }

                METHOD int      resize(size_t new_size, bool can_move = true) {
                    new_size = round_up_page_size(new_size);
                    void *new_ptr = Memory::resize(ptr, region_size, new_size, can_move);
                    if ( new_ptr == nullptr )
                        return -1;

                    ptr = new_ptr;
                    region_size = new_size;
                    return 0;
                }

            private:
                struct NoAlloc {};
                CONSTRUCTOR Region(NoAlloc, void *ptr, size_t size) : BaseRegion(ptr, size) {}

                METHOD void unmap() {
                    Memory::release(ptr, region_size);
                }
        };
    }

    //
    // Pico::Heap is a custom memory allocator.
    //
    // Maintains two kind of structures in memory:
    //   - chunks holds the memory chunks allocated.
    //   - free_slots is an array of structures pointing to free memory areas.
    //
    class Heap
    {
        static constexpr size_t DEFAULT_SIZE = Options::heap_size;

        enum class ChunkType {
            STANDARD,
            BIG
        };

        // An allocated memory chunk.
        struct Chunk {
            size_t size;
            ChunkType type;
        };

        // A free memory entry in the free slots array.
        struct FreeEntry {
            void *base;
            size_t size;

            METHOD bool invalid() const { return base == nullptr; }
            METHOD void invalidate() { base = nullptr; size = 0; }
            METHOD void *start_address() const {
                return base;
            }

            METHOD void *end_address() const {
                return static_cast<uint8_t *>(base) + size - 1;
            }
        };

        // Class managing the list of free slots.
        class FreeSlots {
            public:
                CONSTRUCTOR FreeSlots(void *base, size_t size) : memory_start(base), free_space(0), nr_slots(0) {
                    FreeEntry *first_entry = first();

                    first_entry->base = base;
                    first_entry->size = size;
                    nr_slots = 1;
                    memory_end = first_entry->end_address();
                    free_space = size;
                }

                METHOD int increase_memory_size(size_t extra)
                {
                    void *new_space = static_cast<uint8_t *>(memory_end) + 1;
                    memory_end = static_cast<uint8_t *>(memory_end) + extra;

                    if ( add_slot(new_space, extra) == nullptr )
                        return -1;

                    return 0;
                }

                //
                // Mark a given range of memory as unallocated.
                //
                METHOD void unallocate(void *address, size_t size)
                {
                    FreeEntry *new_entry = add_slot(address, size);

                    assert(new_entry != nullptr);
                }

                //
                // Find an empty memory area of a given size and returns a pointer to it.
                //
                METHOD void *consume(size_t size)
                {
                    // Not enough space available, early exit.
                    if ( size > free_space )
                        return nullptr;

                    FreeEntry *entry = find(size);
                    if ( entry == nullptr )
                        return nullptr;

                    void *ptr = entry->base;

                    if ( entry->size == size ) {
                        entry->invalidate();
                        nr_slots--;
                    }
                    else {
                        entry->base = static_cast<uint8_t *>(ptr) + size;
                        entry->size -= size;
                    }

                    free_space -= size;
                    return ptr;
                }

                METHOD size_t available_space() const { return free_space; }

            private:
                METHOD FreeEntry *first() const {
                    return slots;
                }

                METHOD int expand(size_t extra)
                {
                    int ret = slots.resize(max_slots * sizeof(FreeEntry) + extra);
                    if ( ret != 0 )
                        return ret;

                    max_slots = slots.size() / sizeof(FreeEntry);
                    return 0;
                }

                // Find a free entry whose size is at least the specified size.
                METHOD FreeEntry *find(size_t min_size) const
                {
                    FreeEntry *entries = first();
                    for ( size_t i = 0; i < nr_slots; i++ )
                    {
                        FreeEntry *entry = entries++;

                        if ( entry->invalid() )
                            continue;

                        if ( entry->size >= min_size )
                            return entry;
                    }

                    return nullptr;
                }

                // Find a free entry including a particular address.
                METHOD FreeEntry *find_by_address(void *address) const
                {
                    FreeEntry *entries = first();
                    for ( size_t i = 0; i < nr_slots; i++ )
                    {
                        FreeEntry *entry = entries++;

                        if ( entry->invalid() )
                            continue;

                        if ( address >= entry->start_address() && address <= entry->end_address() )
                            return entry;
                    }

                    return nullptr;
                }

                METHOD FreeEntry *get_unused_slot()
                {
                    FreeEntry *entries = first();
                    for ( size_t i = 0; i < max_slots; i++ ) {
                        FreeEntry *entry = entries++;

                        if ( entry->invalid() )
                            return entry;
                    }

                    // No available slot, expand the slot area.
                    int ret = expand(sizeof(FreeEntry));
                    if ( ret != 0 )
                        return nullptr;

                    // Retry.
                    return get_unused_slot();
                }

                METHOD FreeEntry *add_slot(void *address, size_t size)
                {
                    assert(is_valid_range(address, size));
                    void *prev_address = static_cast<uint8_t *>(address) - 1;

                    // If an entry already exists, then just expand it.
                    FreeEntry *prev_entry = find_by_address(prev_address);
                    if ( prev_entry != nullptr ) {
                        assert(prev_entry->end_address() == prev_address);

                        prev_entry->size += size;
                        free_space += size;
                        return prev_entry;
                    }

                    // Otherwise, create a new entry.
                    FreeEntry *new_slot = get_unused_slot();
                    if ( new_slot == nullptr )
                        return nullptr;

                    new_slot->base = address;
                    new_slot->size = size;

                    // Merge with an adjacent slot if possible.
                    void *next_address = static_cast<uint8_t *>(new_slot->end_address()) + 1;
                    FreeEntry *next_entry = find_by_address(next_address);
                    if ( next_entry != nullptr ) {
                        assert(next_entry->start_address() == next_address);

                        new_slot->size += next_entry->size;
                        next_entry->invalidate();
                        nr_slots--;
                    }

                    nr_slots++;
                    free_space += size;

                    return new_slot;
                }

                METHOD bool is_valid_address(void *address) {
                    return address >= memory_start && address <= memory_end;
                }

                METHOD bool is_valid_range(void *address, size_t size) {
                    void *end_address = static_cast<uint8_t *>(address) + size - 1;
                    return is_valid_address(address) && is_valid_address(end_address);
                }

                Memory::Region slots;
                size_t max_slots = slots.size() / sizeof(FreeEntry);
                const void *memory_start;
                void *memory_end;
                size_t free_space = 0;
                size_t nr_slots = 0;
        };

        public:
            CONSTRUCTOR Heap(size_t size = DEFAULT_SIZE) : chunks(Options::heap_address, size), heap_size(size), total_size(size), free_slots(chunks, heap_size) {}
            METHOD void *allocate(size_t count);
            METHOD void free(void *ptr);
            METHOD void free(void *ptr, size_t size);
            METHOD size_t size() const { return heap_size; }
            METHOD size_t free_space() const { return free_slots.available_space(); }
            METHOD size_t used_space() const { return total_size - free_space(); }
            METHOD size_t allocated_objects() const { return nr_objects; }
            FUNCTION size_t entry_size(void *ptr);
            FUNCTION Heap& global();

        private:
            Mutex mutex;
            Memory::Region chunks;
            size_t heap_size;
            size_t total_size;
            FreeSlots free_slots;
            size_t nr_objects = 0;
            const size_t big_chunk_threshold = Memory::page_size();

            METHOD void *find_free_space(size_t slot_size);
            METHOD int grow(size_t extra);
    };

    METHOD
    void *Heap::find_free_space(size_t size)
    {
        return free_slots.consume(size);
    }

    METHOD int Heap::grow(size_t extra)
    {
        int ret;

        ret = chunks.resize(heap_size + extra, false);
        if ( ret != 0 )
            return ret;

        size_t new_heap_size = chunks.size();

        // Compute actual extra extra size.
        extra = new_heap_size - heap_size;
        heap_size = new_heap_size;
        total_size += extra;

        // More space is available now.
        // Free slots needs to be updated to reflect this change.
        ret = free_slots.increase_memory_size(extra);
        if ( ret != 0 )
            return ret;

        return 0;
    }

    METHOD
    void *Heap::allocate(size_t count)
    {
        CriticalSection critical(mutex);

        size_t needed_size = sizeof(Chunk) + count;

        // Big chunks are allocated into separate memory areas.
        if ( needed_size >= big_chunk_threshold ) {
            size_t alloc_size = Memory::round_up_page_size(needed_size);
            struct Chunk *chunk = static_cast<Chunk *>(Memory::allocate(alloc_size, Memory::READ | Memory::WRITE));

            chunk->type = ChunkType::BIG;
            chunk->size = alloc_size - sizeof(Chunk);
            nr_objects++;
            total_size += alloc_size;

            void *ptr = chunk + 1;

            return ptr;
        }

        Chunk *chunk;
        while ( (chunk = static_cast<Chunk *>(find_free_space(needed_size))) == nullptr )
        {
            // No free space available, grow the heap.
            if ( grow(needed_size) != 0 )
                return nullptr;
        }

        chunk->type = ChunkType::STANDARD;
        chunk->size = count;
        nr_objects++;

        void *ptr = chunk + 1;
        return ptr;
    }

    METHOD
    void Heap::free(void *ptr, size_t size)
    {
        if ( ptr == nullptr )
            return;

        CriticalSection critical(mutex);

        struct Chunk *chunk = reinterpret_cast<Chunk *>(static_cast<char *>(ptr) - sizeof(Chunk));
        size_t used_size = sizeof(Chunk) + chunk->size;

        // Sized-deallocation is not supported.
        assert(size == chunk->size);

        // Big chunks have their own dedicated memory region, just release it.
        if ( chunk->type == ChunkType::BIG ) {
            Memory::release(chunk, used_size);

            nr_objects--;
            total_size -= used_size;

            return;
        }

        assert(chunk->type == ChunkType::STANDARD);

        chunk->size = 0;
        free_slots.unallocate(chunk, used_size);
        nr_objects--;
    }

    METHOD
    void Heap::free(void *ptr)
    {
        if ( ptr == nullptr )
            return;

        free(ptr, entry_size(ptr));
    }

    METHOD
    size_t Heap::entry_size(void *ptr)
    {
        struct Chunk *chunk = reinterpret_cast<Chunk *>(static_cast<char *>(ptr) - sizeof(Chunk));

        return chunk->size;
    }

    //
    // Returns the default global Heap.
    //
    METHOD
    Heap& Heap::global()
    {
        static Heap default_heap;

        return default_heap;
    }
}

//
// Global memory allocation functions.
//

void *operator new(size_t count)
{
    return Pico::Heap::global().allocate(count);
}

void *operator new[](size_t count)
{
    return Pico::Heap::global().allocate(count);
}

void *operator new(size_t, void *ptr)
{
    return ptr;
}

void *operator new[](size_t, void *ptr)
{
    return ptr;
}

void operator delete(void *ptr) noexcept
{
    return Pico::Heap::global().free(ptr);
}

void operator delete[](void *ptr) noexcept
{
    return Pico::Heap::global().free(ptr);
}

void operator delete(void *ptr, size_t size) noexcept
{
    return Pico::Heap::global().free(ptr, size);
}

void operator delete[](void *ptr, size_t size) noexcept
{
    return Pico::Heap::global().free(ptr, size);
}

#endif
