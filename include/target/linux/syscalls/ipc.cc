#ifndef SYSCALL_IPC_H_
#define SYSCALL_IPC_H_

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/*
 * System calls defined in this file.
 */
namespace Syscall {

    SYSTEM_CALL int pipe(int pipefd[2]);
    SYSTEM_CALL int shmget(key_t, size_t, int);
    SYSTEM_CALL int shmat(int, const void *, int);
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
        return DO_SYSCALL(shmget, key, size, shmflg);
    }

    SYSTEM_CALL
    int shmat(int shmid, const void *shmaddr, int shmflg)
    {
        return DO_SYSCALL(shmat, shmid, shmaddr, shmflg);
    }

    SYSTEM_CALL
    int shmdt(const void *shmaddr)
    {
        return DO_SYSCALL(shmdt, shmaddr);
    }

    SYSTEM_CALL
    int shmctl(int shmid, int cmd, struct shmid_ds *buf)
    {
        return DO_SYSCALL(shmctl, shmid, cmd, buf);
    }
}

#endif
