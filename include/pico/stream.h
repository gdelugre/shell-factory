#ifndef PICOLIB_STREAM_H_
#define PICOLIB_STREAM_H_

#include <initializer_list>
#include <type_traits>
#include <cstdarg>

namespace Pico {

    template <typename Io>
    class Stream;

    class IO
    {
        public:
            enum poll_event {
                INPUT_READY,
                OUTPUT_READY,
                POLL_ERROR
            };

            template <typename Io, typename Callback>
            FUNCTION int select(std::initializer_list<Stream<Io>>,
                                std::initializer_list<Stream<Io>>,
                                std::initializer_list<Stream<Io>>,
                                long, Callback);

            template <typename Io, typename Callback>
            FUNCTION int select(std::initializer_list<Stream<Io>> r_ios,
                                std::initializer_list<Stream<Io>> w_ios,
                                long timeout, Callback cb)
            {
                return select(r_ios, w_ios, {}, timeout, cb);
            }

            template <typename Io, typename Callback>
            FUNCTION int select(std::initializer_list<Stream<Io>> r_ios,
                                long timeout, Callback cb)
            {
                return select(r_ios, {}, timeout, cb);
            }

        protected:
            IO() = default;
    };

    class BasicIO : IO
    {
        public:
            CONSTRUCTOR BasicIO() = default;
            CONSTRUCTOR BasicIO(int fd) : fd(fd) {}
            METHOD ssize_t in(void *, size_t);
            METHOD ssize_t out(const void *, size_t);
            METHOD int close();
            METHOD int file_desc() const { return fd; }

        private:
            int fd;
    };

    template <typename Io>
    class Stream
    {
        public:
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

            METHOD bool operator ==(Stream<Io> stm) {
                return file_desc() == stm.file_desc();
            }

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

        private:
            ReadIo rx;
            WriteIo tx;
    };

    template <typename ReadIo, typename WriteIo = ReadIo>
    class BiStream : public Stream<DuplexIO<ReadIo, WriteIo>>
    {
        public:
            CONSTRUCTOR BiStream(Stream<ReadIo> r, Stream<WriteIo> w)
            {
                this->io = DuplexIO<ReadIo, WriteIo>(r.io_port(), w.io_port());
            }

            template <typename T>
            METHOD void duplicate2(Stream<T>& r, Stream<T>& w) {
                Stream<ReadIo>(this->io.read_file_desc()).duplicate(r);
                Stream<WriteIo>(this->io.write_file_desc()).duplicate(w);
            }
    };
}

#endif
