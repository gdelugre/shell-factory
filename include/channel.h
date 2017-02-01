#ifndef CHANNEL_HELPER_H_
#define CHANNEL_HELPER_H_

#include <channel-options.h>
#include <pico.h>

namespace Shellcode {

    using namespace Pico;

    //
    // The list of defined channel modes.
    //
    enum class ChannelType {
        USE_STDOUT,
        USE_STDERR,
        TCP_CONNECT,
        TCP_LISTEN,
        TCP6_CONNECT,
        TCP6_LISTEN,
        SCTP_CONNECT,
        SCTP_LISTEN,
        SCTP6_CONNECT,
        SCTP6_LISTEN,
        UDP_CONNECT,
        UDP6_CONNECT,
        REUSE_SOCKET,
        REUSE_FILE,
    };

    //
    // Static structure holding information for each mode.
    //
    template <enum ChannelType>
    struct ChannelMode;

    #define DEFINE_CHANNEL_MODE(mode, type)                     \
        template<>                                              \
        struct ChannelMode<ChannelType::mode>                   \
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
    DEFINE_CHANNEL_MODE(UDP_CONNECT,    Network::UdpSocket);
    DEFINE_CHANNEL_MODE(UDP6_CONNECT,   Network::Udp6Socket);
    DEFINE_CHANNEL_MODE(REUSE_SOCKET,   Network::Socket);
    DEFINE_CHANNEL_MODE(REUSE_FILE,     BasicStream);

    //
    // The channel class definition.
    // Every channel is a daughter class of the underlying stream it is representing.
    //
    template <enum ChannelType C>
    struct Channel : ChannelMode<C>::stream_type
    {
        CONSTRUCTOR Channel();

        CONSTRUCTOR Channel(int fd);

        template <enum Network::AddressType T>
        CONSTRUCTOR Channel(Network::Address<T> addr, uint16_t port);

        template <enum Network::AddressType T>
        CONSTRUCTOR Channel(Network::Address<T> laddr, uint16_t lport,
                            Network::Address<T> raddr, uint16_t rport);
    };

    template<>
    CONSTRUCTOR
    Channel<ChannelType::USE_STDOUT>::Channel() :
        ChannelMode<ChannelType::USE_STDOUT>::stream_type(Stdio::input(), Stdio::output()) {}

    template<>
    CONSTRUCTOR
    Channel<ChannelType::USE_STDERR>::Channel() :
        ChannelMode<ChannelType::USE_STDERR>::stream_type(Stdio::input(), Stdio::error()) {}

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<ChannelType::UDP_CONNECT>::Channel(Network::Address<T> laddr, uint16_t lport, Network::Address<T> raddr, uint16_t rport)
    {
        static_assert(T == Network::IPV4, "UDP_CONNECT requires an IPV4 address.");
        bind(laddr, lport);
        connect(raddr, rport);
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<ChannelType::UDP6_CONNECT>::Channel(Network::Address<T> laddr, uint16_t lport, Network::Address<T> raddr, uint16_t rport)
    {
        static_assert(T == Network::IPV6, "UDP6_CONNECT requires an IPV4 address.");
        bind(laddr, lport);
        connect(raddr, rport);
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<ChannelType::TCP_CONNECT>::Channel(Network::Address<T> addr, uint16_t port) {
        static_assert(T == Network::IPV4, "TCP_CONNECT requires an IPV4 address.");
        connect(addr, port);
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<ChannelType::TCP6_CONNECT>::Channel(Network::Address<T> addr, uint16_t port) {
        static_assert(T == Network::IPV6, "TCP6_CONNECT requires an IPV6 address.");
        connect(addr, port);
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<ChannelType::TCP_LISTEN>::Channel(Network::Address<T> addr, uint16_t port) :
        TcpSocket(Network::SocketServer<Network::TcpSocket, Options::fork_on_accept>::start(addr, port, Options::reuse_addr))
    {
        static_assert(T == Network::IPV4, "TCP_LISTEN requires an IPV4 address.");
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<ChannelType::TCP6_LISTEN>::Channel(Network::Address<T> addr, uint16_t port) :
        Tcp6Socket(Network::SocketServer<Network::Tcp6Socket, Options::fork_on_accept>::start(addr, port, Options::reuse_addr))
    {
        static_assert(T == Network::IPV6, "TCP6_LISTEN requires an IPV6 address.");
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<ChannelType::SCTP_CONNECT>::Channel(Network::Address<T> addr, uint16_t port) {
        static_assert(T == Network::IPV4, "SCTP_CONNECT requires an IPV4 address.");
        connect(addr, port);
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<ChannelType::SCTP6_CONNECT>::Channel(Network::Address<T> addr, uint16_t port) {
        static_assert(T == Network::IPV6, "SCTP6_CONNECT requires an IPV6 address.");
        connect(addr, port);
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<ChannelType::SCTP_LISTEN>::Channel(Network::Address<T> addr, uint16_t port) :
        SctpSocket(Network::SocketServer<Network::SctpSocket, Options::fork_on_accept>::start(addr, port, Options::reuse_addr))
    {
        static_assert(T == Network::IPV4, "SCTP_LISTEN requires an IPV4 address.");
    }

    template <>
    template <enum Network::AddressType T>
    CONSTRUCTOR
    Channel<ChannelType::SCTP6_LISTEN>::Channel(Network::Address<T> addr, uint16_t port) :
        Sctp6Socket(Network::SocketServer<Network::Sctp6Socket, Options::fork_on_accept>::start(addr, port, Options::reuse_addr))
    {
        static_assert(T == Network::IPV6, "SCTP6_LISTEN requires an IPV6 address.");
    }

    template <>
    CONSTRUCTOR
    Channel<ChannelType::REUSE_SOCKET>::Channel(int fd) : Socket(fd) {}

    template <>
    CONSTRUCTOR
    Channel<ChannelType::REUSE_FILE>::Channel(int fd) : BasicStream(fd) {}


    FUNCTION auto channel()
    {
        using SelectedChannel = Channel<ChannelType::OPT_CHANNEL>;

        // No parameter required for standard IOs.
        #if (CHANNEL == USE_STDOUT) || (CHANNEL == USE_STDERR)
            return SelectedChannel();

        // Connect-back channels require a remote address/port.
        #elif (CHANNEL == TCP_CONNECT) || (CHANNEL == TCP6_CONNECT) || (CHANNEL == SCTP_CONNECT) || (CHANNEL == SCTP6_CONNECT)
            return SelectedChannel(Options::remote_address, Options::remote_port);

        // Listen channels require a local address/port.
        #elif (CHANNEL == TCP_LISTEN) || (CHANNEL == TCP6_LISTEN) || (CHANNEL == SCTP_LISTEN) || (CHANNEL == SCTP6_LISTEN)
            return SelectedChannel(Options::local_address, Options::local_port);

        #elif (CHANNEL == UDP_CONNECT) || (CHANNEL == UDP6_CONNECT)
            return SelectedChannel(Options::local_address, Options::local_port, Options::remote_address, Options::remote_port);

        #elif (CHANNEL == REUSE_SOCKET) || (CHANNEL == REUSE_FILE)
            static_assert(Options::handle != -1, "Requires a HANDLE parameter.");
            return SelectedChannel(Options::handle);

        #else
        #error "No channel mode is selected."
        #endif
    }
}

#endif
