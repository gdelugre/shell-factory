#ifndef FREEBSD_TYPES_H_
#define FREEBSD_TYPES_H_

#include <sys/types.h>
#include <sys/stat.h>

namespace Target {

    /* File descriptors. */
    using handle = int; 

    namespace Security {
        using user_id = uid_t;
        using group_id = gid_t;
    }

    namespace Process {
        using thread_id = lwpid_t;
        using process_id = pid_t;
        using signal_handler = void (*)(int);
        using thread_routine = void (*)(void *);
    }

    namespace Filesystem {
        using fs_rights = mode_t;
        using file_off = off_t;

        static constexpr fs_rights USER_READ   = S_IRUSR;
        static constexpr fs_rights USER_WRITE  = S_IWUSR;
        static constexpr fs_rights USER_EXEC   = S_IXUSR;
        static constexpr fs_rights GROUP_READ  = S_IRGRP;
        static constexpr fs_rights GROUP_WRITE = S_IWGRP;
        static constexpr fs_rights GROUP_EXEC  = S_IXGRP;
        static constexpr fs_rights OTHER_READ  = S_IROTH;
        static constexpr fs_rights OTHER_WRITE = S_IWOTH;
        static constexpr fs_rights OTHER_EXEC  = S_IXOTH;

        static constexpr fs_rights default_file_rights = USER_READ|USER_WRITE | GROUP_READ | OTHER_READ;
        static constexpr fs_rights default_dir_rights  = USER_READ|USER_WRITE|USER_EXEC |
                                                         GROUP_READ|GROUP_WRITE |
                                                         OTHER_READ|OTHER_WRITE;
    }

    using random_pool = handle;
}

#endif
