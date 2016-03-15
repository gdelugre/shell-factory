#ifndef SYSCALL_IO_H_
#define SYSCALL_IO_H_

struct linux_dirent {
    unsigned long   d_ino;
    unsigned long   d_off;
    unsigned short  d_reclen;
    char            d_name[1];
};

/*
 * This file defines Linux specific system calls (i.e. not POSIX).
 */
namespace Syscall {

    #if SYSCALL_EXISTS(dup3)
    SYSTEM_CALL int     dup3(int, int, int);
    #endif
    SYSTEM_CALL int     getdents(unsigned int, struct linux_dirent *, unsigned int);
    SYSTEM_CALL int     getcwd(char *, size_t);

    #if SYSCALL_EXISTS(dup3)
    SYSTEM_CALL
    int dup3(int oldfd, int newfd, int flags)
    {
        return DO_SYSCALL(dup3, oldfd, newfd, flags);
    }
    #endif

    SYSTEM_CALL
    int getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count)
    {
        return DO_SYSCALL(getdents, fd, dirp, count);
    }

    SYSTEM_CALL
    int getcwd(char *buf, size_t size)
    {
        return DO_SYSCALL(getcwd, buf, size);
    }
}

#endif
