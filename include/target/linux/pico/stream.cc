#ifndef PICOLIB_STREAM_IMPL_H_
#define PICOLIB_STREAM_IMPL_H_

namespace Pico {

    constexpr int STDIN_FD  = 0;
    constexpr int STDOUT_FD = 1;
    constexpr int STDERR_FD = 2;

    METHOD
    Stream Stream::standard_input()
    {
        return Stream(STDIN_FD);
    }

    METHOD
    Stream Stream::standard_output()
    {
        return Stream(STDOUT_FD);
    }

    METHOD
    Stream Stream::standard_error()
    {
        return Stream(STDERR_FD);
    }

    METHOD
    ssize_t Stream::in(void *ptr, size_t count)
    {
        return Syscall::read(fd, ptr, count);
    }

    METHOD
    ssize_t Stream::out(const void *ptr, size_t count)
    {
        return Syscall::write(fd, ptr, count);
    }

    METHOD
    Stream Stream::duplicate()
    {
        return Stream( Syscall::dup(fd) );
    }

    METHOD
    void Stream::duplicate(Stream& stm)
    {
        Syscall::dup2(fd, stm.file_desc());
    }

    METHOD
    void Stream::flush()
    {
        Syscall::fsync(fd);
    }
    
    METHOD
    int Stream::close()
    {
        return Syscall::close(fd);
    }
}

#endif
