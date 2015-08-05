#ifndef PICOLIB_STREAM_H_
#define PICOLIB_STREAM_H_

namespace Pico {

    class Stream {
        public:
            FUNCTION Stream standard_input();
            FUNCTION Stream standard_output();
            FUNCTION Stream standard_error();

            CONSTRUCTOR Stream() = default;
            CONSTRUCTOR Stream(int fd) : fd(fd) {}

            METHOD Stream& read(void *ptr, size_t count);
            METHOD Stream& read(Memory::Buffer const& buffer) {
                return read(buffer.pointer(), buffer.size());
            }

            METHOD Stream& write(void *ptr, size_t count);
            METHOD Stream& write(Memory::Buffer const& buffer) { 
                return write(buffer.pointer(), buffer.size());
            }

            METHOD friend Stream& operator <<(Stream &stm, Memory::Buffer const& buffer)
            {
                return stm.write(buffer);
            }

            METHOD friend Stream& operator >>(Stream &stm, Memory::Buffer const& buffer)
            {
                return stm.read(buffer);
            }

            METHOD friend Stream& operator <<(Memory::Buffer const& buffer, Stream &stm)
            {
                return stm >> buffer;
            }

            METHOD friend Stream& operator >>(Memory::Buffer const& buffer, Stream &stm)
            {
                return stm << buffer;
            }

            METHOD Stream duplicate();
            METHOD void replace(Stream const&);
            METHOD int file_desc() const { return fd; }
            METHOD int close();

        protected:
            int fd = -1;
    };

}

#endif
