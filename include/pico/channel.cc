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

namespace Pico {

    enum channel_mode
    {
        NO_CHANNEL,
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

    template <enum channel_mode>
    struct ChannelMode;

    #define DEFINE_CHANNEL_MODE(mode, type, dupable)            \
        template<>                                              \
        struct ChannelMode<mode>                                \
        {                                                       \
            typedef type stream_type;                           \
            static constexpr bool dupable_to_stdio = dupable;   \
        };                                                      \

    DEFINE_CHANNEL_MODE(NO_CHANNEL,     int,                    false);
    DEFINE_CHANNEL_MODE(USE_STDOUT,     BiStream<Stream>,       false);
    DEFINE_CHANNEL_MODE(USE_STDERR,     BiStream<Stream>,       false);
    DEFINE_CHANNEL_MODE(TCP_CONNECT,    Network::TcpSocket,     true);
    DEFINE_CHANNEL_MODE(TCP6_CONNECT,   Network::Tcp6Socket,    true);
    DEFINE_CHANNEL_MODE(TCP_LISTEN,     Network::TcpSocket,     true);
    DEFINE_CHANNEL_MODE(TCP6_LISTEN,    Network::Tcp6Socket,    true);
    DEFINE_CHANNEL_MODE(SCTP_CONNECT,   Network::SctpSocket,    true);
    DEFINE_CHANNEL_MODE(SCTP6_CONNECT,  Network::Sctp6Socket,   true);
    DEFINE_CHANNEL_MODE(SCTP_LISTEN,    Network::SctpSocket,    true);
    DEFINE_CHANNEL_MODE(SCTP6_LISTEN,   Network::Sctp6Socket,   true);

    /* XXX: WIP
    template <enum channel_mode M>
    struct Channel
    {
        typename ChannelMode<M>::stream_type stm;

        static constexpr bool dupable_to_stdio = ChannelMode<M>::dupable_to_stdio;
        CONSTRUCTOR Channel();

        template <enum Network::AddressType T>
        CONSTRUCTOR Channel(Network::Address<T> addr, uint16_t port);

        METHOD Channel& recv(void *buf, size_t count) {
            stm.read(buf, count);
            return *this;
        }
        METHOD Channel& send(const void *buf, size_t count) {
            stm.write(buf, count);
            return *this;
        }
    };

    template<>
    CONSTRUCTOR
    Channel<USE_STDOUT>::Channel() :
        stm(0, 1) {}

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<TCP_CONNECT>::Channel(Network::Address<T> addr, uint16_t port) {
        stm.connect(addr, port); 
    } */
}

struct Channel
{
    int rx;
    int tx;
    bool dupable_to_stdout = true;

    CONSTRUCTOR Channel();
    METHOD int recv(void *buf, size_t count);
    METHOD int send(void *buf, size_t count);
};



#include "socket.cc"
#include "fs.cc"

CONSTRUCTOR
Channel::Channel()
{
    const ip_addr_t host = { .ip6 = HOST };
    const ip_port_t port = PORT;

    switch ( CHANNEL )
    {
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
