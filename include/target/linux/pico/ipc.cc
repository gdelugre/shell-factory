#ifndef PICOLIB_IPC_IMPL_H_
#define PICOLIB_IPC_IMPL_H_

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

        FUNCTION
        constexpr int shm_prot(int pico_prot)
        {
            return (pico_prot & Memory::WRITE ? 0 : SHM_RDONLY) |
                   (pico_prot & Memory::EXEC ? SHM_EXEC : 0);
        }

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
            void *result = Syscall::shmat(shm_obj, base, shm_prot(prot) | SHM_RND);
            if ( Target::is_error(result) )
                ptr = nullptr;
            else
                ptr = result;
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
