#ifndef LINUX_PICOLIB_H_
#define LINUX_PICOLIB_H_

// Import Linux syscalls.
#include <target/linux/syscalls.h>

// Import POSIX implementation.
#include <target/posix/pico.h>

// Target implementation.
#include "pico/concurrency.cc"
#include "pico/memory.cc"
#include "pico/stream.cc"
#include "pico/fs.cc"
#include "pico/process.cc"
#include "pico/debug.cc"
#include "pico/socket.cc"
#include "pico/terminal.cc"
#include "pico/ipc.cc"
#include "pico/security.cc"
#include "pico/random.cc"

#endif
