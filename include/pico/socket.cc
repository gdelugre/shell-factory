#ifndef PICOLIB_SOCKET_H_
#define PICOLIB_SOCKET_H_

namespace Pico {

    namespace Network {

        enum AddressType {
            IPV4,
            IPV6,
        };

        template <enum AddressType>
        struct Address;

        template <>
        struct Address<IPV4>
        {
            union {
                uint8_t bytes[4];
                uint32_t value;
            };
        };

        template <>
        struct Address<IPV6>
        {
            uint8_t bytes[16];
        };

        typedef Address<IPV4> IpAddress;
        typedef Address<IPV6> IpAddress6;

        template <size_t N>
        struct ip_address_type;

        template <>
        struct ip_address_type<4>
        {
            typedef IpAddress type;
        };

        template <>
        struct ip_address_type<16>
        {
            typedef IpAddress6 type;
        };

        // Generates an IP address structure out of a list of static arguments.
        template <typename... V> static constexpr
        auto ip_address_from_bytes(V... bytes)
        {
            constexpr size_t nr_bytes = sizeof...(bytes);
            static_assert(nr_bytes == 4 || nr_bytes == 16, "Invalid number of bytes for IP address.");

            using ip_address = typename ip_address_type<nr_bytes>::type;

            return ip_address { static_cast<uint8_t>(bytes)... };
        }

        class Socket : public Stream
        {
            public:
                using Stream::Stream; // inherit Stream constructor.

                CONSTRUCTOR Socket(int domain, int type, int protocol);
                METHOD int get(int level, int optname, void *val, unsigned *len);
                METHOD int set(int level, int optname, void *val, unsigned len);

                METHOD ssize_t in(void *buf, size_t count);
                METHOD ssize_t out(const void *buf, size_t count);
        };

        class StreamSocket : public Socket
        {
            public:
                using Socket::Socket;

                CONSTRUCTOR StreamSocket(int domain, int protocol) : Socket(domain, SOCK_STREAM, protocol) {}

                template <AddressType T>
                METHOD int connect(Address<T> addr, uint16_t port);

                template <AddressType T>
                METHOD int listen(Address<T> addr, uint16_t port, bool reuse_addr = false);

                METHOD StreamSocket accept();
        };

        class TcpSocket : public StreamSocket
        {
            public:
                using StreamSocket::StreamSocket;
                CONSTRUCTOR TcpSocket() : StreamSocket(AF_INET, IPPROTO_IP) {}
        };

        class Tcp6Socket : public StreamSocket
        {
            public:
                using StreamSocket::StreamSocket;
                CONSTRUCTOR Tcp6Socket() : StreamSocket(AF_INET6, IPPROTO_IP) {}
        };

        class SctpSocket : public StreamSocket
        {
            public:
                using StreamSocket::StreamSocket;
                CONSTRUCTOR SctpSocket() : StreamSocket(AF_INET, IPPROTO_SCTP) {}
        };

        class Sctp6Socket : public StreamSocket
        {
            public:
                using StreamSocket::StreamSocket;
                CONSTRUCTOR Sctp6Socket() : StreamSocket(AF_INET6, IPPROTO_SCTP) {}
        };

        // TODO: consistency check between S and T.
        // TODO: fork on accept.
        template <class SockType>
        class SocketServer
        {
            public:
                template <AddressType T>
                FUNCTION SockType start(Address<T> addr, uint16_t port, bool reuse_addr = false, bool fork = false)
                {
                    SockType server;
                    server.listen(addr, port, reuse_addr);

                    SockType client = SockType(server.accept().file_desc());
                    return client;
                }
        };
    }
}

#endif
