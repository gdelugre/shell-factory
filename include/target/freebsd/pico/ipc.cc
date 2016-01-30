#ifndef PICOLIB_IPC_IMPL_H_
#define PICOLIB_IPC_IMPL_H_

namespace Pico {

    METHOD
    BiStream<Pipe> Pipe::create() {
        int fds[2];

        Syscall::pipe(fds);
        return BiStream<Pipe>(Pipe(fds[0]), Pipe(fds[1]));
    } 

    namespace Memory {

        template <size_t N>
        METHOD
        constexpr key_t ftok(const char *name)
        {
            return name[N] ^ (1000003 * ftok<N-1>(name));
        }

        template <>
        METHOD
        constexpr key_t ftok<size_t(-1)>(const char *name)
        {
            return 0;
        }

        template <int N>
        CONSTRUCTOR SharedRegion::SharedRegion(const char (&name)[N],
                                               void *base, size_t size,
                                               int prot, Rights rights)
        {
            key_t key = ftok<N>(name); // Compile-time hash of input name.

            shm_obj = Syscall::shmget(key, size, IPC_CREAT | rights.value);
            void *result = Syscall::shmat(shm_obj, base, SHM_RND);
            if ( Target::is_error(result) ) {
                ptr = nullptr;
                return;
            }

            ptr = result;
            region_size = Memory::round_up_page_size(size);

            Syscall::mprotect(ptr, region_size, mmap_prot(prot));
        }

        DESTRUCTOR SharedRegion::~SharedRegion()
        {
            if ( ptr != nullptr ) {
                Syscall::shmdt(ptr);
                Syscall::shmctl(shm_obj, IPC_RMID, nullptr);
            }
        }
    }
}

#endif
