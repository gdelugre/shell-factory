#ifndef FREEBSD_TYPES_H_
#define FREEBSD_TYPES_H_

// Import POSIX type definitions.
#include <target/posix/types.h>

#include <termios.h>
#include <sys/mman.h>

namespace Target {

    namespace Process {
        using thread_id = lwpid_t;
        using thread_routine = void (*)(void *);
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

        static constexpr int mmap_stack_flag = MAP_STACK;
    }

    using random_pool = handle;

    using mutex_t = unsigned long;
    using terminal_settings = struct termios;
}

#endif
