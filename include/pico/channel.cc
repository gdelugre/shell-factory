#ifndef CHANNEL_HELPER_H_
#define CHANNEL_HELPER_H_

/* Type traits can be included since they rely exclusively on standard headers and no external linking. */
#include <type_traits>

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

    template <enum channel_mode M>
    struct Channelng
    {
        static_assert(M != NO_CHANNEL, "Cannot instanciate channel: no mode specified");
        typename ChannelMode<M>::stream_type stm;

        static constexpr bool dupable_to_stdio = ChannelMode<M>::dupable_to_stdio;
        CONSTRUCTOR Channelng();

        template <enum Network::AddressType T>
        CONSTRUCTOR Channelng(Network::Address<T> addr, uint16_t port);

        METHOD Channelng& recv(void *buf, size_t count) {
            stm.read(buf, count);
            return *this;
        }
        METHOD Channelng& send(const void *buf, size_t count) {
            stm.write(buf, count);
            return *this;
        }
    };

    template<>
    CONSTRUCTOR
    Channelng<USE_STDOUT>::Channelng() :
        stm(Stream::standard_input(), Stream::standard_output()) {}

    template<>
    CONSTRUCTOR
    Channelng<USE_STDERR>::Channelng() :
        stm(Stream::standard_input(), Stream::standard_error()) {}

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channelng<TCP_CONNECT>::Channelng(Network::Address<T> addr, uint16_t port) {
        static_assert(T == Network::IPV4, "TCP_CONNECT requires an IPV4 address.");
        stm.connect(addr, port); 
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channelng<TCP6_CONNECT>::Channelng(Network::Address<T> addr, uint16_t port) {
        static_assert(T == Network::IPV6, "TCP6_CONNECT requires an IPV6 address.");
        stm.connect(addr, port); 
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channelng<TCP_LISTEN>::Channelng(Network::Address<T> addr, uint16_t port) :
        stm(Network::SocketServer<Network::TcpSocket>::start(addr, port, Options::reuse_addr, Options::fork_on_accept))
    {
        static_assert(T == Network::IPV4, "TCP_LISTEN requires an IPV4 address.");
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channelng<TCP6_LISTEN>::Channelng(Network::Address<T> addr, uint16_t port) :
        stm(Network::SocketServer<Network::Tcp6Socket>::start(addr, port, Options::reuse_addr, Options::fork_on_accept))
    {
        static_assert(T == Network::IPV6, "TCP6_LISTEN requires an IPV6 address.");
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channelng<SCTP_CONNECT>::Channelng(Network::Address<T> addr, uint16_t port) {
        static_assert(T == Network::IPV4, "SCTP_CONNECT requires an IPV4 address.");
        stm.connect(addr, port); 
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channelng<SCTP6_CONNECT>::Channelng(Network::Address<T> addr, uint16_t port) {
        static_assert(T == Network::IPV6, "SCTP6_CONNECT requires an IPV6 address.");
        stm.connect(addr, port); 
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channelng<SCTP_LISTEN>::Channelng(Network::Address<T> addr, uint16_t port) :
        stm(Network::SocketServer<Network::SctpSocket>::start(addr, port, Options::reuse_addr, Options::fork_on_accept))
    {
        static_assert(T == Network::IPV4, "SCTP_LISTEN requires an IPV4 address.");
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channelng<SCTP6_LISTEN>::Channelng(Network::Address<T> addr, uint16_t port) :
        stm(Network::SocketServer<Network::Sctp6Socket>::start(addr, port, Options::reuse_addr, Options::fork_on_accept))
    {
        static_assert(T == Network::IPV6, "SCTP6_LISTEN requires an IPV6 address.");
    }
}

namespace Options {

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
    #define HOST            0,0,0,0
    #endif

    /*
     * PORT parameter.
     * Used for socket channels.
     */
    #ifndef PORT
    #define PORT            0
    #endif

    FUNCTION auto channel()
    {
        using Mode = ChannelMode<CHANNEL>;

        if ( std::is_base_of<Network::Socket, Mode::stream_type>::value )
        {
            uint16_t port = PORT;
            auto address = Network::ip_address_from_bytes(HOST);

            return Channelng<CHANNEL>(address, port); 
        }
        else
            return Channelng<CHANNEL>();
    }
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

CONSTRUCTOR
Channel::Channel()
{
    const ip_addr_t host = { .ip6 = { HOST } };
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
