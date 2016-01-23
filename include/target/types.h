#ifndef GENERIC_TYPES_H_
#define GENERIC_TYPES_H_

// Empty base optimization.
class EmptyBaseClass {};
class EmptyClass : EmptyBaseClass {};
using EmptyType = EmptyClass;

#if defined(__linux__)
#include <target/linux/types.h>
#elif defined(__FreeBSD__)
#include <target/freebsd/types.h>
#else
#error "Types are not defined for this operating system."
#endif

#endif
