#ifndef POSIX_PICO_SOCKET_H_
#define POSIX_PICO_SOCKET_H_

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

namespace Pico {

    namespace Network {

        template <AddressType>
        struct Address;

        template <>
        struct Address<AddressType::UNIX>
        {
            char *path;
        };

        using UnixAddress           = Address<AddressType::UNIX>;

        FUNCTION
        uint16_t host_to_net(uint16_t port)
        {
            return CPU::to_big_endian(port);
        }

        template <AddressType>
        struct Sockaddr;

        template <>
        struct Sockaddr<AddressType::IPV4>
        {
            static constexpr int family = AF_INET;
            using type = struct sockaddr_in;

            FUNCTION
            struct sockaddr_in pack(IpAddress const ip, uint16_t port, size_t& size)
            {
                struct sockaddr_in addr;

                addr.sin_family         = family;
                addr.sin_port           = host_to_net(port);
                addr.sin_addr.s_addr    = ip.value;

                size = sizeof(addr);
                return addr;
            }
        };

        template <>
        struct Sockaddr<AddressType::IPV6>
        {
            static constexpr int family = AF_INET6;
            using type = struct sockaddr_in6;

            FUNCTION
            struct sockaddr_in6 pack(IpAddress6 const ip, uint16_t port, size_t& size)
            {
                struct sockaddr_in6 addr;

                addr.sin6_flowinfo      = 0;
                addr.sin6_family        = AF_INET6;
                addr.sin6_port          = host_to_net(port);
                Memory::copy(&addr.sin6_addr.s6_addr, &ip, sizeof(ip));

                size = sizeof(addr);
                return addr;
            }
        };

        template <>
        struct Sockaddr<AddressType::UNIX>
        {
            static constexpr int family = AF_UNIX;
            using type = struct sockaddr_un;

            FUNCTION
            struct sockaddr_un pack(UnixAddress const unixaddr, size_t& size)
            {
                struct sockaddr_un addr;

                addr.sun_family = AF_UNIX;
                String::copy(addr.sun_path, unixaddr.path, sizeof(addr.sun_path));

                size = sizeof(addr.sun_family) + String::length(unixaddr.path);
                return addr;
            }
        };

        METHOD
        ssize_t SocketIO::in(void *buf, size_t count)
        {
            return Syscall::recv(fd, buf, count, MSG_WAITALL);
        }

        METHOD
        ssize_t SocketIO::out(const void *buf, size_t count)
        {
            // Use write(2) instead of send(2) since it takes 3 arguments instead of 4.
            // This produces slightly smaller code.

            return Syscall::write(fd, buf, count);
        }

        CONSTRUCTOR
        Socket::Socket(int domain, int type, int protocol)
        {
            int fd = Syscall::socket(domain, type, protocol);
            if ( Target::is_error(fd) )
                fd = Target::invalid_handle;

            io = SocketIO(fd);
        }

        CONSTRUCTOR
        SequencedSocket::SequencedSocket(int domain, int protocol) : Socket(domain, SOCK_SEQPACKET, protocol) {}

        CONSTRUCTOR
        DatagramSocket::DatagramSocket(int domain, int protocol) : Socket(domain, SOCK_DGRAM, protocol) {}

        CONSTRUCTOR
        UdpSocket::UdpSocket() : DatagramSocket(AF_INET, IPPROTO_IP) {}

        CONSTRUCTOR
        Udp6Socket::Udp6Socket() : DatagramSocket(AF_INET6, IPPROTO_IP) {}

        CONSTRUCTOR
        StreamSocket::StreamSocket(int domain, int protocol) : Socket(domain, SOCK_STREAM, protocol) {}

        CONSTRUCTOR
        TcpSocket::TcpSocket() : StreamSocket(AF_INET, IPPROTO_IP) {}

        CONSTRUCTOR
        Tcp6Socket::Tcp6Socket() : StreamSocket(AF_INET6, IPPROTO_IP) {}

        CONSTRUCTOR
        UnixStreamSocket::UnixStreamSocket() : StreamSocket(AF_UNIX, 0) {}

