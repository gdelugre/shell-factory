#ifndef PICOLIB_STREAM_IMPL_H_
#define PICOLIB_STREAM_IMPL_H_

#include <sys/ioctl.h>
#include <fcntl.h>

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
    int SingleIO::close()
    {
        return Syscall::close(fd);
    }

    METHOD int SingleIO::close_on_exec()
    {
        return Syscall::fcntl(fd, F_SETFD, reinterpret_cast<void *>(FD_CLOEXEC));
    }

    template <typename Callback>
    METHOD
    int IO::select(std::initializer_list<SingleIO> r_ios,
                   std::initializer_list<SingleIO> w_ios,
                   std::initializer_list<SingleIO> e_ios,
                   long timeout, Callback cb)
    {
        struct timeval to, *ptv = nullptr;
        fd_set r_fds, w_fds, e_fds;
        fd_set *pr_fds = nullptr, *pw_fds = nullptr, *pe_fds = nullptr;
        int nfds = -1;

        if ( r_ios.size() > 0 )
        {
            FD_ZERO(&r_fds);
            for ( auto stm : r_ios )
            {
                int fd = stm.file_desc();
                FD_SET(fd, &r_fds);

                if ( fd > nfds )
                    nfds = fd;
            }

            pr_fds = &r_fds;
        }

        if ( w_ios.size() > 0 )
        {
            FD_ZERO(&w_fds);
            for ( auto stm : w_ios )
            {
                int fd = stm.file_desc();
                FD_SET(fd, &w_fds);

                if ( fd > nfds )
                    nfds = fd;
            }

            pw_fds = &w_fds;
        }

        if ( e_ios.size() > 0 )
        {
            FD_ZERO(&e_fds);
            for ( auto stm : e_ios )
            {
                int fd = stm.file_desc();
                FD_SET(fd, &e_fds);

                if ( fd > nfds )
                    nfds = fd;
            }

            pe_fds = &e_fds;
        }

        if ( timeout >= 0 )
        {
            to.tv_sec = timeout / 1000000;
            to.tv_usec = timeout % 1000000;
            ptv = &to;
        }

        int res = Syscall::select(nfds + 1, pr_fds, pw_fds, pe_fds, ptv);
        if ( res <= 0 )
            return res;

        if ( r_ios.size() > 0 )
        {
            for ( auto stm : r_ios )
                if ( FD_ISSET(stm.file_desc(), &r_fds) )
                    cb(INPUT_READY, stm);
        }

        if ( w_ios.size() > 0 )
        {
            for ( auto stm : w_ios )
                if ( FD_ISSET(stm.file_desc(), &w_fds) )
                    cb(OUTPUT_READY, stm);
        }

        if ( e_ios.size() > 0 )
        {
            for ( auto stm : e_ios )
                if ( FD_ISSET(stm.file_desc(), &w_fds) )
                    cb(POLL_ERROR, stm);
        }

        return res;
    }

    METHOD
    ssize_t BasicIO::in(void *ptr, size_t count)
    {
        ssize_t nr_bytes = Syscall::read(fd, ptr, count);
        if ( Target::is_error(nr_bytes) ) {
            b_error = true;
            return -1;
        }

        b_eof = (count > 0 && nr_bytes == 0);
        return nr_bytes;
    }

    METHOD
    ssize_t BasicIO::out(const void *ptr, size_t count)
    {
        ssize_t nr_bytes = Syscall::write(fd, ptr, count);
        if ( Target::is_error(nr_bytes) ) {
            b_error = true;
            return -1;
        }

        return nr_bytes;
    }

    METHOD
    size_t BasicIO::available_input_bytes()
    {
        size_t nr_bytes = 0;

        Syscall::ioctl(fd, FIONREAD, &nr_bytes);
        return nr_bytes;
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
