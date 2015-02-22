#ifndef PICOLIB_IO_H_
#define PICOLIB_IO_H_

#include "memory.cc"

#define foreach_dirent(dirents, dirent, off, dsize) \
    for (off = 0, dirent = dirents; \
            dirent && off < dsize; \
            off += dirent->d_reclen, dirent = static_cast<struct linux_dirent *>((void *)(((char *) dirent) + dirent->d_reclen)))

#define dirent_name(dirent) dirent->d_name

FUNCTION
int read_directory(const char *pathname, struct linux_dirent **p_dirents, size_t *dsize)
{
    int                 ret;
    int                 dirfd = Syscall::open(pathname, O_DIRECTORY | O_RDONLY);
    size_t              buffer_sz = PAGE_SIZE;
    size_t              read_size = 0;
    void                *buffer = _malloc(buffer_sz);
    struct linux_dirent *dirents = static_cast<linux_dirent *>(buffer);

    if ( dirfd < 0 )
        return dirfd;

    while ( true )
    {
        ret = Syscall::getdents(dirfd, dirents, buffer_sz);
        if ( ret == 0 )
            break;
     
        if ( ret < 0 )
        {
            _free(buffer);
            Syscall::close(dirfd);
            return ret;
        }

        read_size += ret;
        buffer_sz *= 2;
        buffer = _realloc(buffer, buffer_sz);
        dirents = static_cast<struct linux_dirent *>((void *)((char *) buffer + read_size));
    }

    *p_dirents = (struct linux_dirent *) buffer;
    *dsize = read_size;

    Syscall::close(dirfd);
    return 0;
}

FUNCTION
void drop_file(const char *filename, mode_t mode, void *data, size_t size)
{
    int fd = Syscall::create(filename, O_TRUNC | O_WRONLY, mode);
    Syscall::write(fd, data, size);
    Syscall::close(fd);
}

#endif
