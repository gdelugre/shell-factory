#ifndef _SOCKET_HELPER_H
#define _SOCKET_HELPER_H

#include <factory.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "io.c"

#ifndef CHANNEL
#define CHANNEL NO_CHANNEL
#endif

#define UNDEFINED_HOST -1
#define UNDEFINED_PORT 0

#ifndef HOST
#define HOST UNDEFINED_HOST
#endif

#ifndef PORT
#define PORT UNDEFINED_PORT
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
    USE_STDOUT,
    USE_STDERR,
};

SYSTEM_CALL
int _socket(int domain, int type, int protocol)
{
    return INTERNAL_SYSCALL(socket,, 3, domain, type, protocol);
}

SYSTEM_CALL
int _connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    return INTERNAL_SYSCALL(connect,, 3, sockfd, addr, addrlen);
}

SYSTEM_CALL
int _listen(int socket, int backlog)
{
    return INTERNAL_SYSCALL(listen,, 2, socket, backlog);
}

SYSTEM_CALL
int _bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    return INTERNAL_SYSCALL(bind,, 3, sockfd, addr, addrlen);
}

SYSTEM_CALL
int _accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    return INTERNAL_SYSCALL(accept,, 3, sockfd, addr, addrlen);
}

FUNCTION
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

FUNCTION
int tcp_connect(const long addr, const short port)
{
    const unsigned long host_addr = HOST;
    const unsigned short host_port = PORT;

    _Static_assert(CHANNEL != TCP_CONNECT || host_addr != UNDEFINED_HOST, "Must specify an address to connect to.\n");
    _Static_assert(CHANNEL != TCP_CONNECT || host_port != UNDEFINED_PORT, "Must specify a port to connect to.\n");

    struct sockaddr_in serv_addr;
    int sock = _socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = host_port;
    serv_addr.sin_addr.s_addr = host_addr;

    if ( _connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
        return -1;

    return sock;
}

FUNCTION
int tcp_listen(const long addr, const short port)
{
    const unsigned long host_addr = HOST;
    const unsigned short host_port = PORT;

    _Static_assert(CHANNEL != TCP_LISTEN || host_addr != UNDEFINED_HOST, "Must specify an address to listen to.\n");
    _Static_assert(CHANNEL != TCP_LISTEN || host_port != UNDEFINED_PORT, "Must specify a port to listen to.\n");

    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sock, listen_sock = _socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = host_port;
    serv_addr.sin_addr.s_addr = host_addr;    

    if ( _bind(listen_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) )
        return -1;

    _listen(listen_sock, 0);

    client_sock = _accept(listen_sock, (struct sockaddr *) &client_addr, &client_len);
    _close(listen_sock);

    return client_sock;
}

FUNCTION
struct channel get_communication_channel()
{
    int sock;
    struct channel chan;

    switch ( CHANNEL )
    {
        case REUSE_SOCKET:
            chan.rx = chan.tx = find_open_socket();
            break;

        case TCP_CONNECT:
            chan.rx = chan.tx = tcp_connect(HOST, PORT);
            break;

        case TCP_LISTEN:
            chan.rx = chan.tx = tcp_listen(HOST, PORT);
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
