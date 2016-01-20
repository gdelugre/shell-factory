#ifndef FREEBSD_PICOLIB_H_
#define FREEBSD_PICOLIB_H_

/* Import FreeBSD syscalls. */
#include <target/freebsd/syscalls.h>

/* Target implementation. */
#include "pico/memory.cc"
#include "pico/stream.cc"
#include "pico/fs.cc"
#include "pico/process.cc"
#include "pico/debug.cc"
#include "pico/socket.cc"
#include "pico/ipc.cc"
#include "pico/security.cc"
#include "pico/random.cc"

#endif
