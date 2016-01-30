#ifndef PICOLIB_IPC_H_
#define PICOLIB_IPC_H_

namespace Pico {

    class Pipe : public BasicStream {
        public:
            using BasicStream::BasicStream;
            FUNCTION BiStream<Pipe> create();
    };

    namespace Memory {

        class SharedRegion : BaseRegion {
            public:
                using Rights = Filesystem::Rights;
                using shm_handle = Target::Memory::shm_handle;
                static constexpr auto default_rights = Target::Memory::default_shm_rights;

                template <int N>
                CONSTRUCTOR SharedRegion(const char (&name)[N], void *base, size_t size,
                                         int prot = READ|WRITE, Rights rights = default_rights);

                template <int N>
                CONSTRUCTOR SharedRegion(const char (&name)[N], size_t size,
                                         int prot = READ|WRITE, Rights rights = default_rights)
                            : SharedRegion(name, nullptr, size, prot, rights) {}

                DESTRUCTOR ~SharedRegion();

            private:
                shm_handle shm_obj;
        };
    }
}

#endif
