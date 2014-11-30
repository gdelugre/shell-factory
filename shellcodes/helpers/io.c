#ifndef _IO_HELPER_H
#define _IO_HELPER_H

#include <factory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#define stdin 0
#define stdout 1
#define stderr 2

static inline int
_open(const char *path, int flags)
{
    return INTERNAL_SYSCALL(openat,, 3, AT_FDCWD, path, flags);
}

static inline int 
_create(const char *path, int flags, mode_t mode)
{
    return INTERNAL_SYSCALL(openat,, 4, AT_FDCWD, path, flags | O_CREAT, mode);
}

static inline int
_dup2(int filedes, int filedes2)
{   
    return INTERNAL_SYSCALL(dup2,, 2, filedes, filedes2);
}

static inline off_t
_lseek(int fd, off_t offset, int whence)
{
    return INTERNAL_SYSCALL(lseek,, 3, fd, offset, whence);
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
_poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    return INTERNAL_SYSCALL(poll,, 3, fds, nfds, timeout);
}

static inline int
_select(int nfds, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds, struct timeval *timeout)
{
    return INTERNAL_SYSCALL(select,, 5, nfds, read_fds, write_fds, except_fds, timeout);
}

static inline int
_close(int fd)
{
    return INTERNAL_SYSCALL(close,, 1, fd);
}

static inline void 
drop_file(const char *filename, mode_t mode, void *data, size_t size)
{
    int fd = _create(filename, O_TRUNC | O_WRONLY, mode);
    _write(fd, data, size);
    _close(fd);
}

#endif
