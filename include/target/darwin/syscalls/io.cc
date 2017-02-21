#ifndef SYSCALL_IO_H_
#define SYSCALL_IO_H_

#include <dirent.h>

/*
 * This file defines Darwin specific system calls (i.e. not POSIX).
 */
namespace Syscall {

    SYSTEM_CALL int     getdirentries(unsigned int, struct dirent *, unsigned int, long *);

    SYSTEM_CALL
    int getdirentries(unsigned int fd, struct dirent *dirp, unsigned int nbytes, long *basep)
    {
        return DO_SYSCALL(getdirentries, fd, dirp, nbytes, basep);
    }
}

#endif
