#ifndef CHANNEL_HELPER_H_
#define CHANNEL_HELPER_H_

/* Type traits can be included since they rely exclusively on standard headers and no external linking. */
#include <type_traits>

namespace Pico {

    //
    // The list of defined channel modes.
    //
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

    //
    // Static structure holding information for each mode.
    //
    template <enum channel_mode>
    struct ChannelMode;

    #define DEFINE_CHANNEL_MODE(mode, type, dupable)            \
        template<>                                              \
        struct ChannelMode<mode>                                \
        {                                                       \
            using stream_type = type;                           \
            static constexpr bool dupable_to_stdio = dupable;   \
        };                                                      \

    DEFINE_CHANNEL_MODE(NO_CHANNEL,     void,                   false);
    DEFINE_CHANNEL_MODE(USE_STDOUT,     BiStream<BasicStream>,  false);
    DEFINE_CHANNEL_MODE(USE_STDERR,     BiStream<BasicStream>,  false);
    DEFINE_CHANNEL_MODE(TCP_CONNECT,    Network::TcpSocket,     true);
    DEFINE_CHANNEL_MODE(TCP6_CONNECT,   Network::Tcp6Socket,    true);
    DEFINE_CHANNEL_MODE(TCP_LISTEN,     Network::TcpSocket,     true);
    DEFINE_CHANNEL_MODE(TCP6_LISTEN,    Network::Tcp6Socket,    true);
    DEFINE_CHANNEL_MODE(SCTP_CONNECT,   Network::SctpSocket,    true);
    DEFINE_CHANNEL_MODE(SCTP6_CONNECT,  Network::Sctp6Socket,   true);
    DEFINE_CHANNEL_MODE(SCTP_LISTEN,    Network::SctpSocket,    true);
    DEFINE_CHANNEL_MODE(SCTP6_LISTEN,   Network::Sctp6Socket,   true);

    //
    // The channel class definition.
    // Every channel is a daughter class of the underlying stream it is representing.
    //
    template <enum channel_mode M>
    struct Channel : ChannelMode<M>::stream_type
    {
        static_assert(M != NO_CHANNEL, "Cannot instanciate channel: no mode specified");

        CONSTRUCTOR Channel();

        template <enum Network::AddressType T>
        CONSTRUCTOR Channel(Network::Address<T> addr, uint16_t port);

        METHOD void dup_to_stdio() {
            if ( ChannelMode<M>::dupable_to_stdio )
            {
                BasicStream std_in = Stdio::input();
                BasicStream std_out = Stdio::output();
                BasicStream std_err = Stdio::error();

                this->duplicate(std_in);
                this->duplicate(std_out);
                this->duplicate(std_err);
            }
        }
    };

    template<>
    CONSTRUCTOR
    Channel<USE_STDOUT>::Channel() :
        ChannelMode<USE_STDOUT>::stream_type(Stdio::input(), Stdio::output()) {}

    template<>
    CONSTRUCTOR
    Channel<USE_STDERR>::Channel() :
        ChannelMode<USE_STDERR>::stream_type(Stdio::input(), Stdio::error()) {}

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<TCP_CONNECT>::Channel(Network::Address<T> addr, uint16_t port) {
        static_assert(T == Network::IPV4, "TCP_CONNECT requires an IPV4 address.");
        connect(addr, port);
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<TCP6_CONNECT>::Channel(Network::Address<T> addr, uint16_t port) {
        static_assert(T == Network::IPV6, "TCP6_CONNECT requires an IPV6 address.");
        connect(addr, port);
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<TCP_LISTEN>::Channel(Network::Address<T> addr, uint16_t port) :
        TcpSocket(Network::SocketServer<Network::TcpSocket, Options::fork_on_accept>::start(addr, port, Options::reuse_addr))
    {
        static_assert(T == Network::IPV4, "TCP_LISTEN requires an IPV4 address.");
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<TCP6_LISTEN>::Channel(Network::Address<T> addr, uint16_t port) :
        Tcp6Socket(Network::SocketServer<Network::Tcp6Socket, Options::fork_on_accept>::start(addr, port, Options::reuse_addr))
    {
        static_assert(T == Network::IPV6, "TCP6_LISTEN requires an IPV6 address.");
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<SCTP_CONNECT>::Channel(Network::Address<T> addr, uint16_t port) {
        static_assert(T == Network::IPV4, "SCTP_CONNECT requires an IPV4 address.");
        connect(addr, port);
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<SCTP6_CONNECT>::Channel(Network::Address<T> addr, uint16_t port) {
        static_assert(T == Network::IPV6, "SCTP6_CONNECT requires an IPV6 address.");
        connect(addr, port);
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<SCTP_LISTEN>::Channel(Network::Address<T> addr, uint16_t port) :
        SctpSocket(Network::SocketServer<Network::SctpSocket, Options::fork_on_accept>::start(addr, port, Options::reuse_addr))
    {
        static_assert(T == Network::IPV4, "SCTP_LISTEN requires an IPV4 address.");
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<SCTP6_LISTEN>::Channel(Network::Address<T> addr, uint16_t port) :
        Sctp6Socket(Network::SocketServer<Network::Sctp6Socket, Options::fork_on_accept>::start(addr, port, Options::reuse_addr))
    {
        static_assert(T == Network::IPV6, "SCTP6_LISTEN requires an IPV6 address.");
    }
}

namespace Options {

    #ifndef CHANNEL
    #define CHANNEL USE_STDOUT
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
        using namespace Pico;
        using SelectedChannel = Channel<CHANNEL>;

        switch ( CHANNEL )
        {
            case USE_STDOUT:
            case USE_STDERR:
                return SelectedChannel();

            case TCP_CONNECT:
            case TCP6_CONNECT:
            case SCTP_CONNECT:
            case SCTP6_CONNECT:
            {
                auto remote_address = Network::ip_address_from_bytes(HOST);
                uint16_t remote_port = PORT;

                return SelectedChannel(remote_address, remote_port);
            }

            case TCP_LISTEN:
            case TCP6_LISTEN:
            case SCTP_LISTEN:
            case SCTP6_LISTEN:
            {
                auto local_address = Network::ip_address_from_bytes(HOST);
                uint16_t local_port = PORT;

                return SelectedChannel(local_address, local_port);
            }
        }
    }
}

#endif
