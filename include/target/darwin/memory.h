#ifndef DARWIN_MEMORY_H_
#define DARWIN_MEMORY_H_

#if defined(__amd64__)
#include <target/darwin/amd64/memory.h>
#else
#error "No memory header defined for this architecture."
#endif

#endif
