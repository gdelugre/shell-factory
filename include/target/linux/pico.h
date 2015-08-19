#ifndef LINUX_PICOLIB_H_
#define LINUX_PICOLIB_H_

/* Import Linux syscalls. */
#include <target/linux/syscalls.h>

/* Target implementation. */
#include "pico/memory.cc"
#include "pico/stream.cc"
#include "pico/fs.cc"
#include "pico/process.cc"
#include "pico/debug.cc"
#include "pico/socket.cc"

#endif
