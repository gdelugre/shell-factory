#ifndef PICOLIB_FS_IMPL_H_
#define PICOLIB_FS_IMPL_H_

#include <sys/unistd.h>
#include <dirent.h>

#define foreach_dirent(dirents, ent, off, dsize) \
    for (off = 0, ent = dirents; \
            ent && off < dsize; \
            off += ent->d_reclen, ent = static_cast<struct dirent *>((void *)(((char *) ent) + ent->d_reclen)))

#define dirent_name(dirent) dirent->d_name

namespace Pico {

    namespace Filesystem {

        METHOD
        int Directory::get_current(char *buf, size_t size)
        {
            return Syscall::getcwd(buf, size);
        }

        template <typename Func>
        METHOD
        int Directory::each(const char *path, Func proc)
        {
            Directory dir(path);
            if ( dir.is_invalid() )
                return -1;

            int ret = dir.list(proc);

            dir.close();
            return ret;
        }

        template <typename Func>
        METHOD
        int Directory::list(Func proc)
        {
            // Fetch the list of directories.
            Memory::Region region;
            struct dirent *dirents = region;
            size_t read_size = 0;

            while ( true )
            {
                int ret = Syscall::getdents(fd, dirents, region.size() - read_size);
                if ( ret == 0 )
                    break;

                if ( Target::is_error(ret) )
                    return -1;

                read_size += ret;
                region.resize(region.size() * 2);
                dirents = static_cast<struct dirent *>((void *)((char *) region.pointer() + read_size));
            }

            // Process each directory entry and pass it to function.
            dirents = region;
            struct dirent *current;
            size_t off;
            int ret;

            foreach_dirent(dirents, current, off, read_size)
            {
                ret = proc(dirent_name(current));
                if ( ret != 0 )
                    break;
            }

            return ret;
        }
    }
}

#endif
