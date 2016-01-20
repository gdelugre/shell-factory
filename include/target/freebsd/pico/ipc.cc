#ifndef PICOLIB_IPC_IMPL_H_
#define PICOLIB_IPC_IMPL_H_

namespace Pico {

    METHOD
    BiStream<Pipe> Pipe::create() {
        int fds[2];

        Syscall::pipe(fds);
        return BiStream<Pipe>(Pipe(fds[0]), Pipe(fds[1]));
    } 
}

#endif
