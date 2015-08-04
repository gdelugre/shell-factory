#ifndef PICOLIB_STREAM_H_
#define PICOLIB_STREAM_H_

namespace Pico {

    class Stream {
        public:
            METHOD int read(void *ptr, size_t count);
            METHOD int read(Memory::Buffer const& buffer) {
                return read(buffer.pointer(), buffer.size());
            }

            METHOD int write(void *ptr, size_t count);
            METHOD int write(Memory::Buffer const& buffer) { 
                return write(buffer.pointer(), buffer.size());
            }

            METHOD Stream& operator <<(Memory::Buffer const& buffer) {
                write(buffer);
                return *this;
            }

            METHOD int close();

        protected:
            int fd;
    };
}

#endif
