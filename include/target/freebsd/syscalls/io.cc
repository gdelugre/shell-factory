#ifndef SYSCALL_IO_H_
#define SYSCALL_IO_H_

#include <dirent.h>

/*
 * This file defines FreeBSD specific system calls (i.e. not POSIX).
 */
namespace Syscall {

    SYSTEM_CALL int     getdents(unsigned int, struct dirent *, unsigned int);
    SYSTEM_CALL int     getcwd(char *, size_t);

    SYSTEM_CALL
    int getdents(unsigned int fd, struct dirent *dirp, unsigned int count)
    {
        return DO_SYSCALL(getdents, fd, dirp, count);
    }

    SYSTEM_CALL
    int getcwd(char *buf, size_t size)
    {
        return DO_SYSCALL(__getcwd, buf, size);
    }
}

#endif
