#ifndef PICOLIB_SOCKET_IMPL_H_
#define PICOLIB_SOCKET_IMPL_H_

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>

namespace Pico {

    namespace Network {

        template <AddressType>
        struct Sockaddr;

        template <>
        struct Address<AddressType::UNIX_ABSTRACT>
        {
            char *path;
        };

        using UnixAbstractAddress   = Address<AddressType::UNIX_ABSTRACT>;

        template <>
        struct Sockaddr<AddressType::UNIX_ABSTRACT>
        {
            static constexpr int family = AF_UNIX;
            using type = struct sockaddr_un;

            FUNCTION
            struct sockaddr_un pack(UnixAbstractAddress const unixaddr, size_t& size)
            {
                struct sockaddr_un addr;

                addr.sun_family = AF_UNIX;
                addr.sun_path[0] = '\0';
                String::copy(addr.sun_path + 1, unixaddr.path, sizeof(addr.sun_path) - 1);

                size = sizeof(addr.sun_family) + String::length(unixaddr.path) + 1;
                return addr;
            }
        };

        METHOD
        size_t SocketIO::available_input_bytes()
        {
            size_t nr_bytes = 0;

            Syscall::ioctl(fd, FIONREAD, &nr_bytes);
            return nr_bytes;
        }

        CONSTRUCTOR
        SctpSocket::SctpSocket() : StreamSocket(AF_INET, IPPROTO_SCTP) {}

        CONSTRUCTOR
        Sctp6Socket::Sctp6Socket() : StreamSocket(AF_INET6, IPPROTO_SCTP) {}
    }
}

#endif
