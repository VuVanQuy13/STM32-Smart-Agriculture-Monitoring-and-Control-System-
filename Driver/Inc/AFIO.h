#ifndef __AFIO_H
#define __AFIO_H
#include <stdint.h>
#include <stddef.h>

typedef struct
{
    union
    {
        uint32_t REG;
        struct
        {
            uint32_t PIN : 4;
            uint32_t PORT : 3;
            uint32_t EVOE : 1;
            uint32_t Reserved : 24;
        } BITS;
    } EVCR;

    uint32_t MAPR;

    union
    {
        uint32_t REG;
        struct
        {
            uint32_t EXTI0 : 4;
            uint32_t EXTI1 : 4;
            uint32_t EXTI2 : 4;
            uint32_t EXTI3 : 4;
            uint32_t Reserved : 16;
        } BITS;
    } EXTICR1;

    union
    {
        uint32_t REG;
        struct
        {
            uint32_t EXTI4 : 4;
            uint32_t EXTI5 : 4;
            uint32_t EXTI6 : 4;
            uint32_t EXTI7 : 4;
            uint32_t Reserved : 16;
        } BITS;
    } EXTICR2;

    union
    {
        uint32_t REG;
        struct
        {
            uint32_t EXTI8 : 4;
            uint32_t EXTI9 : 4;
            uint32_t EXTI10 : 4;
            uint32_t EXTI11 : 4;
            uint32_t Reserved : 16;
        } BITS;
    } EXTICR3;

    union
    {
        uint32_t REG;
        struct
        {
            uint32_t EXTI12 : 4;
            uint32_t EXTI13 : 4;
            uint32_t EXTI14 : 4;
            uint32_t EXTI15 : 4;
            uint32_t Reserved : 16;
        } BITS;
    } EXTICR4;

    uint32_t Reserved;
    uint32_t MAPR2;

} AFIO_Typedef;

_Static_assert(offsetof(AFIO_Typedef, EXTICR1) == 0x08, "AFIO EXTICR1 offset");
_Static_assert(offsetof(AFIO_Typedef, MAPR2) == 0x1C, "AFIO MAPR2 offset");

#define AFIO ((volatile AFIO_Typedef *)(0x40010000UL))

#define AFIO_MAPR_SWJ_CFG_MASK     (7u << 24)
#define AFIO_MAPR_SWJ_NO_JTAG_SW   (2u << 24)

static inline void AFIO_Disable_JTAG_Enable_SWD(void)
{
    AFIO->MAPR = (AFIO->MAPR & ~(uint32_t)AFIO_MAPR_SWJ_CFG_MASK)
               | AFIO_MAPR_SWJ_NO_JTAG_SW;
}

#endif
