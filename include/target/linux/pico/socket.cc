#ifndef PICOLIB_SOCKET_IMPL_H_
#define PICOLIB_SOCKET_IMPL_H_

namespace Pico {

    namespace Network {

        FUNCTION
        uint16_t _htons(uint16_t port)
        {
            return cpu_to_be16(port);
        }

        template <unsigned N>
        struct Sockaddr;

        template <>
        struct Sockaddr<4>
        {
            static constexpr int family = AF_INET;
            typedef struct sockaddr_in type;

            FUNCTION
            struct sockaddr_in pack(IpAddress<4> const ip, uint16_t port)
            {
                struct sockaddr_in addr;

                addr.sin_family         = family;
                addr.sin_port           = _htons(port);
                addr.sin_addr.s_addr    = ip.value;

                return addr;
            }
        };

        template <>
        struct Sockaddr<6>
        {
            static constexpr int family = AF_INET6;
            typedef struct sockaddr_in6 type;

            FUNCTION
            struct sockaddr_in6 pack(IpAddress<6> const ip, uint16_t port)
            {
                struct sockaddr_in6 addr;

                addr.sin6_flowinfo      = 0;
                addr.sin6_family        = AF_INET6;
                addr.sin6_port          = _htons(port);
                _memcpy(&addr.sin6_addr.s6_addr, &ip, sizeof(ip));

                return addr;
            }
        };

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

        template <unsigned N>
        METHOD
        int StreamSocket::connect(IpAddress<N> ip, uint16_t port)
        {
            auto serv_addr = Sockaddr<N>::pack(ip, port);
            
            return Syscall::connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        }        
    }
}

#endif
