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
}

#endif
