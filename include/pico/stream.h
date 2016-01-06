#ifndef PICOLIB_STREAM_H_
#define PICOLIB_STREAM_H_

#include <initializer_list>
#include <type_traits>
#include <cstdarg>

namespace Pico {

    class SingleIO;
    class IO
    {
        public:
            enum poll_event {
                INPUT_READY,
                OUTPUT_READY,
                POLL_ERROR
            };

            template <typename Callback>
            FUNCTION int select(std::initializer_list<SingleIO>,
                                std::initializer_list<SingleIO>,
                                std::initializer_list<SingleIO>,
                                long, Callback);

            template <typename Callback>
            FUNCTION int select(std::initializer_list<SingleIO> r_ios,
                                std::initializer_list<SingleIO> w_ios,
                                long timeout, Callback cb)
            {
                return select(r_ios, w_ios, {}, timeout, cb);
            }

            template <typename Callback>
            FUNCTION int select(std::initializer_list<SingleIO> r_ios,
                                long timeout, Callback cb)
            {
                return select(r_ios, {}, timeout, cb);
            }

        protected:
            IO() = default;
    };

    class SingleIO : IO
    {
        public:
            CONSTRUCTOR SingleIO() = default;
            CONSTRUCTOR SingleIO(int fd) : fd(fd) {}
            METHOD int file_desc() const { return fd; }
            METHOD int close();
            METHOD int close_on_exec();
            METHOD bool operator ==(SingleIO io) {
                return file_desc() == io.file_desc();
            }

        protected:
            int fd;
    };

    class BasicIO : public SingleIO
    {
        public:
            using SingleIO::SingleIO;
            CONSTRUCTOR BasicIO() = default;

            METHOD ssize_t in(void *, size_t);
            METHOD ssize_t out(const void *, size_t);
    };

    template <typename Io>
    class Stream
    {
        public:
            typedef Io io_type;

            CONSTRUCTOR Stream() = default;
            CONSTRUCTOR Stream(int fd) : io(fd) {}

            METHOD ssize_t read(void *ptr, size_t count) {
                return io.in(ptr, count);
            }

            METHOD ssize_t write(const void *ptr, size_t count) {
                return io.out(ptr, count);
            }

            METHOD ssize_t read(Memory::Buffer const& buffer) {
                return read(buffer.pointer(), buffer.size());
            }

            METHOD ssize_t write(Memory::Buffer const& buffer) {
                return write(buffer.pointer(), buffer.size());
            }

            METHOD friend Stream<Io>& operator <<(Stream<Io>& stm, Memory::Buffer const& buffer)
            {
                stm.write(buffer);
                return stm;
            }

            METHOD friend Stream<Io>& operator >>(Stream<Io>& stm, Memory::Buffer const& buffer)
            {
                stm.read(buffer);
                return stm;
            }

            METHOD friend Stream<Io>& operator <<(Memory::Buffer const& buffer, Stream<Io>& stm)
            {
                stm >> buffer;
                return stm;
            }

            METHOD friend Stream<Io>& operator >>(Memory::Buffer const& buffer, Stream<Io>& stm)
            {
                stm << buffer;
                return stm;
            }

            // Streams are compared using their IO ports.
            METHOD bool operator ==(Stream<Io> stm) {
                return io_port() == stm.io_port();
            }

            // Stream can be implicitly casted to their IO port.
            METHOD operator Io() const { return io_port(); }

            METHOD ssize_t readline(char *ptr, size_t n, char delim = '\n') {
                size_t nr_read;

                for ( nr_read = 0; n > 0 && nr_read < n - 1; nr_read++, ptr++ ) {
                    if ( read(ptr, 1) < 0 || *ptr == delim )
                        break;
                }
                *ptr = '\0';
                return nr_read;
            }

            METHOD_NOINLINE int printf(const char *format, ...);
            METHOD int vprintf(const char *format, va_list ap);
            METHOD Io  io_port() const { return io; }
            METHOD int file_desc() { return io.file_desc(); }
            METHOD int close() { return io.close(); }
            METHOD int close_on_exec() { return io.close_on_exec(); }

            template <typename T>
            METHOD void duplicate(Stream<T>& s);

            template <typename T>
            METHOD void duplicate2(Stream<T>& r, Stream<T> &w)
            {
                duplicate(r);
                duplicate(w);
            }

        protected:
            Io io;
    };

    typedef Stream<BasicIO> BasicStream;

    namespace Stdio
    {
        FUNCTION BasicStream input();
        FUNCTION BasicStream output();
        FUNCTION BasicStream error();
    }

    template <typename ReadIo, typename WriteIo>
    class DuplexIO : IO
    {
        static_assert(std::is_base_of<IO, ReadIo>::value, "DuplexIO requires an IO object");
        static_assert(std::is_base_of<IO, WriteIo>::value, "DuplexIO requires an IO object");

        public:
            CONSTRUCTOR DuplexIO() = default;
            CONSTRUCTOR DuplexIO(ReadIo rx, WriteIo tx) : rx(rx), tx(tx) {}
            METHOD ssize_t in(void *ptr, size_t count) {
                return rx.in(ptr, count);
            }
            METHOD ssize_t out(const void *ptr, size_t count) {
                return tx.out(ptr, count);
            }
            METHOD int read_file_desc() const {
                return rx.file_desc();
            }
            METHOD int write_file_desc() const {
                return tx.file_desc();
            }
            METHOD int close() {
                return rx.close() | tx.close();
            }
            METHOD bool operator ==(DuplexIO io) {
                return rx == io.read_file_desc() && tx == io.write_file_desc();
            }

        private:
            ReadIo rx;
            WriteIo tx;
    };

    template <typename ReadStream, typename WriteStream = ReadStream>
    class BiStream : public Stream<DuplexIO<typename ReadStream::io_type, typename WriteStream::io_type>>
    {
        public:
            CONSTRUCTOR BiStream(ReadStream r, WriteStream w)
            {
                using ReadIO = typename ReadStream::io_type;
                using WriteIO = typename WriteStream::io_type;

                this->io = DuplexIO<ReadIO, WriteIO>(r.io_port(), w.io_port());
            }

            METHOD ReadStream read_stream() const { return ReadStream(this->io.read_file_desc()); }
            METHOD WriteStream write_stream() const { return WriteStream(this->io.write_file_desc()); }

            template <typename T>
            METHOD void duplicate2(Stream<T>& r, Stream<T>& w) {
                this->read_stream().duplicate(r);
                this->write_stream().duplicate(w);
            }
    };
}

#endif
