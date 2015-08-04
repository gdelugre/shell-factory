#ifndef PICOLIB_FS_IMPL_H_
#define PICOLIB_FS_IMPL_H_

#define foreach_dirent(dirents, dirent, off, dsize) \
    for (off = 0, dirent = dirents; \
            dirent && off < dsize; \
            off += dirent->d_reclen, dirent = static_cast<struct linux_dirent *>((void *)(((char *) dirent) + dirent->d_reclen)))

#define dirent_name(dirent) dirent->d_name

namespace Pico {

    namespace Filesystem {

        constexpr int open_flags(int pico_flags)
        {
            int flags = O_NONBLOCK;

            // TODO: add assert if not read and not write
            if ( pico_flags & File::READ && !(pico_flags & File::WRITE) )
                flags |= O_RDONLY;
            else if ( pico_flags & File::WRITE && !(pico_flags & File::READ) )
                flags |= O_WRONLY;
            else
                flags |= O_RDWR;

            if ( pico_flags & File::APPEND )
                flags |= O_APPEND;

            if ( pico_flags & File::TRUNCATE )
                flags |= O_TRUNC;

            return flags;
        }

        METHOD
        File&& File::open(const char *path, int flags)
        {
            return File(path, flags, false);
        }

        METHOD
        File&& File::create(const char *path, int flags, mode_t mode)
        {
            return File(path, flags, true, mode);
        }

        CONSTRUCTOR
        File::File(const char *path, int flags, bool create, mode_t mode)
        {
            if ( create )
                fd = Syscall::create(path, open_flags(flags), mode);
            else
                fd = Syscall::open(path, open_flags(flags));
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
