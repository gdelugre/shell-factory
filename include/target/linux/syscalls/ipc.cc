#ifndef SYSCALL_IPC_H_
#define SYSCALL_IPC_H_

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>

#if SYSCALL_EXISTS(ipc)
#define SEMOP        1
#define SEMGET       2
#define SEMCTL       3
#define SEMTIMEDOP   4
#define MSGSND      11
#define MSGRCV      12
#define MSGGET      13
#define MSGCTL      14
#define SHMAT       21
#define SHMDT       22
#define SHMGET      23
#define SHMCTL      24
#endif

/*
 * System calls defined in this file.
 */
namespace Syscall {

    SYSTEM_CALL int pipe(int pipefd[2]);
    SYSTEM_CALL int shmget(key_t, size_t, int);
    SYSTEM_CALL void *shmat(int, const void *, int);
    SYSTEM_CALL int shmdt(const void *);
    SYSTEM_CALL int shmctl(int, int, struct shmid_ds *);
    SYSTEM_CALL int futex(int *, int, int, const struct timespec *, int *, int);

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
        return DO_SYSCALL(ipc, SHMGET, key, size, shmflg);
        #endif
    }

    SYSTEM_CALL
    void *shmat(int shmid, const void *shmaddr, int shmflg)
    {
        #if SYSCALL_EXISTS(shmat)
        return (void *) DO_SYSCALL(shmat, shmid, shmaddr, shmflg);
        #else
        void *ptr = nullptr;
        int res = DO_SYSCALL(ipc, SHMAT, shmid, shmflg, &ptr, shmaddr);
        if ( Target::is_error(res) )
            return (void *) res;
        else
            return ptr;
        #endif
    }

    SYSTEM_CALL
    int shmdt(const void *shmaddr)
    {
        #if SYSCALL_EXISTS(shmdt)
        return DO_SYSCALL(shmdt, shmaddr);
        #else
        return DO_SYSCALL(ipc, SHMDT, 0, 0, 0, shmaddr);
        #endif
    }

    SYSTEM_CALL
    int shmctl(int shmid, int cmd, struct shmid_ds *buf)
    {
        #if SYSCALL_EXISTS(shmctl)
        return DO_SYSCALL(shmctl, shmid, cmd, buf);
        #else
        return DO_SYSCALL(ipc, SHMCTL, shmid, cmd, 0, buf);
        #endif
    }

    SYSTEM_CALL
    int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3)
    {
        return DO_SYSCALL(futex, uaddr, futex_op, val, timeout, uaddr2, val3);
    }
}

#endif
