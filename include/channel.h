#ifndef CHANNEL_HELPER_H_
#define CHANNEL_HELPER_H_

#include <pico.h>

namespace Shellcode {

    using namespace Pico;

    //
    // The list of defined channel modes.
    //
    #define USE_STDOUT      0
    #define USE_STDERR      1
    #define TCP_CONNECT     2
    #define TCP_LISTEN      3
    #define TCP6_CONNECT    4
    #define TCP6_LISTEN     5
    #define SCTP_CONNECT    6
    #define SCTP_LISTEN     7
    #define SCTP6_CONNECT   8
    #define SCTP6_LISTEN    9

    //
    // Static structure holding information for each mode.
    //
    template <unsigned>
    struct ChannelMode;

    #define DEFINE_CHANNEL_MODE(mode, type)                     \
        template<>                                              \
        struct ChannelMode<mode>                                \
        {                                                       \
            using stream_type = type;                           \
        };                                                      \

    DEFINE_CHANNEL_MODE(USE_STDOUT,     BiStream<BasicStream>);
    DEFINE_CHANNEL_MODE(USE_STDERR,     BiStream<BasicStream>);
    DEFINE_CHANNEL_MODE(TCP_CONNECT,    Network::TcpSocket);
    DEFINE_CHANNEL_MODE(TCP6_CONNECT,   Network::Tcp6Socket);
    DEFINE_CHANNEL_MODE(TCP_LISTEN,     Network::TcpSocket);
    DEFINE_CHANNEL_MODE(TCP6_LISTEN,    Network::Tcp6Socket);
    DEFINE_CHANNEL_MODE(SCTP_CONNECT,   Network::SctpSocket);
    DEFINE_CHANNEL_MODE(SCTP6_CONNECT,  Network::Sctp6Socket);
    DEFINE_CHANNEL_MODE(SCTP_LISTEN,    Network::SctpSocket);
    DEFINE_CHANNEL_MODE(SCTP6_LISTEN,   Network::Sctp6Socket);

    //
    // The channel class definition.
    // Every channel is a daughter class of the underlying stream it is representing.
    //
    template <unsigned M>
    struct Channel : ChannelMode<M>::stream_type
    {
        CONSTRUCTOR Channel();

        template <enum Network::AddressType T>
        CONSTRUCTOR Channel(Network::Address<T> addr, uint16_t port);
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
        using namespace Shellcode;
        using SelectedChannel = Channel<CHANNEL>;

        // No parameter required for standard IOs.
        #if (CHANNEL == USE_STDOUT) || (CHANNEL == USE_STDERR)
            return SelectedChannel();

        // Connect-back channels require a remote address/port.
        #elif (CHANNEL == TCP_CONNECT) || (CHANNEL == TCP6_CONNECT) || (CHANNEL == SCTP_CONNECT) || (CHANNEL == SCTP6_CONNECT)
            auto remote_address = Pico::Network::ip_address_from_bytes(HOST);
            uint16_t remote_port = PORT;

            return SelectedChannel(remote_address, remote_port);

        // Listen channels require a local address/port.
        #elif (CHANNEL == TCP_LISTEN) || (CHANNEL == TCP6_LISTEN) || (CHANNEL == SCTP_LISTEN) || (CHANNEL == SCTP6_LISTEN)
            auto local_address = Pico::Network::ip_address_from_bytes(HOST);
            uint16_t local_port = PORT;

            return SelectedChannel(local_address, local_port);

        #else
        #error "No channel mode is selected."
        #endif
    }
}

#endif
