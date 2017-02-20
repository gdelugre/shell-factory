#ifndef LINUX_TYPES_H_
#define LINUX_TYPES_H_

// Import POSIX type definitions.
#include <target/posix/types.h>

#include <asm-generic/termios.h>
#include <sys/mman.h>

namespace Target {

    namespace Process {
        using thread_id = pid_t;
        using thread_routine = int (*)(void *);
    }

    namespace Network {
        enum class AddressType {
            IPV4,
            IPV6,
            UNIX,
            UNIX_ABSTRACT
        };
    }

    namespace Flags {
        using namespace Memory::Flags;

        static constexpr int mmap_stack_flag = MAP_GROWSDOWN;
    }

    //
    // random_pool corresponds to the file descriptor of /dev/urandom if getrandom is not available.
    //
    using random_pool = handle;

    using mutex_t = int;
    using terminal_settings = struct termios;
}

#endif
