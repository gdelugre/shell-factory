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

        constexpr key_t ftok(const char *name)
        {
            return (*name ? (*name) ^ (1000003 * ftok(name +1)) : 0);
        }

        CONSTRUCTOR SharedRegion::SharedRegion(const char *name, void *base, size_t size, int prot, Rights rights)
        {
            key_t key = ftok(name);

            shm_obj = Syscall::shmget(key, size, IPC_CREAT | rights.value);
            void *result = Syscall::shmat(shm_obj, base, shm_prot(prot));
            if ( Target::is_error(result) )
                ptr = nullptr;
            else
                ptr = result;
        }

        DESTRUCTOR SharedRegion::~SharedRegion()
        {
            if ( ptr != nullptr )
                Syscall::shmdt(ptr);
        }
    }
}

#endif
