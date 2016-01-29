#ifndef LINUX_TYPES_H_
#define LINUX_TYPES_H_

#include <linux/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>

namespace Target {

    /* File descriptors. */
    using handle = int; 

    namespace Security {
        using user_id = uid_t;
        using group_id = gid_t;
    }

    namespace Process {
        using thread_id = pid_t;
        using process_id = pid_t;
        using signal_handler = sighandler_t;
        using thread_routine = int (*)(void *);
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

    namespace Memory {
        using shm_handle = int;
    }

    //
    // random_pool corresponds to the file descriptor of /dev/urandom if getrandom is not available.
    //
    #if SYSCALL_EXISTS(getrandom)
    using random_pool = EmptyType;
    #else
    using random_pool = handle;
    #endif

    using error_code = int;
    constexpr error_code max_error = 4095;

    template <typename T>
    FUNCTION
    constexpr bool is_error(T err)
    {
        return Options::disable_error_checks ? false :
               (reinterpret_cast<unsigned long>(err) >= static_cast<unsigned long>(-max_error));
    }
}

#endif
