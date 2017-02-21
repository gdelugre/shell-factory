#ifndef SYSCALL_IPC_H_
#define SYSCALL_IPC_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


/*
 * System calls defined in this file.
 */
namespace Syscall {

    SYSTEM_CALL int pipe(int[2]);
    SYSTEM_CALL int shmget(key_t, size_t, int);
    SYSTEM_CALL void *shmat(int, const void *, int);
    SYSTEM_CALL int shmdt(const void *);
    SYSTEM_CALL int shmctl(int, int, struct shmid_ds *);

    SYSTEM_CALL
    int pipe(int pipefd[2])
    {
        return DO_SYSCALL(pipe, pipefd);
    }

    SYSTEM_CALL
    int shmget(key_t key, size_t size, int shmflg)
    {
        #if SYSCALL_EXISTS(shmget)
        return DO_SYSCALL(shmget, key, size, shmflg);
        #else
        return DO_SYSCALL(shmsys, 3, key, size, shmflg);
        #endif
    }

    SYSTEM_CALL
    void *shmat(int shmid, const void *shmaddr, int shmflg)
    {
        #if SYSCALL_EXISTS(shmat)
        return (void *) DO_SYSCALL(shmat, shmid, shmaddr, shmflg);
        #else
        return (void *) DO_SYSCALL(shmsys, 0, shmid, shmaddr, shmflg);
        #endif
    }

    SYSTEM_CALL
    int shmdt(const void *shmaddr)
    {
        #if SYSCALL_EXISTS(shmdt)
        return DO_SYSCALL(shmdt, shmaddr);
        #else
        return DO_SYSCALL(shmsys, 2, shmaddr);
        #endif
    }

    SYSTEM_CALL
    int shmctl(int shmid, int cmd, struct shmid_ds *buf)
    {
        #if SYSCALL_EXISTS(shmctl)
        return DO_SYSCALL(shmctl, shmid, cmd, buf);
        #else
        return DO_SYSCALL(shmsys, 4, shmid, cmd, buf);
        #endif
    }
}

#endif
