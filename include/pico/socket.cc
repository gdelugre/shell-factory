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
    }
}

typedef union {
    uint8_t bytes[4];
    uint32_t value;
} ipv4_addr_t;

typedef uint8_t ipv6_addr_t[16];
typedef union {
    ipv4_addr_t ip4;
    ipv6_addr_t ip6;
} ip_addr_t;
typedef uint16_t ip_port_t;

static inline
in_addr_t _inet_addr(const ip_addr_t addr)
{
    return addr.ip4.value;
}

static inline
void _inet6_addr(const ip_addr_t addr, struct sockaddr_in6 *saddr)
{
    _memcpy(&saddr->sin6_addr.s6_addr, addr.ip6, sizeof(ipv6_addr_t));
}

static inline
uint16_t _htons(ip_port_t hostport)
{
    return cpu_to_be16(hostport);
}

FUNCTION
int sock_stream_connect(int protocol, const ip_addr_t host_addr, const ip_port_t host_port)
{
    struct sockaddr_in  serv_addr;
    int                 sock = Syscall::socket(AF_INET, SOCK_STREAM, protocol);

    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_port          = _htons(host_port);
    serv_addr.sin_addr.s_addr   = _inet_addr(host_addr);

    if ( Syscall::connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
        return -1;

    return sock;
}

FUNCTION
int sock6_stream_connect(int protocol, const ip_addr_t host_addr, const ip_port_t host_port)
{
    struct sockaddr_in6 serv_addr;
    int                 sock = Syscall::socket(AF_INET6, SOCK_STREAM, protocol);

    serv_addr.sin6_flowinfo = 0;
    serv_addr.sin6_family   = AF_INET6;
    serv_addr.sin6_port     = _htons(host_port);
    _inet6_addr(host_addr, &serv_addr);

    if ( Syscall::connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
       return -1;

    return sock; 
}

FUNCTION
int tcp_connect(const ip_addr_t host_addr, const ip_port_t host_port)
{
    //static_assert(CHANNEL != TCP_CONNECT || !UNDEFINED_HOST, "Must specify an address to connect to.\n");
    //static_assert(CHANNEL != TCP_CONNECT || !UNDEFINED_PORT, "Must specify a port to connect to.\n");

    return sock_stream_connect(IPPROTO_IP, host_addr, host_port);
}

FUNCTION
int tcp6_connect(const ip_addr_t host_addr, const ip_port_t host_port)
{
    //static_assert(CHANNEL != TCP6_CONNECT || !UNDEFINED_HOST, "Must specify an address to connect to.\n");
    //static_assert(CHANNEL != TCP6_CONNECT || !UNDEFINED_PORT, "Must specify a port to connect to.\n");

    return sock6_stream_connect(IPPROTO_IP, host_addr, host_port);    
}

FUNCTION
int sctp_connect(const ip_addr_t host_addr, const ip_port_t host_port)
{
    //static_assert(CHANNEL != SCTP_CONNECT || !UNDEFINED_HOST, "Must specify an address to connect to.\n");
    //static_assert(CHANNEL != SCTP_CONNECT || !UNDEFINED_PORT, "Must specify a port to connect to.\n");

    return sock_stream_connect(IPPROTO_SCTP, host_addr, host_port);
}

FUNCTION
int sctp6_connect(const ip_addr_t host_addr, const ip_port_t host_port)
{
    //static_assert(CHANNEL != SCTP6_CONNECT || !UNDEFINED_HOST, "Must specify an address to connect to.\n");
    //static_assert(CHANNEL != SCTP6_CONNECT || !UNDEFINED_PORT, "Must specify a port to connect to.\n");

    return sock6_stream_connect(IPPROTO_SCTP, host_addr, host_port);
}

FUNCTION
int sock_stream_bind_server(int listen_sock,
                            struct sockaddr *serv_addr,
                            struct sockaddr *client_addr,
                            socklen_t addr_len)
{
    socklen_t   client_len = addr_len;
    int         client_sock;

    if ( Options::reuse_addr )
    {
        int option = 1;
        Syscall::setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    }

    if ( Syscall::bind(listen_sock, serv_addr, addr_len) )
        return -1;

    Syscall::listen(listen_sock, 1);

    if ( Options::fork_on_accept )
    {
        while ( true ) 
        {
            client_sock = Syscall::accept(listen_sock, client_addr, &client_len);
            if ( Syscall::fork() == 0 )
                break;
            else
                Syscall::close(client_sock);
        }
    }
    else
        client_sock = Syscall::accept(listen_sock, client_addr, &client_len);

    Syscall::close(listen_sock);
    return client_sock;
}

FUNCTION
int sock_stream_listen(int protocol, const ip_addr_t host_addr, const ip_port_t host_port)
{
    struct sockaddr_in  serv_addr, client_addr;
    int                 listen_sock = Syscall::socket(AF_INET, SOCK_STREAM, protocol);

    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_port          = _htons(host_port);
    serv_addr.sin_addr.s_addr   = _inet_addr(host_addr);

    return sock_stream_bind_server(listen_sock,
            (struct sockaddr *) &serv_addr,
            (struct sockaddr *) &client_addr,
            sizeof(serv_addr));
}


FUNCTION
int sock6_stream_listen(int protocol, const ip_addr_t host_addr, const ip_port_t host_port)
{
    struct sockaddr_in6 serv_addr, client_addr;
    int                 listen_sock = Syscall::socket(AF_INET6, SOCK_STREAM, protocol);

    serv_addr.sin6_flowinfo     = 0;
    serv_addr.sin6_family       = AF_INET6;
    serv_addr.sin6_port         = _htons(host_port);
    _inet6_addr(host_addr, &serv_addr);

    return sock_stream_bind_server(listen_sock,
            (struct sockaddr *) &serv_addr,
            (struct sockaddr *) &client_addr,
            sizeof(serv_addr));
}

FUNCTION
int tcp_listen(const ip_addr_t host_addr, const ip_port_t host_port)
{
    //static_assert(CHANNEL != TCP_LISTEN || !UNDEFINED_HOST, "Must specify an address to listen to.\n");
    //static_assert(CHANNEL != TCP_LISTEN || !UNDEFINED_PORT, "Must specify a port to listen to.\n");

    return sock_stream_listen(IPPROTO_IP, host_addr, host_port);
}

FUNCTION
int tcp6_listen(const ip_addr_t host_addr, const ip_port_t host_port)
{
    //static_assert(CHANNEL != TCP6_LISTEN || !UNDEFINED_HOST, "Must specify an address to listen to.\n");
    //static_assert(CHANNEL != TCP6_LISTEN || !UNDEFINED_PORT, "Must specify a port to listen to.\n");

    return sock6_stream_listen(IPPROTO_IP, host_addr, host_port);
}

FUNCTION
int sctp_listen(const ip_addr_t host_addr, const ip_port_t host_port)
{
    //static_assert(CHANNEL != SCTP_LISTEN || !UNDEFINED_HOST, "Must specify an address to listen to.\n");
    //static_assert(CHANNEL != SCTP_LISTEN || !UNDEFINED_PORT, "Must specify a port to listen to.\n");

    return sock_stream_listen(IPPROTO_SCTP, host_addr, host_port);
}

FUNCTION 
int sctp6_listen(const ip_addr_t host_addr, const ip_port_t host_port)
{
    //static_assert(CHANNEL != SCTP6_LISTEN || !UNDEFINED_HOST, "Must specify an address to listen to.\n");
    //static_assert(CHANNEL != SCTP6_LISTEN || !UNDEFINED_PORT, "Must specify a port to listen to.\n");

    return sock6_stream_listen(IPPROTO_SCTP, host_addr, host_port);
}

#endif
