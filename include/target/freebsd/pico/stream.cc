#ifndef PICOLIB_STREAM_IMPL_H_
#define PICOLIB_STREAM_IMPL_H_

#include <sys/ioctl.h>
#include <fcntl.h>

namespace Pico {

    METHOD
    size_t BasicIO::available_input_bytes()
    {
        size_t nr_bytes = 0;

        Syscall::ioctl(fd, FIONREAD, &nr_bytes);
        return nr_bytes;
    }
}

#endif
