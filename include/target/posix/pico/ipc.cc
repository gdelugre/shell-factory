#ifndef POSIX_PICO_IPC_H_
#define POSIX_PICO_IPC_H_

#include <sys/ipc.h>
#include <sys/shm.h>

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
        constexpr key_t ftok<size_t(-1)>(const char *)
        {
            return 0;
        }

        FUNCTION
        constexpr int shm_flags(int pico_prot)
        {
            return (pico_prot & Memory::WRITE) ? 0 : SHM_RDONLY;
        }

        template <int N>
        CONSTRUCTOR SharedRegion::SharedRegion(const char (&name)[N],
                                               void *base, size_t size,
                                               int prot, Rights rights)
        {
            key_t key = ftok<N>(name); // Compile-time hash of input name.

            shm_obj = Syscall::shmget(key, size, IPC_CREAT | rights.value);
            void *result = Syscall::shmat(shm_obj, base, shm_flags(prot) | SHM_RND);
            if ( Target::is_error(result) ) {
                ptr = nullptr;
                return;
            }

            ptr = result;
            region_size = Memory::round_up_page_size(size);

            if ( prot & Memory::EXEC )
                set_protection(prot);
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
