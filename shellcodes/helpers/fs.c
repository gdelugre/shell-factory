#ifndef _FS_HELPER_H
#define _FS_HELPER_H

#include <factory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static inline int
_open(const char *path, int flags)
{
    return INTERNAL_SYSCALL(openat,, 3, AT_FDCWD, path, flags);
}

static inline int
_dup2(int filedes, int filedes2)
{
    return INTERNAL_SYSCALL(dup2,, 2, filedes, filedes2);
}

static inline ssize_t
_read(int fd, void *buf, size_t count)
{
    return INTERNAL_SYSCALL(read,, 3, fd, buf, count);
}

static inline ssize_t
_write(int fd, const void *buf, size_t count)
{
    return INTERNAL_SYSCALL(write,, 3, fd, buf, count);
}

static inline int
_close(int fd)
{
    return INTERNAL_SYSCALL(close,, 1, fd);
}

#endif
