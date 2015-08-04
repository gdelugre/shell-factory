#ifndef PICOLIB_STREAM_IMPL_H_
#define PICOLIB_STREAM_IMPL_H_

namespace Pico {

    METHOD
    int Stream::read(void *ptr, size_t count)
    {
        Syscall::read(fd, ptr, count);
    }

    METHOD
    int Stream::write(void *ptr, size_t count)
    {
        Syscall::write(fd, ptr, count);
    }
    
    METHOD
    int Stream::close()
    {
        Syscall::close(fd);
    }
}

#endif
