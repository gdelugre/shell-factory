#ifndef _SOCKET_HELPER_H
#define _SOCKET_HELPER_H

#include <factory.h>

#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/net.h>

/* Syscalls defined in this file. */
SYSTEM_CALL int _socket(int, int, int);
SYSTEM_CALL int _getsockopt(int, int, int , void *, socklen_t *);
SYSTEM_CALL int _setsockopt(int, int, int, const void *, socklen_t);
SYSTEM_CALL int _connect(int, const struct sockaddr *, socklen_t);
SYSTEM_CALL int _listen(int, int);
SYSTEM_CALL int _bind(int, const struct sockaddr *, socklen_t);
SYSTEM_CALL int _accept(int, struct sockaddr *, socklen_t *);

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

#ifndef CHANNEL
#define CHANNEL NO_CHANNEL
#endif

#ifndef HOST
    #define HOST            {0}
    #define UNDEFINED_HOST  1
#else
    #define UNDEFINED_HOST  0
#endif

#ifndef PORT
    #define PORT            0
    #define UNDEFINED_PORT  1
#else
    #define UNDEFINED_PORT  0
#endif

#ifndef FORK_ON_ACCEPT
    #define FORK_ON_ACCEPT  0
#endif

#ifndef REUSE_ADDR
    #define REUSE_ADDR      0
#endif

struct channel
{
    int rx;
    int tx;
};

enum channel_mode
{
    NO_CHANNEL,
    REUSE_SOCKET,
    TCP_CONNECT,
    TCP_LISTEN,
    TCP6_CONNECT,
    TCP6_LISTEN,
    SCTP_CONNECT,
    SCTP_LISTEN,
    SCTP6_CONNECT,
    SCTP6_LISTEN,
    USE_STDOUT,
    USE_STDERR,
};

#include "cpu.c"
#include "string.c"
#include "io.c"
#include "process.c"

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

#if SYSCALL_EXISTS(socketcall)
SYSTEM_CALL
int _socketcall(int num, long *args)
{
    return DO_SYSCALL(socketcall, 2, num, args);
}
#endif

SYSTEM_CALL
int _socket(int domain, int type, int protocol)
{
#if SYSCALL_EXISTS(socket)
    return DO_SYSCALL(socket, 3, domain, type, protocol);
#else
    long args[] = { domain, type, protocol };
    return _socketcall(SYS_SOCKET, args);
#endif
}

SYSTEM_CALL
int _getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
#if SYSCALL_EXISTS(getsockopt)
    return DO_SYSCALL(getsockopt, 5, sockfd, level, optname, optval, optlen);
#else
    long args[] = { sockfd, level, optname, (long) optval, (long) optlen };
    return _socketcall(SYS_GETSOCKOPT, args);
#endif
}

SYSTEM_CALL
int _setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
#if SYSCALL_EXISTS(setsockopt)
    return DO_SYSCALL(setsockopt, 5, sockfd, level, optname, optval, optlen);
#else
    long args[] = { sockfd, level, optname, (long) optval, optlen };
    return _socketcall(SYS_SETSOCKOPT, args);
#endif
}

SYSTEM_CALL
int _connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
#if SYSCALL_EXISTS(connect)
    return DO_SYSCALL(connect, 3, sockfd, addr, addrlen);
#else
    long args[] = { sockfd, (long) addr, addrlen };
    return _socketcall(SYS_CONNECT, args);
#endif
}

SYSTEM_CALL
int _listen(int socket, int backlog)
{
#if SYSCALL_EXISTS(listen)
    return DO_SYSCALL(listen, 2, socket, backlog);
#else
    long args[] = { socket, backlog };
    return _socketcall(SYS_LISTEN, args);
#endif
}

SYSTEM_CALL
int _bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
#if SYSCALL_EXISTS(bind)
    return DO_SYSCALL(bind, 3, sockfd, addr, addrlen);
