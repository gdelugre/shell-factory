#ifndef DARWIN_TYPES_H_
#define DARWIN_TYPES_H_

// Import POSIX type definitions.
#include <target/posix/types.h>

#include <mach/port.h>
#include <mach/semaphore.h>
#include <termios.h>

namespace Target {

    namespace Process {
        using thread_id = mach_port_t;
        using thread_routine = void (*)(void *);
    }

    namespace Network {
        enum class AddressType {
            IPV4,
            IPV6,
            UNIX
        };
    }

    namespace Flags {
        using namespace Memory::Flags;
    }

    using random_pool = handle;

    using mutex_t = semaphore_t;
    using terminal_settings = struct termios;
}

#endif
