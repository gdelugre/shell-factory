#ifndef POSIX_PICO_FS_H_
#define POSIX_PICO_FS_H_

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

        constexpr int seek_whence(Seek method)
        {
            return (method == Seek::BEGIN) ? SEEK_SET :
                   (method == Seek::CURRENT) ? SEEK_CUR :
                   (method == Seek::END) ? SEEK_END : -1;
        }


        METHOD
        Memory::Region File::map(void *base, int prot, size_t size, file_off offset)
        {
            int flags = MAP_SHARED;
            void *ptr = Syscall::mmap(base, size, Memory::mmap_prot(prot), flags, file_desc(), offset);
            if ( Target::is_error(ptr) ) {
                ptr = nullptr;
                size = 0;
            }

            return Memory::Region::from(ptr, size);
        }

        METHOD
        File File::open(const char *path, int flags)
        {
            return File(path, flags, false);
        }

        METHOD
        int File::seek(off_t offset, Seek method)
        {
            return Syscall::lseek(this->file_desc(), offset, seek_whence(method));
        }

        METHOD
        File File::create(const char *path, int flags, Rights rights)
        {
            return File(path, flags, true, rights.value);
        }

        METHOD
        size_t File::size(const char *path)
        {
            struct stat st;

            Syscall::stat(path, &st);
            return st.st_size;
        }

        METHOD
        size_t File::size()
        {
            struct stat st;

            Syscall::fstat(this->file_desc(), &st);
            return st.st_size;
        }

        METHOD
        bool File::exists(const char *path)
        {
            return Syscall::access(path, F_OK) == 0;
        }

        METHOD
        bool File::is_readable(const char *path)
        {
            return Syscall::access(path, R_OK) == 0;
        }

        METHOD
        bool File::is_writable(const char *path)
        {
            return Syscall::access(path, W_OK) == 0;
        }

        METHOD
        bool File::is_executable(const char *path)
        {
            return Syscall::access(path, X_OK) == 0;
        }

        METHOD
        Owner File::owner(const char *path)
        {
            struct stat st;

            Syscall::stat(path, &st);
            return Owner(st.st_uid, st.st_gid);
        }

        METHOD
        int File::change_owner(const char *path, Owner owner)
        {
            return Syscall::chown(path, owner.user.id(), owner.group.id());
        }

        METHOD
        Rights File::rights(const char *path)
        {
            struct stat st;

            Syscall::stat(path, &st);
            return Rights(st.st_mode);
        }

        METHOD
        int File::change_rights(const char *path, Rights rights)
        {
            return Syscall::chmod(path, rights.value);
        }

        METHOD
        Owner File::owner()
        {
            struct stat st;

            Syscall::fstat(this->file_desc(), &st);
            return Owner(st.st_uid, st.st_gid);
        }

        METHOD
        int File::change_owner(Owner owner)
        {
            return Syscall::fchown(this->file_desc(), owner.user.id(), owner.group.id());
        }

        METHOD
        Rights File::rights()
        {
            struct stat st;

            Syscall::fstat(this->file_desc(), &st);
            return Rights(st.st_mode);
        }

        METHOD
        int File::change_rights(Rights rights)
        {
            return Syscall::fchmod(this->file_desc(), rights.value);
        }

        CONSTRUCTOR
        File::File(const char *path, int flags, bool create, Rights rights)
        {
            int fd;

            if ( create )
                fd = Syscall::open(path, open_flags(flags) | O_CREAT, rights.value);
            else
                fd = Syscall::open(path, open_flags(flags));

            if ( Target::is_error(fd) )
                fd = Target::invalid_handle;

            io = BasicIO(fd);
        }

        METHOD
        int File::remove(const char *path)
        {
            return Syscall::unlink(path);
        }

        METHOD
        int Directory::create(const char *path, Rights rights)
        {
            return Syscall::mkdir(path, rights.value);
        }

        METHOD
        int Directory::remove(const char *path)
        {
            return Syscall::rmdir(path);
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
            if ( Target::is_error(fd) )
                fd = Target::invalid_handle;
        }

        METHOD
        Directory Directory::open(const char *path)
        {
            return Directory(path);
        }

        METHOD
        int Directory::set_current()
        {
            return Syscall::fchdir(fd);
        }

    }
}

#endif
