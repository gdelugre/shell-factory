#ifndef POSIX_SYSCALL_IO_H_
#define POSIX_SYSCALL_IO_H_

#include <fcntl.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/stat.h>

namespace Syscall {

    SYSTEM_CALL int     open(const char *, int);
    SYSTEM_CALL int     open(const char *, int, mode_t);
    SYSTEM_CALL int     close(int);
    SYSTEM_CALL int     creat(const char *, int, mode_t);
    SYSTEM_CALL int     access(const char *, int);
    SYSTEM_CALL int     dup(int);
    SYSTEM_CALL int     dup2(int, int);
    SYSTEM_CALL off_t   lseek(int, off_t, int);
    SYSTEM_CALL ssize_t read(int, void *, size_t);
    SYSTEM_CALL ssize_t write(int, const void *, size_t);
    SYSTEM_CALL int     fsync(int);
    SYSTEM_CALL int     poll(struct pollfd *, nfds_t, int);
    SYSTEM_CALL int     select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
    SYSTEM_CALL ssize_t readlink(const char *, char *, size_t);
    SYSTEM_CALL int     stat(const char *, struct stat *);
    SYSTEM_CALL int     fstat(int, struct stat *);
    SYSTEM_CALL int     lstat(const char *, struct stat *);
    SYSTEM_CALL int     rename(const char *, const char *);
    SYSTEM_CALL int     symlink(const char *, const char *);
    SYSTEM_CALL int     link(const char *, const char *);
    SYSTEM_CALL int     unlink(const char *);
    SYSTEM_CALL int     mknod(const char *, mode_t, dev_t);
    SYSTEM_CALL int     mkdir(const char *, mode_t);
    SYSTEM_CALL int     rmdir(const char *);
    SYSTEM_CALL int     chdir(const char *);
    SYSTEM_CALL int     fchdir(int);
    SYSTEM_CALL int     chroot(const char *);
    SYSTEM_CALL int     chown(const char *, uid_t, gid_t);
    SYSTEM_CALL int     fchown(int, uid_t, gid_t);
    SYSTEM_CALL int     lchown(const char *, uid_t, gid_t);
    SYSTEM_CALL int     chmod(const char *, mode_t);
    SYSTEM_CALL int     fchmod(int, mode_t);
    SYSTEM_CALL int     fcntl(int, int);
    SYSTEM_CALL int     fcntl(int, int, void *);
    SYSTEM_CALL int     ioctl(int, unsigned long);
    SYSTEM_CALL int     ioctl(int, unsigned long, void *);
    SYSTEM_CALL int     ioctl(int, unsigned long, long);

    // These may not be implemented everywhere.
    #if SYSCALL_EXISTS(mkfifo)
    SYSTEM_CALL int     mkfifo(const char *, mode_t);
    #endif

    #if SYSCALL_EXISTS(openat)
    SYSTEM_CALL int     openat(int, const char *, int);
    SYSTEM_CALL int     openat(int, const char *, int, mode_t);
    #endif
    #if SYSCALL_EXISTS(faccessat)
    SYSTEM_CALL int     faccessat(int, const char *, int, int);
    #endif
    #if SYSCALL_EXISTS(readlinkat)
    SYSTEM_CALL ssize_t readlinkat(int, const char *, char *, size_t);
    #endif
    #if SYSCALL_EXISTS(fstatat) || SYSCALL_EXISTS(newfstatat)
    SYSTEM_CALL int     fstatat(int, const char *, struct stat *, int);
    #endif
    #if SYSCALL_EXISTS(renameat)
    SYSTEM_CALL int     renameat(int, const char *, int, const char *);
    #endif
    #if SYSCALL_EXISTS(symlinkat)
    SYSTEM_CALL int     symlinkat(const char *, int, const char *);
    #endif
    #if SYSCALL_EXISTS(linkat)
    SYSTEM_CALL int     linkat(int, const char *, int, const char *, int);
    #endif
    #if SYSCALL_EXISTS(unlinkat)
    SYSTEM_CALL int     unlinkat(int, const char *, int);
    #endif
    #if SYSCALL_EXISTS(mknodat)
    SYSTEM_CALL int     mknodat(int, const char *, mode_t, dev_t);
    #endif
    #if SYSCALL_EXISTS(mkfifoat)
    SYSTEM_CALL int     mkfifoat(int, const char *, mode_t);
    #endif
    #if SYSCALL_EXISTS(mkdirat)
    SYSTEM_CALL int     mkdirat(int, const char *, mode_t);
    #endif
    #if SYSCALL_EXISTS(fchownat)
    SYSTEM_CALL int     fchownat(int, const char *, uid_t, gid_t, int);
    #endif
    #if SYSCALL_EXISTS(fchmodat)
    SYSTEM_CALL int     fchmodat(int, const char *, mode_t, int);
    #endif

