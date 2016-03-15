#ifndef SYSCALL_SECURITY_H_
#define SYSCALL_SECURITY_H_

/*
 * This file defines Linux specific system calls (i.e. not POSIX).
 */
namespace Syscall {

    SYSTEM_CALL int     getrandom(void *, size_t, unsigned int);
    SYSTEM_CALL int     setresuid(uid_t, uid_t, uid_t);
    SYSTEM_CALL int     setresgid(gid_t, gid_t, gid_t);

    #if SYSCALL_EXISTS(getrandom)
    SYSTEM_CALL
    int getrandom(void *buf, size_t buflen, unsigned int flags)
    {
        return DO_SYSCALL(getrandom, buf, buflen, flags);
    }
    #endif

    SYSTEM_CALL
    int setresuid(uid_t ruid, uid_t euid, uid_t suid)
    {
        return DO_SYSCALL(setresuid, ruid, euid, suid);
    }

    SYSTEM_CALL
    int setresgid(gid_t rgid, gid_t egid, gid_t sgid)
    {
        return DO_SYSCALL(setresgid, rgid, egid, sgid);
    }

    // Linux does not have system calls for seteuid / setegid.
    // Define the POSIX wrappers.
    SYSTEM_CALL
    int seteuid(uid_t euid)
    {
        return setresuid(-1, euid, -1);
    }

    SYSTEM_CALL
    int setegid(gid_t egid)
    {
        return setresgid(-1, egid, -1);
    }
}

#endif
