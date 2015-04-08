#ifndef PICOLIB_SOCKET_H_
#define PICOLIB_SOCKET_H_

#include "cpu.cc"
#include "string.cc"
#include "io.cc"
#include "process.cc"

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

#define IPV4(a,b,c,d) (ipv4_addr_t) { a,b,c,d }
#define IPV6(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) \
    (ipv6_addr_t) { a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p }

#ifndef FORK_ON_ACCEPT
    #define FORK_ON_ACCEPT  0
#endif

#ifndef REUSE_ADDR
    #define REUSE_ADDR      0
#endif

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
int find_open_socket()
{
    for ( int fd = 0; fd != 0xFFFF; fd++ )
    {
        struct stat sb;

        DO_SYSCALL(fstat, 2, fd, &sb);
        if ( S_ISSOCK(sb.st_mode) )
            return fd;
    }

    return -1;
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
    static_assert(CHANNEL != TCP_CONNECT || !UNDEFINED_HOST, "Must specify an address to connect to.\n");
    static_assert(CHANNEL != TCP_CONNECT || !UNDEFINED_PORT, "Must specify a port to connect to.\n");

    return sock_stream_connect(IPPROTO_IP, host_addr, host_port);
}

FUNCTION
int tcp6_connect(const ip_addr_t host_addr, const ip_port_t host_port)
{
    static_assert(CHANNEL != TCP6_CONNECT || !UNDEFINED_HOST, "Must specify an address to connect to.\n");
    static_assert(CHANNEL != TCP6_CONNECT || !UNDEFINED_PORT, "Must specify a port to connect to.\n");

    return sock6_stream_connect(IPPROTO_IP, host_addr, host_port);    
}

FUNCTION
int sctp_connect(const ip_addr_t host_addr, const ip_port_t host_port)
{
    static_assert(CHANNEL != SCTP_CONNECT || !UNDEFINED_HOST, "Must specify an address to connect to.\n");
    static_assert(CHANNEL != SCTP_CONNECT || !UNDEFINED_PORT, "Must specify a port to connect to.\n");

    return sock_stream_connect(IPPROTO_SCTP, host_addr, host_port);
}

FUNCTION
int sctp6_connect(const ip_addr_t host_addr, const ip_port_t host_port)
{
    static_assert(CHANNEL != SCTP6_CONNECT || !UNDEFINED_HOST, "Must specify an address to connect to.\n");
    static_assert(CHANNEL != SCTP6_CONNECT || !UNDEFINED_PORT, "Must specify a port to connect to.\n");

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

    if ( REUSE_ADDR )
    {
        int option = 1;
        Syscall::setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    }

    if ( Syscall::bind(listen_sock, serv_addr, addr_len) )
        return -1;

    Syscall::listen(listen_sock, 1);

    if ( FORK_ON_ACCEPT )
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
    static_assert(CHANNEL != TCP_LISTEN || !UNDEFINED_HOST, "Must specify an address to listen to.\n");
    static_assert(CHANNEL != TCP_LISTEN || !UNDEFINED_PORT, "Must specify a port to listen to.\n");

    return sock_stream_listen(IPPROTO_IP, host_addr, host_port);
}

FUNCTION
int tcp6_listen(const ip_addr_t host_addr, const ip_port_t host_port)
{
    static_assert(CHANNEL != TCP6_LISTEN || !UNDEFINED_HOST, "Must specify an address to listen to.\n");
    static_assert(CHANNEL != TCP6_LISTEN || !UNDEFINED_PORT, "Must specify a port to listen to.\n");

    return sock6_stream_listen(IPPROTO_IP, host_addr, host_port);
}

FUNCTION
int sctp_listen(const ip_addr_t host_addr, const ip_port_t host_port)
{
    static_assert(CHANNEL != SCTP_LISTEN || !UNDEFINED_HOST, "Must specify an address to listen to.\n");
    static_assert(CHANNEL != SCTP_LISTEN || !UNDEFINED_PORT, "Must specify a port to listen to.\n");

    return sock_stream_listen(IPPROTO_SCTP, host_addr, host_port);
}

FUNCTION 
int sctp6_listen(const ip_addr_t host_addr, const ip_port_t host_port)
{
    static_assert(CHANNEL != SCTP6_LISTEN || !UNDEFINED_HOST, "Must specify an address to listen to.\n");
    static_assert(CHANNEL != SCTP6_LISTEN || !UNDEFINED_PORT, "Must specify a port to listen to.\n");

    return sock6_stream_listen(IPPROTO_SCTP, host_addr, host_port);
}

#endif
