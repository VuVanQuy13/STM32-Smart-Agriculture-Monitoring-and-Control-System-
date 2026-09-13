#include "TIM.h"
#include "EXTI.h"

#define TIM_DELAY_MAX_COUNT 65535UL
#define TIM_DELAY_STALL_LIMIT 1000000UL
#define TIM2_MS_PRESCALER 7199UL
#define TIM2_COUNTS_PER_MS 10UL

void TIM2_init_IT(void)
{
    TIM2_CR1 &= ~(uint32_t)(1 << 0);
    TIM2_PSC = TIM2_MS_PRESCALER;
    TIM2_ARR = 9999;
    TIM2_EGR = 1;
    TIM2_SR = 0;
    TIM2_DIER |= (1 << 0);
    TIM2_CR1 |= (1 << 0);
    NVIC_ISER0 = (1u << 28);
}

void TIM2_IRQHandler(void)
{
    if (TIM2_SR & 0x01)
    {
        TIM2_SR &= ~(uint32_t)(1 << 0);
    }
}

uint8_t delay_ms(uint32_t ms)
{
    uint16_t last_count;
    uint32_t elapsed = 0;
    uint32_t stalled_count = 0;

    if (ms > TIM_DELAY_MAX_COUNT)
    {
        return 1;
    }

    if (ms == 0) return 0;
    TIM2_CR1 &= ~(uint32_t)(1 << 0);
    TIM2_PSC = TIM2_MS_PRESCALER;
    TIM2_ARR = 0xFFFF;
    TIM2_EGR |= (1 << 0);
    TIM2_SR &= ~(uint32_t)(1 << 0);
    TIM2_CNT = 0;
    TIM2_CR1 |= (1 << 0);
    last_count = (uint16_t)TIM2_CNT;
    while (elapsed < ms * TIM2_COUNTS_PER_MS)
    {
        uint16_t current_count = (uint16_t)TIM2_CNT;
        if (current_count == last_count)
        {
            if (stalled_count >= TIM_DELAY_STALL_LIMIT)
            {
                TIM2_CR1 &= ~(uint32_t)(1 << 0);
                TIM2_CNT = 0;
                return 1;
            }
            stalled_count++;
        }
        else
        {
            /* Cong don qua lan tran bo dem 16 bit. */
            elapsed += (uint16_t)(current_count - last_count);
            last_count = current_count;
            stalled_count = 0;
        }
    }
    TIM2_CR1 &= ~(uint32_t)(1 << 0);
    TIM2_CNT = 0;
    return 0;
}


void TIM3_Init_10s(void)
{
    TIM3_PSC = 35999;
    TIM3_ARR = 19999;
    TIM3_CNT = 0;
    TIM3_EGR |= (1 << 0);
    TIM3_SR  &= ~(uint32_t)(1 << 0);
    TIM3_DIER |= (1 << 0);
    TIM3_CR1 |= (1 << 0);
    NVIC_ISER0 = (1u << 29);
}


uint8_t delay_us(uint32_t us)
{
    uint16_t last_count;
    uint32_t elapsed = 0;
    uint32_t stalled_count = 0;

    if (us > TIM_DELAY_MAX_COUNT)
    {
        return 1;
    }

    if (us == 0) return 0;
    TIM4_CR1 &= ~1u;
    TIM4_PSC = 71;
    TIM4_ARR = 0xFFFF;
    TIM4_EGR |= (1 << 0);
    TIM4_SR &= ~(uint32_t)(1 << 0);
    TIM4_CNT = 0;
    TIM4_CR1 |= (1 << 0);
    last_count = (uint16_t)TIM4_CNT;
    while (elapsed < us)
    {
        uint16_t current_count = (uint16_t)TIM4_CNT;
        if (current_count == last_count)
        {
            if (stalled_count >= TIM_DELAY_STALL_LIMIT)
            {
                TIM4_CR1 &= ~(uint32_t)(1 << 0);
                TIM4_CNT = 0;
                return 1;
            }
            stalled_count++;
        }
        else
        {
            elapsed += (uint16_t)(current_count - last_count);
            last_count = current_count;
            stalled_count = 0;
        }
    }
    TIM4_CR1 &= ~(uint32_t)(1 << 0);
    TIM4_CNT = 0;
    return 0;
}


void TIM4_Start_Free(void)
{
    TIM4_PSC = 71;
    TIM4_ARR = 0xFFFF;
    TIM4_EGR |= (1 << 0);
    TIM4_SR &= ~(uint32_t)(1 << 0);
    TIM4_CNT = 0;
    TIM4_CR1 |= (1 << 0);
}

uint16_t TIM4_Get_us(void)
{
    return (uint16_t)TIM4_CNT;
}


static volatile uint32_t ms_counter = 0;

void SysTick_Init(void)
{
    SYSTICK_LOAD = 71999;
    SYSTICK_VAL  = 0;
    SYSTICK_CTRL = (1 << 0)
                 | (1 << 1)
                 | (1 << 2);
}

uint32_t millis(void)
{
    return ms_counter;
}


__attribute__((weak)) void SysTick_App_Tick(void)
{
}

void SysTick_Handler(void)
{
    ms_counter++;
    SysTick_App_Tick();
}
