#ifndef GENERIC_PICOLIB_H_
#define GENERIC_PICOLIB_H_

#include <target/common/pico.h>

#if defined(__linux__)
#include <target/linux/pico.h>
#elif defined(__FreeBSD__)
#include <target/freebsd/pico.h>
#elif defined(__APPLE__)
#include <target/darwin/pico.h>
#else
#error "Picolib has no support for this operating system."
#endif

#endif
