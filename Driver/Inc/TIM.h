#ifndef __TIM_H
#define __TIM_H
#include <stdint.h>

#define TIM2_ADD_BASE (0x40000000UL)
#define TIM3_ADD_BASE (0x40000400UL)
#define TIM4_ADD_BASE (0x40000800UL)

#define TIM2_CR1 (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x00))
#define TIM2_CR2 (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x04))
#define TIM2_SMCR (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x08))
#define TIM2_DIER (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x0C))
#define TIM2_SR (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x10))
#define TIM2_EGR (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x14))
#define TIM2_CCMR1 (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x18))
#define TIM2_CCMR2 (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x1C))
#define TIM2_CCER (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x20))
#define TIM2_CNT (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x24))
#define TIM2_PSC (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x28))
#define TIM2_ARR (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x2C))
#define TIM2_CCR1 (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x34))
#define TIM2_CCR2 (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x38))
#define TIM2_CCR3 (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x3C))
#define TIM2_CCR4 (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x40))
#define TIM2_DCR (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x48))
#define TIM2_DMAR (*(volatile uint32_t *)(TIM2_ADD_BASE + 0x4C))


#define TIM3_CR1 (*(volatile uint32_t *)(TIM3_ADD_BASE + 0x00))
#define TIM3_DIER (*(volatile uint32_t *)(TIM3_ADD_BASE + 0x0C))
#define TIM3_SR (*(volatile uint32_t *)(TIM3_ADD_BASE + 0x10))
#define TIM3_EGR (*(volatile uint32_t *)(TIM3_ADD_BASE + 0x14))
#define TIM3_CNT (*(volatile uint32_t *)(TIM3_ADD_BASE + 0x24))
#define TIM3_PSC (*(volatile uint32_t *)(TIM3_ADD_BASE + 0x28))
#define TIM3_ARR (*(volatile uint32_t *)(TIM3_ADD_BASE + 0x2C))


#define TIM4_CR1 (*(volatile uint32_t *)(TIM4_ADD_BASE + 0x00))
#define TIM4_DIER (*(volatile uint32_t *)(TIM4_ADD_BASE + 0x0C))
#define TIM4_SR (*(volatile uint32_t *)(TIM4_ADD_BASE + 0x10))
#define TIM4_EGR (*(volatile uint32_t *)(TIM4_ADD_BASE + 0x14))
#define TIM4_CNT (*(volatile uint32_t *)(TIM4_ADD_BASE + 0x24))
#define TIM4_PSC (*(volatile uint32_t *)(TIM4_ADD_BASE + 0x28))
#define TIM4_ARR (*(volatile uint32_t *)(TIM4_ADD_BASE + 0x2C))


#define SYSTICK_CTRL (*(volatile uint32_t *)0xE000E010)
#define SYSTICK_LOAD (*(volatile uint32_t *)0xE000E014)
#define SYSTICK_VAL (*(volatile uint32_t *)0xE000E018)

void TIM2_init_IT(void);
void TIM2_IRQHandler(void);


void TIM3_Init_10s(void);
void TIM3_IRQHandler(void);

uint8_t delay_ms(uint32_t ms);
uint8_t delay_us(uint32_t us);


void TIM4_Start_Free(void);
uint16_t TIM4_Get_us(void);


void SysTick_Init(void);
uint32_t millis(void);
void SysTick_Handler(void);
/* Optional short application callback; invoked after the millis increment. */
void SysTick_App_Tick(void);

#endif
