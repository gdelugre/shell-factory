#ifndef _ARCH_I386_HELPER_H
#define _ARCH_I386_HELPER_H

#define PAGE_SIZE 4096

static inline
void * get_sp(void)
{
    void *sp;
    asm volatile("movl %%esp, %0\n" : "=r" (sp));
    return sp;
}

static inline 
void set_sp(void *sp)
{
  asm volatile("movl %0, %%esp\n" :: "r" (sp));
}

static inline
void x86_brand(unsigned char brand_name[48])
{
    int base = 0x80000002, i;
    unsigned int *regs = (unsigned int *) brand_name;

    for ( i = 0; i < 3; i++ )
    {
        asm volatile("cpuid" : "=a" (regs[i*4+0]), "=b" (regs[i*4+1]), "=c" (regs[i*4+2]), "=d" (regs[i*4+3])
                         : "a" (base+i));
        asm volatile("":::"memory");
    }
}

static inline
unsigned int x86_apid_id(void)
{
    unsigned int info;
    int op = 0x1;

    asm volatile("cpuid" : "=b" (info) : "a" (op));
    asm volatile("":::"eax", "ebx", "ecx", "edx");
    return (info >> 24);
}

static inline 
uint64_t read_tsc() 
{
    uint64_t tsc;
    __asm__ __volatile__ ("rdtsc" : "=A" (tsc) );
    return tsc;
}

#endif
