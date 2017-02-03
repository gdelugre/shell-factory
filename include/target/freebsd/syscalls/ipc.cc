#ifndef SYSCALL_IPC_H_
#define SYSCALL_IPC_H_

#include <sys/types.h>


/*
 * This file defines FreeBSD specific system calls (i.e. not POSIX).
 */
namespace Syscall {

    SYSTEM_CALL int _umtx_op(void *, int ,u_long, void *, void *);
    SYSTEM_CALL int pipe(int[2]);
    SYSTEM_CALL int pipe2(int[2], int);
    #if SYSCALL_EXISTS(shmsys)
    SYSTEM_CALL int shmsys(int, int, int, int);
    SYSTEM_CALL int shmsys(int, int, const void *, int);
    SYSTEM_CALL int shmsys(int, const void *);
    SYSTEM_CALL int shmsys(int, int, int, struct shmid_ds *);
    #endif

    SYSTEM_CALL
    int _umtx_op(void *obj, int op, u_long val, void *uaddr, void *uaddr2)
    {
        return DO_SYSCALL(_umtx_op, obj, op, val, uaddr, uaddr2);
    }

    #if SYSCALL_EXISTS(pipe2)
    SYSTEM_CALL
    int pipe2(int filedes[2], int flags)
    {
        return DO_SYSCALL(pipe2, filedes, flags);
    }
    #endif

    //
    // Fallback version if the POSIX version is not present (since FreeBSD 11).
    //
    #if !SYSCALL_EXISTS(pipe)
    SYSTEM_CALL
    int pipe(int filedes[2])
    {
        return pipe2(filedes, 0);
    }
    #endif

    //
    // Common entry point for shared memory operations.
    //
    #if SYSCALL_EXISTS(shmsys)

        SYSTEM_CALL
        int shmsys(int which, int a2, int a3, int a4)
        {
            return DO_SYSCALL(shmsys, which, a2, a3, a4);
        }

        SYSTEM_CALL
        int shmsys(int which, int a2, const void *a3, int a4)
        {
            return DO_SYSCALL(shmsys, which, a2, a3, a4);
        }

        SYSTEM_CALL
        int shmsys(int which, const void *a2)
        {
            return DO_SYSCALL(shmsys, which, a2);
        }

        SYSTEM_CALL
        int shmsys(int which, int a2, int a3, struct shmid_ds *buf)
        {
            return DO_SYSCALL(shmsys, which, a2, a3, buf);
        }

        #if !SYSCALL_EXISTS(shmget)
        SYSTEM_CALL
        int shmget(key_t key, size_t size, int shmflg)
        {
            return shmsys(3, key, size, shmflg);
        }
        #endif

        #if !SYSCALL_EXISTS(shmat)
        SYSTEM_CALL
        void *shmat(int shmid, const void *shmaddr, int shmflg)
        {
            return (void *) shmsys(0, shmid, smdaddr, shmflg);
        }
        #endif

        #if !SYSCALL_EXISTS(shmdt)
        SYSTEM_CALL
        int shmdt(const void *shmaddr)
        {
            return shmsys(2, shmaddr);
        }
        #endif

        #if !SYSCALL_EXISTS(shmctl)
        SYSTEM_CALL
        int shmctl(int shmid, int cmd, struct shmid_ds *buf)
        {
            return shmsys(4, shmid ,cmd, buf);
        }
        #endif

    #endif // SYSCALL_EXISTS(shmsys)
}

#endif
