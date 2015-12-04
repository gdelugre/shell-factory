#ifndef PICOLIB_IPC_H_
#define PICOLIB_IPC_H_

namespace Pico {

    class Pipe : public BasicStream {
        public:
            using BasicStream::BasicStream;
            FUNCTION BiStream<Pipe> create();
    };
}

#endif
