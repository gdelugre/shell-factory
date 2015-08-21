#ifndef PICOLIB_FS_IMPL_H_
#define PICOLIB_FS_IMPL_H_

#define foreach_dirent(dirents, dirent, off, dsize) \
    for (off = 0, dirent = dirents; \
            dirent && off < dsize; \
            off += dirent->d_reclen, dirent = static_cast<struct linux_dirent *>((void *)(((char *) dirent) + dirent->d_reclen)))

#define dirent_name(dirent) dirent->d_name

namespace Pico {

    namespace Filesystem {

        // TODO: would be better with a nicely formatted constexpr, but keep it that way for gcc <= 4.9 compatibility.
        constexpr int open_flags(int pico_flags)
        {
            return (pico_flags & File::TRUNCATE ? O_TRUNC : 0) |
                   (pico_flags & File::APPEND ? O_APPEND : 0) |
                   (((pico_flags & File::READ) && !(pico_flags & File::WRITE)) ? O_RDONLY : 0) |
                   (((pico_flags & File::WRITE && !(pico_flags & File::READ))) ? O_WRONLY : 0) |
                   (((pico_flags & File::READ) && (pico_flags & File::WRITE)) ? O_RDWR : 0) |
                   O_NONBLOCK;
        }

        METHOD
        File File::open(const char *path, int flags)
        {
            return File(path, flags, false);
        }

        METHOD
        File File::create(const char *path, int flags, mode_t mode)
        {
            return File(path, flags, true, mode);
        }

        CONSTRUCTOR
        File::File(const char *path, int flags, bool create, mode_t mode)
        {
            int fd;

            if ( create )
                fd = Syscall::create(path, open_flags(flags), mode);
            else
                fd = Syscall::open(path, open_flags(flags));

            io = BasicIO(fd);
        }

        METHOD
        int Directory::create(const char *path, mode_t mode)
        {
            return Syscall::mkdir(path, mode);
        }

        METHOD
        int Directory::remove(const char *path)
        {
            return Syscall::rmdir(path);
        }

        METHOD
        int Directory::get_current(char *buf, size_t size)
        {
            return Syscall::getcwd(buf, size);
        }

        METHOD
        int Directory::set_current(const char *path)
        {
            return Syscall::chdir(path);
        }

        METHOD
        int Directory::change_root(const char *path)
        {
            return Syscall::chroot(path);
        }

        CONSTRUCTOR
        Directory::Directory(const char *path)
        {
            fd = Syscall::open(path, O_RDONLY | O_DIRECTORY);
        }

        METHOD
        int Directory::set_current()
        {
            return Syscall::fchdir(fd);
        }

        METHOD
        int Directory::close()
        {
            return Syscall::close(fd);
        }

        template <typename Func>
        METHOD
        int Directory::each(const char *path, Func proc)
        {
            Directory dir(path);
            if ( dir.file_desc() < 0 )
                return dir.file_desc();

            int ret = dir.list(proc);

            dir.close();
            return ret;
        }

        template <typename Func>
        METHOD
        int Directory::list(Func proc)
        {
            // Fetch the list of directories.
            Memory::Buffer buffer(PAGE_SIZE);
            struct linux_dirent *dirents = buffer.as<struct linux_dirent *>();
            size_t read_size = 0;

            while ( true )
            {
                int ret = Syscall::getdents(fd, dirents, buffer.size() - read_size);
                if ( ret == 0 )
                    break;

                if ( ret < 0 )
                {
                    buffer.free();
                    return ret;
                }

                read_size += ret;
                buffer.resize(buffer.size() * 2);
                dirents = static_cast<struct linux_dirent *>((void *)((char *) buffer.pointer() + read_size));
            }

            // Process each directory entry and pass it to function.
            dirents = buffer.as<struct linux_dirent *>();
            struct linux_dirent *current;
            size_t off;
            int ret;

            foreach_dirent(dirents, current, off, read_size)
            {
                ret = proc(dirent_name(current));
                if ( ret != 0 )
                    break;
            }

            buffer.free();
            return ret;
        }
    }
}

#endif
