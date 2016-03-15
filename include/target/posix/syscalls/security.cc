#ifndef POSIX_SYSCALL_SECURITY_H_
#define POSIX_SYSCALL_SECURITY_H_

namespace Syscall {

    SYSTEM_CALL uid_t   getuid();
    SYSTEM_CALL uid_t   geteuid();
    SYSTEM_CALL gid_t   getgid();
    SYSTEM_CALL gid_t   getegid();
    SYSTEM_CALL int     setuid(uid_t);
    SYSTEM_CALL int     seteuid(uid_t);
    SYSTEM_CALL int     setgid(gid_t);
    SYSTEM_CALL int     setegid(gid_t);
    SYSTEM_CALL int     setreuid(uid_t, uid_t);
    SYSTEM_CALL int     setregid(gid_t, gid_t);
    SYSTEM_CALL int     getgroups(int, gid_t[]);
    SYSTEM_CALL int     setgroups(int, const gid_t[]);

    SYSTEM_CALL
    uid_t getuid()
    {
        return DO_SYSCALL(getuid);
    }

    SYSTEM_CALL
    uid_t geteuid()
    {
        return DO_SYSCALL(geteuid);
    }

    SYSTEM_CALL
    gid_t getgid()
    {
        return DO_SYSCALL(getgid);
    }

    SYSTEM_CALL
    gid_t getegid()
    {
        return DO_SYSCALL(getegid);
    }

    SYSTEM_CALL
    int setuid(uid_t uid)
    {
        return DO_SYSCALL(setuid, uid);
    }

    #if SYSCALL_EXISTS(seteuid)
    SYSTEM_CALL
    int seteuid(uid_t euid)
    {
        return DO_SYSCALL(seteuid, euid);
    }
    #endif

    SYSTEM_CALL
    int setgid(gid_t gid)
    {
        return DO_SYSCALL(setgid, gid);
    }

    #if SYSCALL_EXISTS(setegid)
    SYSTEM_CALL
    int setegid(gid_t egid)
    {
        return DO_SYSCALL(setegid, egid);
    }
    #endif

    SYSTEM_CALL
    int setreuid(uid_t ruid, uid_t euid)
    {
        return DO_SYSCALL(setreuid, ruid, euid);
    }

    SYSTEM_CALL
    int setregid(gid_t rgid, gid_t egid)
    {
        return DO_SYSCALL(setregid, rgid, egid);
    }

    SYSTEM_CALL
    int getgroups(int gidsetsize, gid_t grouplist[])
    {
        return DO_SYSCALL(getgroups, gidsetsize, grouplist);
    }

    SYSTEM_CALL int setgroups(int gidsetsize, const gid_t grouplist[])
    {
        return DO_SYSCALL(setgroups, gidsetsize, grouplist);
    }
}

#endif
