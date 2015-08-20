#ifndef PICOLIB_STREAM_H_
#define PICOLIB_STREAM_H_

#include <cstdarg>

namespace Pico {

    class IO
    {
        public:
            // Do not declare these methods as pure. Compiler will fail at link time.
            METHOD ssize_t in(void *, size_t) {}
            METHOD ssize_t out(const void *, size_t) {}
            METHOD int close() {}

            METHOD IO& read(void *ptr, size_t count) {
                in(ptr, count);
                return *this;
            }

            METHOD IO& write(const void *ptr, size_t count) {
                out(ptr, count);
                return *this;
            }

            METHOD IO& read(Memory::Buffer const& buffer) {
                return read(buffer.pointer(), buffer.size());
            }

            METHOD IO& write(Memory::Buffer const& buffer) { 
                return write(buffer.pointer(), buffer.size());
            }

            METHOD friend IO& operator <<(IO &io, Memory::Buffer const& buffer)
            {
                return io.write(buffer);
            }

            METHOD friend IO& operator >>(IO &io, Memory::Buffer const& buffer)
            {
                return io.read(buffer);
            }

            METHOD friend IO& operator <<(Memory::Buffer const& buffer, IO &io)
            {
                return io >> buffer;
            }

            METHOD friend IO& operator >>(Memory::Buffer const& buffer, IO &io)
            {
                return io << buffer;
            }
    };

    class Stream : public IO
    {
        public:
            FUNCTION Stream standard_input();
            FUNCTION Stream standard_output();
            FUNCTION Stream standard_error();

            CONSTRUCTOR Stream() = default;
            CONSTRUCTOR Stream(int fd) : fd(fd) {}
            CONSTRUCTOR Stream(Stream &stm) : fd( stm.file_desc() ) {}
            CONSTRUCTOR Stream(Stream &&stm) = default;
            METHOD Stream& operator =(Stream &stm) = default;
            METHOD Stream& operator =(Stream &&stm) = default;

            METHOD ssize_t in(void *ptr, size_t count);
            METHOD ssize_t out(const void *ptr, size_t count);
            METHOD_NOINLINE int printf(const char *format, ...);
            METHOD int vprintf(const char *format, va_list ap);

            METHOD Stream duplicate();
            METHOD void duplicate(Stream&);
            METHOD void duplicate(Stream& r, Stream& w) {
                duplicate(r);
                duplicate(w);
            }

            METHOD int file_desc() const { return fd; }
            METHOD void flush();
            METHOD int close();

        protected:
            int fd = -1;
    };

    template <class Rx, class Tx = Rx>
    class BiStream : public IO
    {
        public:
            CONSTRUCTOR BiStream() = default;
            CONSTRUCTOR BiStream(Rx rx, Tx tx) : rx(rx), tx(tx) {}
            CONSTRUCTOR BiStream(int rfd, int wfd) : rx(Stream(rfd)), tx(Stream(wfd)) {}

            METHOD void duplicate(Stream& r, Stream &w) {
                rx.duplicate(r);
                tx.duplicate(w);
            }

            METHOD ssize_t in(void *ptr, size_t count) {
                return rx.in(ptr, count);
            }

            METHOD ssize_t out(const void *ptr, size_t count) {
                return tx.out(ptr, count);
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