    SYSTEM_CALL
    int open(const char *pathname, int flags)
    {
        #if SYSCALL_EXISTS(open)
        return DO_SYSCALL(open, pathname, flags);
        #else
        return openat(AT_FDCWD, pathname, flags);
        #endif
    }

    SYSTEM_CALL
    int open(const char *pathname, int flags, mode_t mode)
    {
        #if SYSCALL_EXISTS(open)
        return DO_SYSCALL(open, pathname, flags, mode);
        #else
        return openat(AT_FDCWD, pathname, flags, mode);
        #endif
    }

    SYSTEM_CALL
    int close(int fd)
    {
        return DO_SYSCALL(close, fd);
    }

    SYSTEM_CALL
    int creat(const char *pathname, mode_t mode)
    {
        #if SYSCALL_EXISTS(creat)
        return DO_SYSCALL(creat, pathname, mode);
        #else
        return open(pathname, O_WRONLY | O_CREAT | O_TRUNC, mode);
        #endif
    }

    SYSTEM_CALL
    int access(const char *pathname, int mode)
    {
        #if SYSCALL_EXISTS(access)
        return DO_SYSCALL(access, pathname, mode);
        #else
        return faccessat(AT_FDCWD, pathname, mode, 0);
        #endif
    }

    SYSTEM_CALL
    int dup(int oldfd)
    {
        return DO_SYSCALL(dup, oldfd);
    }

    SYSTEM_CALL
    int dup2(int filedes, int filedes2)
    {
        return DO_SYSCALL(dup2, filedes, filedes2);
    }

    SYSTEM_CALL
    off_t lseek(int fd, off_t offset, int whence)
    {
        return DO_SYSCALL(lseek, fd, offset, whence);
    }

    SYSTEM_CALL
    ssize_t read(int fd, void *buf, size_t count)
    {
        return DO_SYSCALL(read, fd, buf, count);
    }

    SYSTEM_CALL
    ssize_t write(int fd, const void *buf, size_t count)
    {
        return DO_SYSCALL(write, fd, buf, count);
    }

    SYSTEM_CALL
    int fsync(int fildes)
    {
        return DO_SYSCALL(fsync, fildes);
    }

    SYSTEM_CALL
    int poll(struct pollfd *fds, nfds_t nfds, int timeout)
    {
        return DO_SYSCALL(poll, fds, nfds, timeout);
    }

    SYSTEM_CALL
    int select(int nfds, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds, struct timeval *timeout)
    {
        #if SYSCALL_EXISTS(select)
        return DO_SYSCALL(select, nfds, read_fds, write_fds, except_fds, timeout);
        #else
        //
        // Some architectures on Linux have renamed select as _newselect.
        // The system call appears to remain POSIX compliant though.
        //
        return DO_SYSCALL(_newselect, nfds, read_fds, write_fds, except_fds, timeout);
        #endif
    }

    SYSTEM_CALL
    ssize_t readlink(const char *pathname, char *buf, size_t bufsiz)
    {
        #if SYSCALL_EXISTS(readlink)
        return DO_SYSCALL(readlink, pathname, buf, bufsiz);
        #else
        return readlinkat(AT_FDCWD, pathname, buf, bufsiz);
        #endif
    }

    SYSTEM_CALL
    int stat(const char *path, struct stat *buf)
    {
        #if SYSCALL_EXISTS(stat)
        return DO_SYSCALL(stat, path, buf);
        #else
        return fstatat(AT_FDCWD, path, buf, 0);
        #endif
    }

    SYSTEM_CALL
    int fstat(int fd, struct stat *buf)
    {
        return DO_SYSCALL(fstat, fd, buf);
    }

    SYSTEM_CALL
    int lstat(const char *pathname, struct stat *buf)
    {
        #if SYSCALL_EXISTS(lstat)
        return DO_SYSCALL(lstat, pathname, buf);
        #else
        return fstatat(AT_FDCWD, path, buf, AT_SYMLINK_NOFOLLOW);
        #endif
    }

