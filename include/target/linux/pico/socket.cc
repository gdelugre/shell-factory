#ifndef PICOLIB_SOCKET_IMPL_H_
#define PICOLIB_SOCKET_IMPL_H_

namespace Pico {

    namespace Network {

        FUNCTION
        uint16_t _htons(uint16_t port)
        {
            return cpu_to_be16(port);
        }

        FUNCTION 
        void pack_sockaddr(struct sockaddr_in& addr, IpAddress<4> const ip, uint16_t port)
        {
            addr.sin_family         = AF_INET;
            addr.sin_port           = _htons(port);
            addr.sin_addr.s_addr    = ip.value;
        }

        FUNCTION
        void pack_sockaddr(struct sockaddr_in6& addr, IpAddress<6> const ip, uint16_t port)
        {
            addr.sin6_flowinfo      = 0;
            addr.sin6_family        = AF_INET6;
            addr.sin6_port          = _htons(port);
            _memcpy(&addr.sin6_addr.s6_addr, &ip, sizeof(ip));
        }

        CONSTRUCTOR
        Socket::Socket(int domain, int type, int protocol)
        {
            fd = Syscall::socket(domain, type, protocol);
        }

        METHOD
        int Socket::set(int level, int optname, void *val, size_t len)
        {
            return Syscall::setsockopt(fd, level, optname, val, len);
        }

        METHOD
        int StreamSocket::connect(IpAddress<4> ip, uint16_t port)
        {
            struct sockaddr_in  serv_addr; 

            pack_sockaddr(serv_addr, ip, port);
            return Syscall::connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        }

        METHOD
        int StreamSocket::connect(IpAddress<6> ip, uint16_t port)
        {
            struct sockaddr_in6 serv_addr;

            pack_sockaddr(serv_addr, ip, port);
            return Syscall::connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        }

        METHOD
        int StreamSocket::listen(IpAddress<4> ip, uint16_t port, bool reuse_addr)
        {
        }

        METHOD
        int StreamSocket::listen(IpAddress<6> ip, uint16_t port, bool reuse_addr)
        {
        }
    }
}

#endif
