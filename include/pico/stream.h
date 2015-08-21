#ifndef PICOLIB_STREAM_H_
#define PICOLIB_STREAM_H_

#include <cstdarg>

namespace Pico {

    class BasicIO
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

            METHOD_NOINLINE int printf(const char *format, ...);
            METHOD int vprintf(const char *format, va_list ap);
            METHOD int file_desc() { return io.file_desc(); }
            METHOD int close() { return io.close(); }

            template <typename T>
            METHOD void duplicate(Stream<T>& s);

            template <typename T>
            METHOD void duplicate(Stream<T>& r, Stream<T> &w)
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

    template <class Rx, class Tx = Rx>
    class BiStream
    {
        public:
            CONSTRUCTOR BiStream(Rx rx, Tx tx) : rx(rx), tx(tx) {}

            METHOD void duplicate(Rx& r, Tx& w) {
                rx.duplicate(r);
                tx.duplicate(w);
            }

            METHOD ssize_t read(void *ptr, size_t count) {
                return rx.read(ptr, count);
            }

            METHOD ssize_t write(const void *ptr, size_t count) {
                return tx.write(ptr, count);
            }

            METHOD int close() {
                return rx.close() | tx.close();
            }

        protected:
            Rx rx;
            Tx tx;
    };
}

#endif