    SYSTEM_CALL
    int rename(const char *oldpath, const char *newpath)
    {
        #if SYSCALL_EXISTS(rename)
        return DO_SYSCALL(rename, oldpath, newpath);
        #else
        return renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
        #endif
    }

    SYSTEM_CALL
    int symlink(const char *target, const char *linkpath)
    {
        #if SYSCALL_EXISTS(symlink)
        return DO_SYSCALL(symlink, target, linkpath);
        #else
        return symlinkat(target, AT_FDCWD, linkpath);
        #endif
    }

    SYSTEM_CALL
    int link(const char *oldpath, const char *newpath)
    {
        #if SYSCALL_EXISTS(link)
        return DO_SYSCALL(link, oldpath, newpath);
        #else
        return linkat(AT_FDCWD, oldpath, AT_FDCWD, newpath, 0);
        #endif
    }

    SYSTEM_CALL
    int unlink(const char *path)
    {
        #if SYSCALL_EXISTS(unlink)
        return DO_SYSCALL(unlink, path);
        #else
        return unlinkat(AT_FDCWD, path, 0);
        #endif
    }

    SYSTEM_CALL
    int mknod(const char *pathname, mode_t mode, dev_t dev)
    {
        #if SYSCALL_EXISTS(mknod)
        return DO_SYSCALL(mknod, pathname, mode, dev);
        #else
        return mknodat(AT_FDCWD, pathname, mode, dev);
        #endif
    }

    SYSTEM_CALL
    int mkfifo(const char *pathname, mode_t mode)
    {
        #if SYSCALL_EXISTS(mkfifo)
        return DO_SYSCALL(mkfifo, pathname, mode);
        #else
        return mkfifoat(AT_FDCWD, pathname, mode);
        #endif
    }

    SYSTEM_CALL
    int mkdir(const char *pathname, mode_t mode)
    {
        #if SYSCALL_EXISTS(mkdir)
        return DO_SYSCALL(mkdir, pathname, mode);
        #else
        return mkdirat(AT_FDCWD, pathname, mode);
        #endif
    }

    SYSTEM_CALL
    int rmdir(const char *pathname)
    {
        #if SYSCALL_EXISTS(rmdir)
        return DO_SYSCALL(rmdir, pathname);
        #else
        return unlinkat(AT_FDCWD, pathname, AT_REMOVEDIR);
        #endif
    }

    SYSTEM_CALL
    int chdir(const char *path)
    {
        return DO_SYSCALL(chdir, path);
    }

    SYSTEM_CALL
    int fchdir(int fd)
    {
        return DO_SYSCALL(fchdir, fd);
    }

    SYSTEM_CALL
    int chroot(const char *path)
    {
        return DO_SYSCALL(chroot, path);
    }

    SYSTEM_CALL
    int chown(const char *path, uid_t owner, gid_t group)
    {
        #if SYSCALL_EXISTS(chown)
        return DO_SYSCALL(chown, path, owner, group);
        #else
        return fchownat(AT_FDCWD, path, owner, group, 0);
        #endif
    }

    SYSTEM_CALL
    int fchown(int fd, uid_t owner, gid_t group)
    {
        return DO_SYSCALL(fchown, fd, owner, group);
    }

    SYSTEM_CALL
    int lchown(const char *pathname, uid_t owner, gid_t group)
    {
        #if SYSCALL_EXISTS(lchown)
        return DO_SYSCALL(lchown, pathname, owner, group);
        #else
        return fchownat(AT_FDCWD, pathname, owner, group, AT_SYMLINK_NOFOLLOW);
        #endif
    }

    SYSTEM_CALL
    int chmod(const char *path, mode_t mode)
    {
        #if SYSCALL_EXISTS(chmod)
        return DO_SYSCALL(chmod, path, mode);
        #else
        return chmodat(AT_FDCWD, path, mode);
        #endif
    }

    SYSTEM_CALL
    int fchmod(int fd, mode_t mode)
    {
        return DO_SYSCALL(fchmod, fd, mode);
    }

    SYSTEM_CALL
    int fcntl(int fd, int cmd)
    {
        return DO_SYSCALL(fcntl, fd, cmd);
    }

    /*
     * The best solution would be an 'auto' arg here.
     * But gcc has a bug and cannot generate the syscall correctly.
     * Keep it like this until gcc is fixed (probably never), or
     * until gcc support is definitely dropped in favor of clang.
     */
    SYSTEM_CALL
    int fcntl(int fd, int cmd, void *arg)
    {
        return DO_SYSCALL(fcntl, fd, cmd, arg);
    }