        METHOD
        int Socket::get(int level, int optname, void *val, unsigned *len)
        {
            return Syscall::getsockopt(this->file_desc(), level, optname, val, len);
        }

        METHOD
        int Socket::set(int level, int optname, void *val, unsigned len)
        {
            return Syscall::setsockopt(this->file_desc(), level, optname, val, len);
        }

        template <AddressType T>
        METHOD
        int Socket::bind(Address<T> addr)
        {
            size_t addr_len;
            auto bind_addr = Sockaddr<T>::pack(addr, addr_len);
            return Syscall::bind(this->file_desc(), reinterpret_cast<struct sockaddr *>(&bind_addr), addr_len);
        }

        template <AddressType T>
        METHOD
        int Socket::bind(Address<T> addr, uint16_t port, bool reuse_addr)
        {
            if ( reuse_addr )
            {
                int option = 1;
                set(SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
            }

            size_t addr_len;
            auto bind_addr = Sockaddr<T>::pack(addr, port, addr_len);
            return Syscall::bind(this->file_desc(), reinterpret_cast<struct sockaddr *>(&bind_addr), addr_len);
        }

        template <AddressType T>
        METHOD
        int Socket::connect(Address<T> addr)
        {
            size_t addr_len;
            auto serv_addr = Sockaddr<T>::pack(addr, addr_len);

            return Syscall::connect(this->file_desc(), reinterpret_cast<struct sockaddr *>(&serv_addr), addr_len);
        }

        template <AddressType T>
        METHOD
        int Socket::connect(Address<T> addr, uint16_t port)
        {
            size_t addr_len;
            auto serv_addr = Sockaddr<T>::pack(addr, port, addr_len);

            return Syscall::connect(this->file_desc(), reinterpret_cast<struct sockaddr *>(&serv_addr), addr_len);
        }

        METHOD
        int StreamSocket::listen(int backlog)
        {
            return Syscall::listen(this->file_desc(), backlog);
        }

        template <bool Fork>
        METHOD
        StreamSocket StreamSocket::accept()
        {
            int client_fd;

            do {
                client_fd = Syscall::accept(this->file_desc(), nullptr, 0);
                if ( Target::is_error(client_fd) ) {
                    client_fd = Target::invalid_handle;
                    continue;
                }

                if ( !Fork )
                    break;

                if ( Syscall::fork() == 0 )
                    break;
                else
                    Syscall::close(client_fd);

            } while ( Fork );

            return StreamSocket(client_fd);
        }

        METHOD
        int UnixStreamSocket::send_io(SingleIO const& io)
        {
            char dummy                       = 0;
            int fd                           = io.file_desc();
            struct msghdr msg                = {};
            struct iovec iov                 = { .iov_base = &dummy, .iov_len = sizeof(dummy) };
            char buf[CMSG_SPACE(sizeof(fd))];

            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;
            msg.msg_control = buf;
            msg.msg_controllen = sizeof(buf);

            struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
            cmsg->cmsg_level = SOL_SOCKET;
            cmsg->cmsg_type = SCM_RIGHTS;
            cmsg->cmsg_len = CMSG_LEN(sizeof(fd));

            *((int *) CMSG_DATA(cmsg)) = fd;

            return Syscall::sendmsg(this->file_desc(), &msg, 0);
        }

        METHOD
        SingleIO UnixStreamSocket::recv_io()
        {
            char dummy;
            int fd                           = Target::invalid_handle;
            struct msghdr msg                = {};
            struct iovec iov                 = { .iov_base = &dummy, .iov_len = sizeof(dummy) };
            char buf[CMSG_SPACE(sizeof(fd))];

            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;
            msg.msg_control = buf;
            msg.msg_controllen = sizeof(buf);

            if ( Syscall::recvmsg(this->file_desc(), &msg, 0) == -1 )
                return SingleIO(fd);

            for ( struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
                  cmsg != nullptr;
                  cmsg = CMSG_NXTHDR(&msg, cmsg) )
            {
                if ( cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS )
                {
                    fd = *((int *) CMSG_DATA(cmsg));
                    break;
                }
            }

            return SingleIO(fd);
        }
    }
}

#endif
