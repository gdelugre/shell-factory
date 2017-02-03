#ifndef PICOLIB_SOCKET_H_
#define PICOLIB_SOCKET_H_

namespace Pico {

    namespace Network {

        using AddressType = Target::Network::AddressType;

        template <AddressType>
        struct Address;

        class SocketIO : public SingleIO
        {
            public:
                using SingleIO::SingleIO;
                METHOD ssize_t in(void *, size_t);
                METHOD ssize_t out(const void *, size_t);
                METHOD size_t available_input_bytes();
        };

        class Socket : public Stream<SocketIO>
        {
            public:
                using Stream<SocketIO>::Stream; // inherit Stream constructor.

                CONSTRUCTOR Socket(int domain, int type, int protocol);
                METHOD int get(int level, int optname, void *val, unsigned *len);
                METHOD int set(int level, int optname, void *val, unsigned len);

                template <AddressType T>
                METHOD int connect(Address<T> addr);

                template <AddressType T>
                METHOD int connect(Address<T> addr, uint16_t port);

                template <AddressType T>
                METHOD int bind(Address<T> addr);

                template <AddressType T>
                METHOD int bind(Address<T> addr, uint16_t port, bool reuse_addr = false);
        };

        class SequencedSocket : public Socket
        {
            public:
                using Socket::Socket;
                CONSTRUCTOR SequencedSocket(int domain, int protocol);
        };

        class DatagramSocket : public Socket
        {
            public:
                using Socket::Socket;
                CONSTRUCTOR DatagramSocket(int domain, int protocol);
        };

        class UdpSocket : public DatagramSocket
        {
            public:
                using DatagramSocket::DatagramSocket;
                CONSTRUCTOR UdpSocket();
        };

        class Udp6Socket : public DatagramSocket
        {
            public:
                using DatagramSocket::DatagramSocket;
                CONSTRUCTOR Udp6Socket();
        };

        class StreamSocket : public Socket
        {
            public:
                using Socket::Socket;

                CONSTRUCTOR StreamSocket(int domain, int protocol);
                METHOD int listen(int backlog);

                template <bool Fork = false>
                METHOD StreamSocket accept();
        };

        class TcpSocket : public StreamSocket
        {
            public:
                using StreamSocket::StreamSocket;
                CONSTRUCTOR TcpSocket();
        };

        class Tcp6Socket : public StreamSocket
        {
            public:
                using StreamSocket::StreamSocket;
                CONSTRUCTOR Tcp6Socket();
        };

        class SctpSocket : public StreamSocket
        {
            public:
                using StreamSocket::StreamSocket;
                CONSTRUCTOR SctpSocket();
        };

        class Sctp6Socket : public StreamSocket
        {
            public:
                using StreamSocket::StreamSocket;
                CONSTRUCTOR Sctp6Socket();
        };

        class UnixStreamSocket : public StreamSocket
        {
            public:
                using StreamSocket::StreamSocket;
                CONSTRUCTOR UnixStreamSocket();

                METHOD int send_io(SingleIO const& io);
                METHOD SingleIO recv_io();
        };

        // TODO: consistency check between S and T.
        template <class SockType, bool Fork = false>
        class SocketServer
        {
            public:
                template <AddressType T>
                FUNCTION SockType start(Address<T> addr)
                {
                    SockType server;
                    server.bind(addr);
                    server.listen(1);

                    SockType client = SockType(server.template accept<Fork>().file_desc());
                    return client;
                }

                template <AddressType T>
                FUNCTION SockType start(Address<T> addr, uint16_t port, bool reuse_addr = false)
                {
                    SockType server;
                    server.bind(addr, port, reuse_addr);
                    server.listen(1);

                    SockType client = SockType(server.template accept<Fork>().file_desc());
                    return client;
                }
        };
    }
}

#endif
