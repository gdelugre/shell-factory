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
    Stream& Stream::read(void *ptr, size_t count)
    {
        Syscall::read(fd, ptr, count);
        return *this;
    }

    METHOD
    Stream& Stream::write(void *ptr, size_t count)
    {
        Syscall::write(fd, ptr, count);
        return *this;
    }

    METHOD
    Stream Stream::duplicate()
    {
        return Stream( Syscall::dup(fd) );
    }

    METHOD
    void Stream::replace(Stream const& stm)
    {
        Syscall::dup2(stm.file_desc(), fd);
    }
    
    METHOD
    int Stream::close()
    {
        return Syscall::close(fd);
    }
}

#endif
