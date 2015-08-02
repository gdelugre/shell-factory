#ifndef CHANNEL_HELPER_H_
#define CHANNEL_HELPER_H_

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

struct Channel
{
    int rx;
    int tx;
    bool dupable_to_stdout = true;

    Channel();
    int recv(void *buf, size_t count);
    int send(void *buf, size_t count);
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

#include "socket.cc"
#include "io.cc"

/* XXX: Cannot declare it as METHOD, gcc 5.2 complains about section mismatch ? */
inline
Channel::Channel()
{
    const ip_addr_t host = { .ip6 = HOST };
    const ip_port_t port = PORT;

    switch ( CHANNEL )
    {
        case REUSE_SOCKET:
            this->rx = this->tx = find_open_socket();
            break;

        case TCP_CONNECT:
            this->rx = this->tx = tcp_connect(host, port);
            break;

        case TCP6_CONNECT:
            this->rx = this->tx = tcp6_connect(host, port);
            break;

        case TCP_LISTEN:
            this->rx = this->tx = tcp_listen(host, port);
            break;
        
        case TCP6_LISTEN:
            this->rx = this->tx = tcp6_listen(host, port);
            break;

        case SCTP_CONNECT:
            this->rx = this->tx = sctp_connect(host, port);
            break;

        case SCTP6_CONNECT:
            this->rx = this->tx = sctp6_connect(host, port);
            break;

        case SCTP_LISTEN:
            this->rx = this->tx = sctp_listen(host, port);
            break;

        case SCTP6_LISTEN:
            this->rx = this->tx = sctp6_listen(host, port);
            break;

        case USE_STDOUT:
            this->dupable_to_stdout = false;
            this->rx = 0;
            this->tx = 1;
            break;

        case USE_STDERR:
            this->rx = 0;
            this->tx = 2;
            break;

        case NO_CHANNEL:
        default:
            this->dupable_to_stdout = false;
            this->rx = this->tx = -1;
            break;
    }
}

METHOD
int Channel::recv(void *buf, size_t count)
{
    size_t bytes_left = count;
    ssize_t nr_read;

    while ( bytes_left > 0 )
    {
        nr_read = Syscall::read(this->rx, (char *) buf + count - bytes_left, count);
        if ( nr_read < 0 )
            return -1;

        bytes_left -= nr_read; 
    }

    return 0;
}

METHOD
int Channel::send(void *buf, size_t count)
{
    return Syscall::write(this->tx, buf, count);
}

#endif
