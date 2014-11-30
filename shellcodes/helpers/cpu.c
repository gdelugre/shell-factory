#ifndef _CPU_HELPER_H
#define _CPU_HELPER_H

#include <stdint.h>

#if defined(__arm__)
#include "arch/arm.c"
#elif defined(__amd64__)
#include "arch/amd64.c"
#elif defined(__386__)
#include "arch/i386.c"
#endif

#define STACK_SHIFT(delta) set_sp(get_sp() + delta)

#endif
