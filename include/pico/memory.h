#ifndef PICOLIB_MEMORY_H_
#define PICOLIB_MEMORY_H_

namespace Pico {

    namespace Memory {

        constexpr int READ   = (1 << 0);
        constexpr int WRITE  = (1 << 1);
        constexpr int EXEC   = (1 << 2);
        constexpr int STACK  = (1 << 3);

        FUNCTION void * allocate(size_t size, int prot);
        FUNCTION void   release(void *ptr, size_t size);

        class Buffer
        {
            public:
                CONSTRUCTOR     Buffer() : ptr(nullptr), size_(0) {}
                CONSTRUCTOR     Buffer(size_t size, int prot);
                CONSTRUCTOR     Buffer(size_t size) : Buffer(size, READ | WRITE) {}
                METHOD void *   pointer() const { return ptr; }
                METHOD size_t   size() const { return size_; }
                METHOD void     resize(size_t new_size);
                METHOD void     free();

                template <typename T>
                METHOD T        as() { return static_cast<T>(ptr); }

            private:
                void *ptr;
                size_t size_;
        };

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
            static_assert(sizeof(T) <= 64, "Word size too big for memset");
            return (c << 0) | (c << 8) | (c << 16) | (c << 24) |
                   (c << 32) | (c << 40) | (c << 48) | (c << 56);
        }

        template <typename T>
        FUNCTION
        void *copy_block(void *dest, const void *src, size_t n)
        {
            T *bdest = static_cast<T *>(dest);
            const T *bsrc = static_cast<const T *>(src);

            for ( size_t i = 0; i < n / sizeof(T); i++ )
                bdest[i] = bsrc[i];

            return dest;
        }

        template <typename T>
        FUNCTION
        void *set_block(void *dest, int c, size_t n)
        {
            T *bdest = static_cast<T *>(dest);
            T word = expand_char<T>(c);

            for ( size_t i = 0; i < n / sizeof(T); i++ )
                bdest[i] = word;

            return dest;
        }

        template <typename T>
        FUNCTION
        int compare_block(const void *s1, const void *s2, size_t n)
        {
            const T *bs1 = static_cast<const T *>(s1);
            const T *bs2 = static_cast<const T *>(s2);

            for ( size_t i = 0; i < n / sizeof(T); i++ )
                if ( bs1[i] < bs2[i] )
                    return -1;
                else
                    return +1;
            return 0;
        }

        FUNCTION
        void *copy(void *dest, const void *src, size_t n)
        {

            if ( Options::use_builtins )
                return BUILTIN(memcpy)(dest, src, n);

            if ( is_size_aligned<uint64_t>(n) )
                return copy_block<uint64_t>(dest, src, n);

            if ( is_size_aligned<uint32_t>(n) )
                return copy_block<uint32_t>(dest, src, n);

            if ( is_size_aligned<uint16_t>(n) )
                return copy_block<uint16_t>(dest, src, n);

            return copy_block<uint8_t>(dest, src, n);
        }

        FUNCTION
        void *set(void *s, int c, size_t n)
        {
            if ( Options::use_builtins )
                return BUILTIN(memset)(s, c, n);

            if ( is_size_aligned<uint64_t>(n) )
                return set_block<uint64_t>(s, c, n);

            if ( is_size_aligned<uint32_t>(n) )
                return set_block<uint32_t>(s, c, n);

            if ( is_size_aligned<uint16_t>(n) )
                return set_block<uint16_t>(s, c, n);

            return set_block<uint8_t>(s, c, n);
        }

        FUNCTION
        int compare(const void *s1, const void *s2, size_t n)
        {
            if ( Options::use_builtins )
                return BUILTIN(memcmp)(s1, s2, n);

            if ( is_size_aligned<uint64_t>(n) )
                return compare_block<uint64_t>(s1, s2, n);

            if ( is_size_aligned<uint32_t>(n) )
                return compare_block<uint32_t>(s1, s2, n);

            if ( is_size_aligned<uint16_t>(n) )
                return compare_block<uint16_t>(s1, s2, n);

            return compare_block<uint8_t>(s1, s2, n);
        }

        FUNCTION
        void zero(void *s, size_t n)
        {
            set(s, 0, n);
        }
    }
}

#endif
