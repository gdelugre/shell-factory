#ifndef PICOLIB_SOCKET_IMPL_H_
#define PICOLIB_SOCKET_IMPL_H_

namespace Pico {

    namespace Network {

        FUNCTION
        uint16_t _htons(uint16_t port)
        {
            return cpu_to_be16(port);
        }

        template <enum AddressType>
        struct Sockaddr;

        template <>
        struct Sockaddr<IPV4>
        {
            static constexpr int family = AF_INET;
            typedef struct sockaddr_in type;

            FUNCTION
            struct sockaddr_in pack(IpAddress const ip, uint16_t port)
            {
                struct sockaddr_in addr;

                addr.sin_family         = family;
                addr.sin_port           = _htons(port);
                addr.sin_addr.s_addr    = ip.value;

                return addr;
            }
        };

        template <>
        struct Sockaddr<IPV6>
        {
            static constexpr int family = AF_INET6;
            typedef struct sockaddr_in6 type;

            FUNCTION
            struct sockaddr_in6 pack(IpAddress6 const ip, uint16_t port)
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
        int Socket::get(int level, int optname, void *val, unsigned *len)
        {
            return Syscall::getsockopt(fd, level, optname, val, len);
        }

        METHOD
        int Socket::set(int level, int optname, void *val, unsigned len)
        {
            return Syscall::setsockopt(fd, level, optname, val, len);
        }

        template <enum AddressType T>
        METHOD
        int StreamSocket::connect(Address<T> addr, uint16_t port)
        {
            auto serv_addr = Sockaddr<T>::pack(addr, port);
            
            return Syscall::connect(fd, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr));
        }

        FUNCTION
        int bind_socket(Socket &sock, struct sockaddr *bind_addr, socklen_t addr_len, bool reuse_addr)
        {
            if ( reuse_addr )
            {
                int option = 1;
                sock.set(SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
            }

            return Syscall::bind(sock.file_desc(), bind_addr, addr_len);
        }

        template <enum AddressType T>
        METHOD
        int StreamSocket::listen(Address<T> addr, uint16_t port, bool reuse_addr)
        {
            auto serv_addr = Sockaddr<T>::pack(addr, port);

            bind_socket(*this, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr), reuse_addr);
            return Syscall::listen(fd, 1);
        }
    }
}

#endif
