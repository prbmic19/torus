#ifndef ASM_X86_MISC_WRAPPERS_H
#define ASM_X86_MISC_WRAPPERS_H

#include <torus/compiler.h>
#include <torus/types.h>

__always_inline static u64 read_msr(unsigned int msr)
{
    unsigned int low;
    unsigned int high;

    asm volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));

    return ((u64)high << 32) | (u64)low;
}

__always_inline static void write_msr(unsigned int msr, u64 value)
{
    unsigned int low = value & 0xffffffff;
    unsigned int high = value >> 32;

    asm volatile ("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

__always_inline static void pause(void)
{
    asm volatile ("pause" : : : "memory");
}

#endif
