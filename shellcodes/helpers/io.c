#ifndef _IO_HELPER_H
#define _IO_HELPER_H

#include <factory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>

#include "memory.c"

#define stdin 0
#define stdout 1
#define stderr 2

struct linux_dirent {
    unsigned long   d_ino;
    unsigned long   d_off;
    unsigned short  d_reclen;
    char            d_name[1];
};

#define foreach_dirent(dirents, dirent, off, dsize) \
    for (off = 0, dirent = dirents; \
            dirent && off < dsize; \
            off += dirent->d_reclen, dirent = ((void *) dirent) + dirent->d_reclen)


SYSTEM_CALL
int _open(const char *path, int flags)
{
    return INTERNAL_SYSCALL(openat,, 3, AT_FDCWD, path, flags);
}

SYSTEM_CALL
int _create(const char *path, int flags, mode_t mode)
{
    return INTERNAL_SYSCALL(openat,, 4, AT_FDCWD, path, flags | O_CREAT, mode);
}

SYSTEM_CALL
int _dup2(int filedes, int filedes2)
{   
    return INTERNAL_SYSCALL(dup2,, 2, filedes, filedes2);
}

SYSTEM_CALL
off_t _lseek(int fd, off_t offset, int whence)
{
    return INTERNAL_SYSCALL(lseek,, 3, fd, offset, whence);
}

SYSTEM_CALL
ssize_t _read(int fd, void *buf, size_t count)
{
    return INTERNAL_SYSCALL(read,, 3, fd, buf, count);
}

SYSTEM_CALL
ssize_t _write(int fd, const void *buf, size_t count)
{
    return INTERNAL_SYSCALL(write,, 3, fd, buf, count);
}

SYSTEM_CALL
int _fsync(int fildes)
{
    return INTERNAL_SYSCALL(fsync,, 1, fildes);
}

SYSTEM_CALL
int _poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    return INTERNAL_SYSCALL(poll,, 3, fds, nfds, timeout);
}

SYSTEM_CALL
int _select(int nfds, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds, struct timeval *timeout)
{
    return INTERNAL_SYSCALL(select,, 5, nfds, read_fds, write_fds, except_fds, timeout);
}

SYSTEM_CALL
int _getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count)
{
    return INTERNAL_SYSCALL(getdents,, 3, fd, dirp, count);
}

SYSTEM_CALL
int _close(int fd)
{
    return INTERNAL_SYSCALL(close,, 1, fd);
}

int read_directory(const char *pathname, struct linux_dirent **p_dirents, size_t *dsize)
{
    int                 ret;
    int                 dirfd = _open(pathname, O_DIRECTORY | O_RDONLY);
    size_t              buffer_sz = PAGE_SIZE;
    size_t              read_size = 0;
    void                *buffer = _malloc(buffer_sz);
    struct linux_dirent *dirents = buffer;

    if ( dirfd < 0 )
        return dirfd;

    for (;;)
    {
        ret = _getdents(dirfd, dirents, buffer_sz);
        if ( ret == 0 )
            break;
     
        if ( ret < 0 )
        {
            _free(buffer);
            _close(dirfd);
            return ret;
        }

        read_size += ret;
        buffer_sz *= 2;
        buffer = _realloc(buffer, buffer_sz);
        dirents = buffer + read_size;
    }

    *p_dirents = (struct linux_dirent *) buffer;
    *dsize = read_size;

    _close(dirfd);
    return 0;
}

FUNCTION
void drop_file(const char *filename, mode_t mode, void *data, size_t size)
{
    int fd = _create(filename, O_TRUNC | O_WRONLY, mode);
    _write(fd, data, size);
    _close(fd);
}

#endif
