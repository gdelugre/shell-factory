#ifndef PICOLIB_SOCKET_H_
#define PICOLIB_SOCKET_H_

namespace Pico {

    namespace Network {

        enum AddressType {
            IPV4,
            IPV6,
            UNIX,
            UNIX_ABSTRACT,
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

        template <>
        struct Address<UNIX>
        {
            char *path;
        };

        template <>
        struct Address<UNIX_ABSTRACT>
        {
            char *path;
        };

        using IpAddress             = Address<IPV4>;
        using IpAddress6            = Address<IPV6>;
        using UnixAddress           = Address<UNIX>;
        using UnixAbstractAddress   = Address<UNIX_ABSTRACT>;

        template <size_t N>
        struct ip_address_type;

        template <>
        struct ip_address_type<4>
        {
            using type = IpAddress;
        };

        template <>
        struct ip_address_type<16>
        {
            using type = IpAddress6;
        };

        // Generates an IP address structure out of a list of static arguments.
        template <typename... V> 
        FUNCTION
        auto ip_address_from_bytes(V... bytes)
        {
            constexpr size_t nr_bytes = sizeof...(bytes);
            static_assert(nr_bytes == 4 || nr_bytes == 16, "Invalid number of bytes for IP address.");

            using ip_address = typename ip_address_type<nr_bytes>::type;

            return ip_address { { { static_cast<uint8_t>(bytes)... } } };
        }

        class SocketIO : public SingleIO
        {
            public:
                using SingleIO::SingleIO;
                METHOD ssize_t in(void *, size_t);
                METHOD ssize_t out(const void *, size_t);
        };

        class Socket : public Stream<SocketIO>
        {
            public:
                using Stream<SocketIO>::Stream; // inherit Stream constructor.

                CONSTRUCTOR Socket(int domain, int type, int protocol);
                METHOD int get(int level, int optname, void *val, unsigned *len);
                METHOD int set(int level, int optname, void *val, unsigned len);

                template <AddressType T>
                METHOD int connect(Address<T> addr);

                template <AddressType T>
                METHOD int connect(Address<T> addr, uint16_t port);

                template <enum AddressType T>
                METHOD int bind(Address<T> addr);

                template <enum AddressType T>
                METHOD int bind(Address<T> addr, uint16_t port, bool reuse_addr);
        };

        // TODO: recvfrom / sendto
        class DatagramSocket : public Socket
        {
            public:
                using Socket::Socket;
                CONSTRUCTOR DatagramSocket(int domain, int protocol) : Socket(domain, SOCK_DGRAM, protocol) {}
        };

        class UdpSocket : public DatagramSocket
        {
            public:
                using DatagramSocket::DatagramSocket;
                CONSTRUCTOR UdpSocket() : DatagramSocket(AF_INET, IPPROTO_IP) {}
        };

        class Udp6Socket : public DatagramSocket
        {
            public:
                using DatagramSocket::DatagramSocket;
                CONSTRUCTOR Udp6Socket() : DatagramSocket(AF_INET6, IPPROTO_IP) {}
        };

        class StreamSocket : public Socket
        {
            public:
                using Socket::Socket;

                CONSTRUCTOR StreamSocket(int domain, int protocol) : Socket(domain, SOCK_STREAM, protocol) {}

                METHOD int listen(int backlog);

                template <bool Fork = false>
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

        class UnixStreamSocket : public StreamSocket
        {
            public:
                using StreamSocket::StreamSocket;
                CONSTRUCTOR UnixStreamSocket() : StreamSocket(AF_UNIX, 0) {}

                METHOD int send_io(SingleIO const& io);
                METHOD SingleIO recv_io();
        };

        // TODO: consistency check between S and T.
        template <class SockType, bool Fork = false>
        class SocketServer
        {
            public:
                template <AddressType T>
                FUNCTION SockType start(Address<T> addr)
                {
                    SockType server;
                    server.bind(addr);
                    server.listen(1);

                    SockType client = SockType(server.template accept<Fork>().file_desc());
                    return client;
                }

                template <AddressType T>
                FUNCTION SockType start(Address<T> addr, uint16_t port, bool reuse_addr = false)
                {
                    SockType server;
                    server.bind(addr, port, reuse_addr);
                    server.listen(1);

                    SockType client = SockType(server.template accept<Fork>().file_desc());
                    return client;
                }
        };
    }
}

#endif