    SYSTEM_CALL
    int ioctl(int fd, unsigned long request)
    {
        return DO_SYSCALL(ioctl, fd, request);
    }

    /*
     * See above comment for fcntl.
     */
    SYSTEM_CALL
    int ioctl(int fd, unsigned long request, void *arg)
    {
        return DO_SYSCALL(ioctl, fd, request, arg);
    }

    SYSTEM_CALL
    int ioctl(int fd, unsigned long request, long arg)
    {
        return ioctl(fd, request, reinterpret_cast<void *>(arg));
    }

    #if SYSCALL_EXISTS(openat)
    SYSTEM_CALL
    int openat(int dirfd, const char *pathname, int flags)
    {
        return DO_SYSCALL(openat, dirfd, pathname, flags);
    }

    SYSTEM_CALL
    int openat(int dirfd, const char *pathname, int flags, mode_t mode)
    {
        return DO_SYSCALL(openat, dirfd, pathname, flags, mode);
    }
    #endif

    #if SYSCALL_EXISTS(faccessat)
    SYSTEM_CALL
    int faccessat(int dirfd, const char *pathname, int mode, int flags)
    {
        return DO_SYSCALL(faccessat, dirfd, pathname, mode, flags);
    }
    #endif

    #if SYSCALL_EXISTS(readlinkat)
    SYSTEM_CALL
    ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz)
    {
        return DO_SYSCALL(readlinkat, dirfd, pathname, buf, bufsiz);
    }
    #endif

    #if SYSCALL_EXISTS(fstatat) || SYSCALL_EXISTS(newfstatat)
    SYSTEM_CALL
    int fstatat(int dirfd, const char *pathname, struct stat *buf, int flags)
    {
        #if SYSCALL_EXISTS(fstatat)
        return DO_SYSCALL(fstatat, dirfd, pathname, buf, flags);
        #else
        //
        // Some Linux architectures renamed this newfstatat.
        //
        return DO_SYSCALL(newfstatat, dirfd, pathname, buf, flags);
        #endif
    }
    #endif

    #if SYSCALL_EXISTS(renameat)
    SYSTEM_CALL
    int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath)
    {
        return DO_SYSCALL(renameat, olddirfd, oldpath, newdirfd, newpath);
    }
    #endif

    #if SYSCALL_EXISTS(symlinkat)
    SYSTEM_CALL
    int symlinkat(const char *target, int newdirfd, const char *linkpath)
    {
        return DO_SYSCALL(symlinkat, target, newdirfd, linkpath);
    }
    #endif

    #if SYSCALL_EXISTS(linkat)
    SYSTEM_CALL
    int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags)
    {
        return DO_SYSCALL(linkat, olddirfd, oldpath, newdirfd, newpath, flags);
    }
    #endif

    #if SYSCALL_EXISTS(unlinkat)
    SYSTEM_CALL
    int unlinkat(int dirfd, const char *pathname, int flags)
    {
        return DO_SYSCALL(unlinkat, dirfd, pathname, flags);
    }
    #endif

    #if SYSCALL_EXISTS(mknodat)
    SYSTEM_CALL
    int mknodat(int dirfd, const char *pathname, mode_t mode, dev_t dev)
    {
        return DO_SYSCALL(mknodat, dirfd, pathname, mode, dev);
    }
    #endif

    #if SYSCALL_EXISTS(mkfifoat)
    SYSTEM_CALL
    int mkfifoat(int dirfd, const char *pathname, mode_t mode)
    {
        return DO_SYSCALL(mkfifoat, dirfd, pathname, mode);
    }
    #endif

    #if SYSCALL_EXISTS(mkdirat)
    SYSTEM_CALL
    int mkdirat(int dirfd, const char *pathname, mode_t mode)
    {
        return DO_SYSCALL(mkdirat, dirfd, pathname, mode);
    }
    #endif

    #if SYSCALL_EXISTS(fchownat)
    SYSTEM_CALL
    int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags)
    {
        return DO_SYSCALL(fchownat, dirfd, pathname, owner, group, flags);
    }
    #endif

    #if SYSCALL_EXISTS(fchmodat)
    SYSTEM_CALL
    int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags)
    {
        return DO_SYSCALL(fchmodat, dirfd, pathname, mode, flags);
    }
    #endif
}

#endif
