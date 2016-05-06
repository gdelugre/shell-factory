#ifndef PICOLIB_STREAM_H_
#define PICOLIB_STREAM_H_

#include <initializer_list>
#include <type_traits>
#include <cstdarg>

namespace Pico {

    using handle = Target::handle;

    // Forward String declarations.
    template <typename T>
    class BasicString;
    using String = BasicString<char>;

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
            CONSTRUCTOR SingleIO(handle fd) : fd(fd) {}
            METHOD handle file_desc() const { return fd; }
            METHOD bool eof() const { return b_eof; }
            METHOD bool error() const { return b_error; }
            METHOD int close();
            METHOD int close_on_exec();
            METHOD bool operator ==(SingleIO io) {
                return file_desc() == io.file_desc();
            }
            METHOD bool is_invalid() const {
                return fd == Target::invalid_handle;
            }

        protected:
            handle fd;
            bool b_eof = false;
            bool b_error = false;
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
            using io_type = Io;

            CONSTRUCTOR Stream() = default;
            CONSTRUCTOR Stream(handle fd) : io(fd) {}

            METHOD ssize_t read(void *ptr, size_t count) {
                return io.in(ptr, count);
            }

            METHOD ssize_t write(const void *ptr, size_t count) {
                return io.out(ptr, count);
            }

            METHOD bool eof() const {
                return io.eof();
            }

            METHOD bool error() const {
                return io.error();
            }

            // Allow writing of string literals with no size argument.
            // Used for quick string printing, like Stdio::output().write("Hello world!\n");
            template <unsigned N>
            METHOD ssize_t write(const char (&str)[N])
            {
                return io.out(str, N-1);
            }

            METHOD ssize_t write(String const& str) {
                return write(str.pointer(), str.length());
            }

            // Streams are compared using their IO ports.
            METHOD bool operator ==(Stream<Io> stm) const {
                return io_port() == stm.io_port();
            }

            // Comparison returns false on type mismatch.
            template <typename T>
            METHOD bool operator ==(Stream<T> stm) const {
                return false;
            }

            template <typename T>
            METHOD bool operator !=(Stream<T> stm) const {
                return !(*this == stm);
            }

            // Stream can be implicitly casted to their IO port.
            METHOD operator Io() const { return io_port(); }

            METHOD ssize_t readline(char *ptr, size_t n, char delim = '\n') {
                size_t nr_read;

                for ( nr_read = 0; n > 0 && nr_read < n - 1; nr_read++, ptr++ ) {
                    size_t ret = read(ptr, 1);

                    if ( ret < 0 || this->eof() || *ptr == delim )
                        break;
                }
                *ptr = '\0';
                return nr_read;
            }

            template <typename T = char>
            METHOD BasicString<T> readline(T delim = '\n') {
                BasicString<T> str;
                T chr[2] = {0, 0};

                while ( true ) {
                    ssize_t ret = read(&chr[0], sizeof(T));

                    if ( ret < 0 || this->eof() || *chr == delim )
                        break;

                    str += chr;
                }

                return str;
            }

            template <typename Func, typename T = char>
            METHOD void each_line(Func cb, T delim = '\n') {
                while ( !eof() && !error() ) {
                    BasicString<T> str = readline(delim);

                    if ( cb(str) != 0 )
                        break;
                }
            }

            METHOD_NOINLINE int printf(const char *format, ...);
            METHOD int vprintf(const char *format, va_list ap);
            METHOD Io  io_port() const { return io; }
            METHOD handle file_desc() const { return io.file_desc(); }
            METHOD int close() { return io.close(); }
            METHOD int close_on_exec() { return io.close_on_exec(); }
            METHOD bool is_invalid() const { return io.is_invalid(); }

            template <typename T>
            METHOD void duplicate(Stream<T>& s);

        protected:
            Io io;
    };

    using BasicStream = Stream<BasicIO>;

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

            // Should not be called. Undefined behavior.
            METHOD handle file_desc() const {
                assert(true);
                return Target::invalid_handle;
            }

            METHOD handle read_file_desc() const {
                return rx.file_desc();
            }
            METHOD handle write_file_desc() const {
                return tx.file_desc();
            }
            METHOD int close() {
                return rx.close() | tx.close();
            }
            METHOD bool operator ==(DuplexIO io) {
                return rx == io.read_file_desc() && tx == io.write_file_desc();
            }
            METHOD bool is_invalid() const {
                return rx.is_invalid() || tx.is_invalid();
            }
            METHOD bool eof() const {
                return rx.eof();
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
    };
}

#endif
