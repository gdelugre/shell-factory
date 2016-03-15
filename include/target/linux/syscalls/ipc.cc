#ifndef SYSCALL_IPC_H_
#define SYSCALL_IPC_H_

#include <sys/ipc.h>
#include <sys/time.h>

/*
 * This file defines Linux specific system calls (i.e. not POSIX).
 */
namespace Syscall {

    SYSTEM_CALL int futex(int *, int, int, const struct timespec *, int *, int);
    #if SYSCALL_EXISTS(ipc)
    SYSTEM_CALL int ipc(uint32_t, int, int, uint32_t, void *, uint32_t);
    SYSTEM_CALL int ipc(uint32_t, int, int, uint32_t, void *);
    SYSTEM_CALL int ipc(uint32_t, int, int, uint32_t);
    SYSTEM_CALL int ipc(uint32_t, int, int);
    #endif

    SYSTEM_CALL
    int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3)
    {
        return DO_SYSCALL(futex, uaddr, futex_op, val, timeout, uaddr2, val3);
    }

    //
    // Common ipc entry-point for some architectures.
    //
    #if SYSCALL_EXISTS(ipc)

        // Call types.
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

        SYSTEM_CALL
        int ipc(uint32_t call, int first, int second, uint32_t third, void *ptr, uint32_t fifth)
        {
            return DO_SYSCALL(ipc, call, first, second, third, ptr, fifth);
        }

        SYSTEM_CALL
        int ipc(uint32_t call, int first, int second, uint32_t third, void *ptr)
        {
            return DO_SYSCALL(ipc, call, first, second, third, ptr);
        }

        SYSTEM_CALL
        int ipc(uint32_t call, int first, int second, uint32_t third)
        {
            return DO_SYSCALL(ipc, call, first, second, third);
        }

        SYSTEM_CALL
        int ipc(uint32_t call, int first, int second)
        {
            return DO_SYSCALL(ipc, call, first, second);
        }

        #if !SYSCALL_EXISTS(shmget)
        SYSTEM_CALL
        int shmget(key_t key, size_t size, int shmflg)
        {
            return ipc(SHMGET, key, size, shmflg);
        }
        #endif

        #if !SYSCALL_EXISTS(shmat)
        SYSTEM_CALL
        void *shmat(int shmid, const void *shmaddr, int shmflg)
        {
            void *ptr = nullptr;
            int res = ipc(SHMAT, shmid, shmflg, (uint32_t) &ptr, (void *) shmaddr);
            if ( Target::is_error(res) )
                return (void *) res;
            else
                return ptr;
        }
        #endif

        #if !SYSCALL_EXISTS(shmdt)
        SYSTEM_CALL
        int shmdt(const void *shmaddr)
        {
            return ipc(SHMDT, 0, 0, 0, (void *) shmaddr);
        }
        #endif

        #if !SYSCALL_EXISTS(shmctl)
        SYSTEM_CALL
        int shmctl(int shmid, int cmd, struct shmid_ds *buf)
        {
            return ipc(SHMCTL, shmid, cmd, 0, buf);
        }
        #endif

        #if !SYSCALL_EXISTS(msgget)
        SYSTEM_CALL
        int msgget(key_t key, int msgflg)
        {
            return ipc(MSGGET, key, msgflg);
        }
        #endif

        #if !SYSCALL_EXISTS(msgsnd)
        SYSTEM_CALL
        int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
        {
            return ipc(MSGSND, msqid, msgsz, msgflg, (void *) msgp);
        }
        #endif

        #if !SYSCALL_EXISTS(msgrcv)
        SYSTEM_CALL
        ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)
        {
            struct {
                struct msgbuf *msgp;
                long msgtyp;
            } kludge;

            kludge.msgp = static_cast<msgbuf *>(msgp);
            kludge.msgtyp = msgtyp;

            return ipc(MSGRCV, msqid, msgsz, msgflg, &kludge);
        }
        #endif

        #if !SYSCALL_EXISTS(msgctl)
        SYSTEM_CALL
        int msgctl(int msqid, int cmd, struct msqid_ds *buf)
        {
            return ipc(MSGCTL, msqid, cmd, 0, buf);
        } 
        #endif

        #if !SYSCALL_EXISTS(semget)
        SYSTEM_CALL
        int semget(key_t key, int nsems, int semflg)
        {
            return ipc(SEMGET, key, nsems, semflg);
        }
        #endif

        #if !SYSCALL_EXISTS(semop)
        SYSTEM_CALL
        int semop(int semid, struct sembuf *array, size_t nops)
        {
            return ipc(SEMOP, semid, nops, 0, array);
        }
        #endif

        #if !SYSCALL_EXISTS(semctl)
        SYSTEM_CALL
        int semctl(int semid, int semnum, int cmd)
        {
            uint32_t dummy = 0;
            return ipc(SEMCTL, semid, semnum, cmd, &dummy);
        }

        SYSTEM_CALL
        int semctl(int semid, int semnum, int cmd, int val)
        {
            return ipc(SEMCTL, semid, semnum, cmd, &val);
        }

        SYSTEM_CALL
        int semctl(int semid, int semnum, int cmd, struct semid_ds *buf)
        {
            return ipc(SEMCTL, semid, semnum, cmd, &buf);
        }

        SYSTEM_CALL
        int semctl(int semid, int semnum, int cmd, unsigned short *array)
        {
            return ipc(SEMCTL, semid, semnum, cmd, &array);
        }
        #endif

    #endif // SYSCALL_EXISTS(ipc)

}

#endif
