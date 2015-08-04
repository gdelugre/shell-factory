#ifndef PICOLIB_FS_IMPL_H_
#define PICOLIB_FS_IMPL_H_

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
    }
}

#endif