#else
    long args[] = { sockfd, (long) addr, addrlen };
    return _socketcall(SYS_BIND, args);
#endif
}

SYSTEM_CALL
int _accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
#if SYSCALL_EXISTS(accept)
    return DO_SYSCALL(accept, 3, sockfd, addr, addrlen);
#else
    long args[] = { sockfd, (long) addr, (long) addrlen };
    return _socketcall(SYS_ACCEPT, args);
#endif
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
    int                 sock = _socket(AF_INET, SOCK_STREAM, protocol);

    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_port          = _htons(host_port);
    serv_addr.sin_addr.s_addr   = _inet_addr(host_addr);

    if ( _connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
        return -1;

    return sock;
}

FUNCTION
int sock6_stream_connect(int protocol, const ip_addr_t host_addr, const ip_port_t host_port)
{
    struct sockaddr_in6 serv_addr;
    int                 sock = _socket(AF_INET6, SOCK_STREAM, protocol);

    serv_addr.sin6_flowinfo = 0;
    serv_addr.sin6_family   = AF_INET6;
    serv_addr.sin6_port     = _htons(host_port);
    _inet6_addr(host_addr, &serv_addr);

    if ( _connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
       return -1;

    return sock; 
}

FUNCTION
int tcp_connect(const ip_addr_t host_addr, const ip_port_t host_port)
{
    _Static_assert(CHANNEL != TCP_CONNECT || !UNDEFINED_HOST, "Must specify an address to connect to.\n");
    _Static_assert(CHANNEL != TCP_CONNECT || !UNDEFINED_PORT, "Must specify a port to connect to.\n");

    return sock_stream_connect(IPPROTO_IP, host_addr, host_port);
}

FUNCTION
int tcp6_connect(const ip_addr_t host_addr, const ip_port_t host_port)
{
    _Static_assert(CHANNEL != TCP6_CONNECT || !UNDEFINED_HOST, "Must specify an address to connect to.\n");
    _Static_assert(CHANNEL != TCP6_CONNECT || !UNDEFINED_PORT, "Must specify a port to connect to.\n");

    return sock6_stream_connect(IPPROTO_IP, host_addr, host_port);    
}

FUNCTION
int sctp_connect(const ip_addr_t host_addr, const ip_port_t host_port)
{
    _Static_assert(CHANNEL != SCTP_CONNECT || !UNDEFINED_HOST, "Must specify an address to connect to.\n");
    _Static_assert(CHANNEL != SCTP_CONNECT || !UNDEFINED_PORT, "Must specify a port to connect to.\n");

    return sock_stream_connect(IPPROTO_SCTP, host_addr, host_port);
}

FUNCTION
int sctp6_connect(const ip_addr_t host_addr, const ip_port_t host_port)
{
    _Static_assert(CHANNEL != SCTP6_CONNECT || !UNDEFINED_HOST, "Must specify an address to connect to.\n");
    _Static_assert(CHANNEL != SCTP6_CONNECT || !UNDEFINED_PORT, "Must specify a port to connect to.\n");

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
        _setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    }

    if ( _bind(listen_sock, serv_addr, addr_len) )
        return -1;

    _listen(listen_sock, 1);

    if ( FORK_ON_ACCEPT )
    {
        while ( true ) 
        {
            client_sock = _accept(listen_sock, (struct sockaddr *) &client_addr, &client_len);
            if ( _fork() == 0 )
                break;
            else
                _close(client_sock);
        }
    }
    else
        client_sock = _accept(listen_sock, (struct sockaddr *) &client_addr, &client_len);

    _close(listen_sock);
    return client_sock;
}

