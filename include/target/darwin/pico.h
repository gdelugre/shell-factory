#ifndef DARWIN_PICOLIB_H_
#define DARWIN_PICOLIB_H_

// Import OS X syscalls.
#include <target/darwin/syscalls.h>

// Import POSIX implementation.
#include <target/posix/pico.h>

// Target implementation.
#include "pico/concurrency.cc"
#include "pico/memory.cc"
#include "pico/stream.cc"
#include "pico/fs.cc"
#include "pico/process.cc"
#include "pico/debug.cc"
#include "pico/random.cc"

#endif
