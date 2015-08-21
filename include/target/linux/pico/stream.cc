#ifndef PICOLIB_STREAM_IMPL_H_
#define PICOLIB_STREAM_IMPL_H_

namespace Pico {

    namespace Stdio {
        constexpr int STDIN_FD  = 0;
        constexpr int STDOUT_FD = 1;
        constexpr int STDERR_FD = 2;

        METHOD
        BasicStream input()
        {
            return BasicStream(STDIN_FD);
        }

        METHOD
        BasicStream output()
        {
            return BasicStream(STDOUT_FD);
        }

        METHOD
        BasicStream error()
        {
            return BasicStream(STDERR_FD);
        }
    }

    METHOD
    ssize_t BasicIO::in(void *ptr, size_t count)
    {
        return Syscall::read(fd, ptr, count);
    }

    METHOD
    ssize_t BasicIO::out(const void *ptr, size_t count)
    {
        return Syscall::write(fd, ptr, count);
    }

    METHOD
    int BasicIO::close()
    {
        return Syscall::close(fd);
    }

    template <typename Io>
    template <typename T>
    METHOD
    void Stream<Io>::duplicate(Stream<T>& stm)
    {
        Syscall::dup2(this->file_desc(), stm.file_desc());
    }
}

#endif
