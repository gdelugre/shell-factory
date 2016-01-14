#ifndef SYSCALL_SECURITY_H_
#define SYSCALL_SECURITY_H_

#include <unistd.h>

/*
 * System calls defined in this file.
 */
namespace Syscall {

    SYSTEM_CALL uid_t   getuid();
    SYSTEM_CALL uid_t   geteuid();
    SYSTEM_CALL gid_t   getgid();
    SYSTEM_CALL gid_t   getegid();
    SYSTEM_CALL int     getrandom(void *, size_t, unsigned int);

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

    #if SYSCALL_EXISTS(getrandom)
    SYSTEM_CALL
    int getrandom(void *buf, size_t buflen, unsigned int flags)
    {
        return DO_SYSCALL(getrandom, buf, buflen, flags);
    }
    #endif
}

#endif
