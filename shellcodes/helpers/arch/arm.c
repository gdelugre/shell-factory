#ifndef _ARCH_ARM_HELPER_H
#define _ARCH_ARM_HELPER_H

#define PAGE_SIZE 4096

static inline 
void *get_sp(void)
{
  void *sp;
  asm volatile("mov %0, sp\n" : "=r" (sp));
  return sp;
}

static inline 
void set_sp(void *sp)
{
  asm volatile("mov sp, %0\n" :: "r" (sp));
}

#endif
