#ifndef PICOLIB_MEMORY_H_
#define PICOLIB_MEMORY_H_

namespace Pico {

    namespace Memory {

        constexpr int READ   = (1 << 0);
        constexpr int WRITE  = (1 << 1);
        constexpr int EXEC   = (1 << 2);

        METHOD void * allocate(size_t size, int prot);
        METHOD void   release(void *ptr, size_t size);

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
    }
}

#endif
