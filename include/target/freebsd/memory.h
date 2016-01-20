#ifndef FREEBSD_MEMORY_H_
#define FREEBSD_MEMORY_H_

#if defined(__amd64__)
#include <target/freebsd/amd64/memory.h>
#else
#error "No memory header defined for this architecture."
#endif

#endif
