#ifndef PICOLIB_SOCKET_IMPL_H_
#define PICOLIB_SOCKET_IMPL_H_

#include <sys/un.h>

namespace Pico {

    namespace Network {

        FUNCTION
        uint16_t host_to_net(uint16_t port)
        {
            return CPU::to_big_endian(port);
        }

        // From glibc 2.19, sysdeps/unix/sysv/linux/cmsg_nxthdr.c
        FUNCTION
        struct cmsghdr *cmsg_nxthdr (struct msghdr *mhdr, struct cmsghdr *cmsg)
        {
            if ((size_t) cmsg->cmsg_len < sizeof (struct cmsghdr))
            /* The kernel header does this so there may be a reason.  */
                return NULL;

            cmsg = (struct cmsghdr *) ((unsigned char *) cmsg
                    + CMSG_ALIGN (cmsg->cmsg_len));

            if ((unsigned char *) (cmsg + 1) > ((unsigned char *) mhdr->msg_control
                                                + mhdr->msg_controllen)
                || ((unsigned char *) cmsg + CMSG_ALIGN (cmsg->cmsg_len)
                    > ((unsigned char *) mhdr->msg_control + mhdr->msg_controllen)))
                /* No more entries.  */
                return NULL;

            return cmsg;
        }


        template <enum AddressType>
        struct Sockaddr;

        template <>
        struct Sockaddr<IPV4>
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
        struct Sockaddr<IPV6>
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
        struct Sockaddr<UNIX>
        {
            static constexpr int family = AF_UNIX;
            using type = struct sockaddr_un;

            FUNCTION
            struct sockaddr_un pack(UnixAddress const unixaddr, size_t& size)
            {
                struct sockaddr_un addr;
                String sock_path(addr.sun_path);

                addr.sun_family = AF_UNIX;
                sock_path = unixaddr.path;

                size = sizeof(addr.sun_family) + String(unixaddr.path).length();
                return addr;
            }
        };

        template <>
        struct Sockaddr<UNIX_ABSTRACT>
        {
            static constexpr int family = AF_UNIX;
            using type = struct sockaddr_un;

            FUNCTION
            struct sockaddr_un pack(UnixAbstractAddress const unixaddr, size_t& size)
            {
                struct sockaddr_un addr;
                String sock_path(addr.sun_path + 1);

                addr.sun_family = AF_UNIX;
                addr.sun_path[0] = '\0';
                sock_path = unixaddr.path;

                size = sizeof(addr.sun_family) + String(unixaddr.path).length() + 1;
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
        Socket::Socket(int domain, int type, int protocol) : Socket( Syscall::socket(domain, type, protocol) ) {}

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

        template <enum AddressType T>
        METHOD
        int Socket::bind(Address<T> addr)
        {
            static_assert(T == UNIX || T == UNIX_ABSTRACT, "This method only supports UNIX address sockets.");

            size_t addr_len;
            auto bind_addr = Sockaddr<T>::pack(addr, addr_len);
            return Syscall::bind(this->file_desc(), reinterpret_cast<struct sockaddr *>(&bind_addr), addr_len);
        }

        template <enum AddressType T>
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

        template <enum AddressType T>
        METHOD
        int StreamSocket::connect(Address<T> addr)
        {
            static_assert(T == UNIX || T == UNIX_ABSTRACT, "This method only supports UNIX address sockets.");
            size_t addr_len;
            auto serv_addr = Sockaddr<T>::pack(addr, addr_len);

            return Syscall::connect(this->file_desc(), reinterpret_cast<struct sockaddr *>(&serv_addr), addr_len);
        }

        template <enum AddressType T>
        METHOD
        int StreamSocket::connect(Address<T> addr, uint16_t port)
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
            do {
                int client_fd = Syscall::accept(this->file_desc(), nullptr, 0);
                if ( Fork )
                {
                    if ( Syscall::fork() == 0 )
                        return StreamSocket(client_fd);
                    else
                        Syscall::close(client_fd);
                }
                else
                    return StreamSocket(client_fd);

            } while ( Fork );
        }

        METHOD
        int UnixStreamSocket::send_io(SingleIO const& io)
        {
            char dummy                       = 0;
            int fd                           = io.file_desc();
            struct msghdr msg                = {0};
            char buf[CMSG_SPACE(sizeof(fd))] = {0};
            struct iovec iov                 = { .iov_base = &dummy, .iov_len = sizeof(dummy) };

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
            int fd                           = -1;
            struct msghdr msg                = {0};
            char buf[CMSG_SPACE(sizeof(fd))] = {0};
            struct iovec iov                 = { .iov_base = &dummy, .iov_len = sizeof(dummy) };

            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;
            msg.msg_control = buf;
            msg.msg_controllen = sizeof(buf);

            if ( Syscall::recvmsg(this->file_desc(), &msg, 0) == -1 )
                return SingleIO(fd);

            for ( struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
                  cmsg != nullptr;
                  cmsg = cmsg_nxthdr(&msg, cmsg) )
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
