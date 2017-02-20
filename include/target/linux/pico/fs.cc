#ifndef PICOLIB_FS_IMPL_H_
#define PICOLIB_FS_IMPL_H_

#define foreach_dirent(dirents, dirent, off, dsize) \
    for (off = 0, dirent = dirents; \
            dirent && off < dsize; \
            off += dirent->d_reclen, dirent = static_cast<struct linux_dirent *>((void *)(((char *) dirent) + dirent->d_reclen)))

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
            struct linux_dirent *dirents = region;
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
                dirents = static_cast<struct linux_dirent *>((void *)((char *) region.pointer() + read_size));
            }

            // Process each directory entry and pass it to function.
            dirents = region;
            struct linux_dirent *current;
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
