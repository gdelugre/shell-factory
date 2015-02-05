#ifndef _CHANNEL_HELPER_H
#define _CHANNEL_HELPER_H

/*
 * Channel parameter defaults to NO_CHANNEL.
 */
#ifndef CHANNEL
#define CHANNEL NO_CHANNEL
#endif

/*
 * HOST parameter.
 * Used for socket channels.
 */
#ifndef HOST
    #define HOST            {0}
    #define UNDEFINED_HOST  1
#else
    #define UNDEFINED_HOST  0
#endif

/*
 * PORT parameter.
 * Used for socket channels.
 */
#ifndef PORT
    #define PORT            0
    #define UNDEFINED_PORT  1
#else
    #define UNDEFINED_PORT  0
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

#include "socket.c"
#include "io.c"

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
        nr_read = _read(chan.rx, (char *) buf + count - bytes_left, count);
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