FUNCTION
int sock_stream_listen(int protocol, const ip_addr_t host_addr, const ip_port_t host_port)
{
    struct sockaddr_in  serv_addr, client_addr;
    int                 listen_sock = _socket(AF_INET, SOCK_STREAM, protocol);

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
    int                 listen_sock = _socket(AF_INET6, SOCK_STREAM, protocol);

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
    _Static_assert(CHANNEL != TCP_LISTEN || !UNDEFINED_HOST, "Must specify an address to listen to.\n");
    _Static_assert(CHANNEL != TCP_LISTEN || !UNDEFINED_PORT, "Must specify a port to listen to.\n");

    return sock_stream_listen(IPPROTO_IP, host_addr, host_port);
}

FUNCTION
int tcp6_listen(const ip_addr_t host_addr, const ip_port_t host_port)
{
    _Static_assert(CHANNEL != TCP6_LISTEN || !UNDEFINED_HOST, "Must specify an address to listen to.\n");
    _Static_assert(CHANNEL != TCP6_LISTEN || !UNDEFINED_PORT, "Must specify a port to listen to.\n");

    return sock6_stream_listen(IPPROTO_IP, host_addr, host_port);
}

FUNCTION
int sctp_listen(const ip_addr_t host_addr, const ip_port_t host_port)
{
    _Static_assert(CHANNEL != SCTP_LISTEN || !UNDEFINED_HOST, "Must specify an address to listen to.\n");
    _Static_assert(CHANNEL != SCTP_LISTEN || !UNDEFINED_PORT, "Must specify a port to listen to.\n");

    return sock_stream_listen(IPPROTO_SCTP, host_addr, host_port);
}

FUNCTION 
int sctp6_listen(const ip_addr_t host_addr, const ip_port_t host_port)
{
    _Static_assert(CHANNEL != SCTP6_LISTEN || !UNDEFINED_HOST, "Must specify an address to listen to.\n");
    _Static_assert(CHANNEL != SCTP6_LISTEN || !UNDEFINED_PORT, "Must specify a port to listen to.\n");

    return sock6_stream_listen(IPPROTO_SCTP, host_addr, host_port);
}

FUNCTION
struct channel get_communication_channel()
{
    struct channel chan;
    const ip_addr_t host = { .ip6 = HOST };
    const ip_port_t port = PORT;

    switch ( CHANNEL )
    {
        case REUSE_SOCKET:
            chan.rx = chan.tx = find_open_socket();
            break;

        case TCP_CONNECT:
            chan.rx = chan.tx = tcp_connect(host, port);
            break;

        case TCP6_CONNECT:
            chan.rx = chan.tx = tcp6_connect(host, port);
            break;

        case TCP_LISTEN:
            chan.rx = chan.tx = tcp_listen(host, port);
            break;
        
        case TCP6_LISTEN:
            chan.rx = chan.tx = tcp6_listen(host, port);
            break;

        case SCTP_CONNECT:
            chan.rx = chan.tx = sctp_connect(host, port);
            break;

        case SCTP6_CONNECT:
            chan.rx = chan.tx = sctp6_connect(host, port);
            break;

        case SCTP_LISTEN:
            chan.rx = chan.tx = sctp_listen(host, port);
            break;

        case SCTP6_LISTEN:
            chan.rx = chan.tx = sctp6_listen(host, port);
            break;

        case USE_STDOUT:
            chan.rx = 0;
            chan.tx = 1;
            break;

        case USE_STDERR:
            chan.rx = 0;
            chan.tx = 2;
            break;

        case NO_CHANNEL:
        default:
            chan.rx = chan.tx = -1;
            break;
    }

    return chan;
}

FUNCTION
int channel_recv(struct channel chan, void *buf, size_t count)
{
    size_t bytes_left = count;
    ssize_t nr_read;

    while ( bytes_left > 0 )
    {
        nr_read = _read(chan.rx, buf + count - bytes_left, count);
        if ( nr_read < 0 )
            return -1;

        bytes_left -= nr_read; 
    }

    return 0;
}

FUNCTION
int channel_send(struct channel chan, void *buf, size_t count)
{
    return _write(chan.tx, buf, count);
}

#endif
