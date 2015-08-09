#ifndef PICOLIB_STREAM_H_
#define PICOLIB_STREAM_H_

namespace Pico {

    class IO
    {
        public:
            VIRTUAL_METHOD IO& in(void *, size_t) = 0;
            VIRTUAL_METHOD IO& out(const void *, size_t) = 0;
            VIRTUAL_METHOD int close() = 0;

            METHOD IO& read(void *ptr, size_t count) {
                return in(ptr, count);
            }

            METHOD IO& write(const void *ptr, size_t count) {
                return out(ptr, count);
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

            METHOD Stream& in(void *ptr, size_t count);
            METHOD Stream& out(const void *ptr, size_t count);

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

            METHOD BiStream& in(void *ptr, size_t count) {
                rx.read(ptr, count);
                return *this;
            }

            METHOD BiStream& out(const void *ptr, size_t count) {
                tx.write(ptr, count);
                return *this;
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
