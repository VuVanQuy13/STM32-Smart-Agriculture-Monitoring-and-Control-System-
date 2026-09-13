#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

static inline uint32_t IRQ_Save(void)
{
    uint32_t mask;
    __asm volatile("mrs %0, primask" : "=r"(mask) :: "memory");
    __asm volatile("cpsid i" ::: "memory");
    return mask;
}

static inline void IRQ_Restore(uint32_t mask)
{
    __asm volatile("msr primask, %0" :: "r"(mask) : "memory");
}

#endif
