#ifndef PICOLIB_IPC_H_
#define PICOLIB_IPC_H_

namespace Pico {

    class Pipe : public BasicStream {
        public:
            using BasicStream::BasicStream;
            FUNCTION BiStream<Pipe> create();
    };

    namespace Memory {

        class SharedRegion {
            public:
                using Rights = Filesystem::Rights;
                using shm_handle = Target::Memory::shm_handle;

                CONSTRUCTOR SharedRegion(const char *name, void *base, size_t size, int prot, Rights rights);
                DESTRUCTOR ~SharedRegion();

                // Automatic pointer cast.
                template <typename T>
                METHOD operator T *() const { return static_cast<T *>(ptr); }

                METHOD size_t size() const { return region_size; }

            private:
                void *ptr;
                size_t region_size;
                shm_handle shm_obj;
        };
    }
}

#endif
