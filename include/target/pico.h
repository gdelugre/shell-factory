#ifndef GENERIC_PICOLIB_H_
#define GENERIC_PICOLIB_H_

#if defined(__linux__)
#include <target/linux/pico.h>
#else
#error "Picolib has no support for this operating system."
#endif

#endif
