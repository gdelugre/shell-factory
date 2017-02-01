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

    //
    // Channel type tags.
    //
    struct ChannelTypeStandardIoTag {};
    struct ChannelTypeStreamConnectTag {};
    struct ChannelTypeStreamListenTag {};
    struct ChannelTypeDatagramTag {};
    struct ChannelTypeReuseTag {};

    #define DEFINE_CHANNEL_MODE(mode, type, tag)                \
        template<>                                              \
        struct ChannelMode<ChannelType::mode>                   \
        {                                                       \
            using stream_type = type;                           \
            using type_tag = tag;                               \
        };                                                      \

    DEFINE_CHANNEL_MODE(USE_STDOUT,     BiStream<BasicStream>,  ChannelTypeStandardIoTag);
    DEFINE_CHANNEL_MODE(USE_STDERR,     BiStream<BasicStream>,  ChannelTypeStandardIoTag);
    DEFINE_CHANNEL_MODE(TCP_CONNECT,    Network::TcpSocket,     ChannelTypeStreamConnectTag);
    DEFINE_CHANNEL_MODE(TCP6_CONNECT,   Network::Tcp6Socket,    ChannelTypeStreamConnectTag);
    DEFINE_CHANNEL_MODE(TCP_LISTEN,     Network::TcpSocket,     ChannelTypeStreamListenTag);
    DEFINE_CHANNEL_MODE(TCP6_LISTEN,    Network::Tcp6Socket,    ChannelTypeStreamListenTag);
    DEFINE_CHANNEL_MODE(SCTP_CONNECT,   Network::SctpSocket,    ChannelTypeStreamConnectTag);
    DEFINE_CHANNEL_MODE(SCTP6_CONNECT,  Network::Sctp6Socket,   ChannelTypeStreamConnectTag);
    DEFINE_CHANNEL_MODE(SCTP_LISTEN,    Network::SctpSocket,    ChannelTypeStreamListenTag);
    DEFINE_CHANNEL_MODE(SCTP6_LISTEN,   Network::Sctp6Socket,   ChannelTypeStreamListenTag);
    DEFINE_CHANNEL_MODE(UDP_CONNECT,    Network::UdpSocket,     ChannelTypeDatagramTag);
    DEFINE_CHANNEL_MODE(UDP6_CONNECT,   Network::Udp6Socket,    ChannelTypeDatagramTag);
    DEFINE_CHANNEL_MODE(REUSE_SOCKET,   Network::Socket,        ChannelTypeReuseTag);
    DEFINE_CHANNEL_MODE(REUSE_FILE,     BasicStream,            ChannelTypeReuseTag);

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

    //
    // Compile-time switch.
    // This part can be rewritten as soon as 'if constexpr' becomes available.
    //

    template <enum ChannelType T>
    FUNCTION auto _channel_select(ChannelTypeStandardIoTag)
    {
        return Channel<T>();
    }

    template <enum ChannelType T>
    FUNCTION auto _channel_select(ChannelTypeStreamConnectTag)
    {
        return Channel<T>(Options::remote_address, Options::remote_port);
    }

    template <enum ChannelType T>
    FUNCTION auto _channel_select(ChannelTypeStreamListenTag)
    {
        return Channel<T>(Options::local_address, Options::local_port);
    }

    template <enum ChannelType T>
    FUNCTION auto _channel_select(ChannelTypeDatagramTag)
    {
        return Channel<T>(Options::local_address, Options::local_port, Options::remote_address, Options::remote_port);
    }

    template <enum ChannelType T>
    FUNCTION auto _channel_select(ChannelTypeReuseTag)
    {
        return Channel<T>(Options::handle);
    }

    FUNCTION auto channel()
    {
        using ChannelTypeTag = ChannelMode<ChannelType::OPT_CHANNEL>::type_tag;

        return _channel_select<ChannelType::OPT_CHANNEL>(ChannelTypeTag{});
    }
}

#endif
