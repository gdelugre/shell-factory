#ifndef PICOLIB_STREAM_H_
#define PICOLIB_STREAM_H_

namespace Pico {

    class IO
    {
        public:
            VIRTUAL_METHOD ssize_t in(void *, size_t) = 0;
            VIRTUAL_METHOD ssize_t out(const void *, size_t) = 0;
            VIRTUAL_METHOD int close() = 0;

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

            METHOD Stream duplicate();
            METHOD void replace(Stream const&);
            METHOD int file_desc() const { return fd; }
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
