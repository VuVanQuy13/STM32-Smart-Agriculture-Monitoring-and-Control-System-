#include "scheduler.h"
#include "RCC.h"
#include "TIM.h"


static volatile uint8_t sensor_ready = 0;

void Scheduler_Init(void)
{
    RCC_Enable_TIM3();
    TIM3_Init_10s();
}

uint8_t Scheduler_Consume_Sensor_Ready(void)
{
    uint32_t primask;
    uint8_t ready;

    __asm volatile("mrs %0, primask" : "=r"(primask) :: "memory");
    __asm volatile("cpsid i" ::: "memory");
    ready = sensor_ready;
    sensor_ready = 0;
    __asm volatile("msr primask, %0" :: "r"(primask) : "memory");

    return ready;
}


void TIM3_IRQHandler(void)
{
    if (TIM3_SR & (1 << 0))
    {
        TIM3_SR &= ~(uint32_t)(1 << 0);


        sensor_ready = 1;
    }
}
