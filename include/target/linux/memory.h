#ifndef LINUX_MEMORY_H_
#define LINUX_MEMORY_H_

#if defined(__i386__)
#include <target/linux/i386/memory.h>
#elif defined(__amd64__)
#include <target/linux/amd64/memory.h>
#elif defined(__arm__)
#include <target/linux/arm/memory.h>
#elif defined(__aarch64__)
#include <target/linux/aarch64/memory.h>
#elif defined(__mips__)
#include <target/linux/mips/memory.h>
#else
#error "No memory header defined for this architecture."
#endif

#endif
