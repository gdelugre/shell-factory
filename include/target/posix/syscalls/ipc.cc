#ifndef POSIX_SYSCALL_IPC_H_
#define POSIX_SYSCALL_IPC_H_

#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>

namespace Syscall {

    SYSTEM_CALL int     pipe(int[2]);
    SYSTEM_CALL int     shmget(key_t, size_t, int);
    SYSTEM_CALL void *  shmat(int, const void *, int);
    SYSTEM_CALL int     shmdt(const void *);
    SYSTEM_CALL int     shmctl(int, int, struct shmid_ds *);
    SYSTEM_CALL int     msgget(key_t, int);
    SYSTEM_CALL int     msgsnd(int, const void *, size_t, int);
    SYSTEM_CALL ssize_t msgrcv(int, void *, size_t, long, int);
    SYSTEM_CALL int     msgctl(int, int, struct msqid_ds *); 
    SYSTEM_CALL int     semget(key_t, int, int);
    SYSTEM_CALL int     semop(int, struct sembuf *, size_t);
    SYSTEM_CALL int     semctl(int, int, int);
    SYSTEM_CALL int     semctl(int, int, int, int);
    SYSTEM_CALL int     semctl(int, int, int, struct semid_ds *);
    SYSTEM_CALL int     semctl(int, int, int, unsigned short *);

    SYSTEM_CALL
    int pipe(int fd[2])
    {
        return DO_SYSCALL(pipe, fd);
    }

    #if SYSCALL_EXISTS(shmget)
    SYSTEM_CALL
    int shmget(key_t key, size_t size, int flag)
    {
        return DO_SYSCALL(shmget, key, size, flag);
    }
    #endif

    #if SYSCALL_EXISTS(shmat)
    SYSTEM_CALL
    void *shmat(int shmid, const void *addr, int flag)
    {
        return (void *) DO_SYSCALL(shmat, shmid, addr, flag);
    }
    #endif

    #if SYSCALL_EXISTS(shmdt)
    SYSTEM_CALL
    int shmdt(const void *addr)
    {
        return DO_SYSCALL(shmdt, addr);
    }
    #endif

    #if SYSCALL_EXISTS(shmctl)
    SYSTEM_CALL
    int shmctl(int shmid, int cmd, struct shmid_ds *buf)
    {
        return DO_SYSCALL(shmctl, shmid, cmd, buf);
    }
    #endif

    #if SYSCALL_EXISTS(msgget)
    SYSTEM_CALL
    int msgget(key_t key, int msgflg)
    {
        return DO_SYSCALL(msgget, key, msgflg);
    }
    #endif

    #if SYSCALL_EXISTS(msgsnd)
    SYSTEM_CALL
    int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
    {
        return DO_SYSCALL(msgsnd, msqid, msgp, msgsz, msgflg);
    }
    #endif

    #if SYSCALL_EXISTS(msgrcv)
    SYSTEM_CALL
    ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)
    {
        return DO_SYSCALL(msgrcv, msqid, msgp, msgsz, msgtyp, msgflg);
    }
    #endif

    #if SYSCALL_EXISTS(msgctl)
    SYSTEM_CALL
    int msgctl(int msqid, int cmd, struct msqid_ds *buf)
    {
        return DO_SYSCALL(msgctl, msqid, cmd, buf);
    }
    #endif

    #if SYSCALL_EXISTS(semget)
    SYSTEM_CALL
    int semget(key_t key, int nsems, int flag)
    {
        return DO_SYSCALL(semget, key, nsems, flag);
    }
    #endif

    #if SYSCALL_EXISTS(semop)
    SYSTEM_CALL
    int semop(int semid, struct sembuf *array, size_t nops)
    {
        return DO_SYSCALL(semop, semid, array, nops);
    }
    #endif

    #if SYSCALL_EXISTS(semctl)
    SYSTEM_CALL
    int semctl(int semid, int semnum, int cmd)
    {
        return DO_SYSCALL(semctl, semid, semnum, cmd);
    }

    SYSTEM_CALL
    int semctl(int semid, int semnum, int cmd, int val)
    {
        return DO_SYSCALL(semctl, semid, semnum, cmd, val);
    }

    SYSTEM_CALL
    int semctl(int semid, int semnum, int cmd, struct semid_ds *buf)
    {
        return DO_SYSCALL(semctl, semid, semnum, cmd, buf);
    }

    SYSTEM_CALL
    int semctl(int semid, int semnum, int cmd, unsigned short *array)
    {
        return DO_SYSCALL(semctl, semid, semnum, cmd, array);
    }
    #endif
}

#endif
