#include "RCC.h"

#define RCC_WAIT_TIMEOUT 100000UL

uint8_t RCC_Config_72Mhz(void)
{
    uint32_t timeout;

    RCC->CR.BITS.HSEON = 1;
    timeout = RCC_WAIT_TIMEOUT;
    while (!RCC->CR.BITS.HSERDY)
    {
        if (timeout == 0)
        {
            return 1;
        }
        timeout--;
    }

    ACR->BITS.LATENCY = 2;

    RCC->CFGR.BITS.PLLSRC = 1;
    RCC->CFGR.BITS.PLLMUL = 7;
    RCC->CFGR.BITS.USBPRE = 0;
    RCC->CR.BITS.PLLON = 1;
    timeout = RCC_WAIT_TIMEOUT;
    while (!RCC->CR.BITS.PLLRDY)
    {
        if (timeout == 0)
        {
            return 1;
        }
        timeout--;
    }


    RCC->CFGR.BITS.HPRE = 0;

    RCC->CFGR.BITS.PPRE1 = 4;

    RCC->CFGR.BITS.PPRE2 = 0;
    RCC->CFGR.BITS.ADCPRE = 2;

    RCC->CFGR.BITS.SW = 2;
    timeout = RCC_WAIT_TIMEOUT;
    while (RCC->CFGR.BITS.SWS != 2)
    {
        if (timeout == 0)
        {
            return 1;
        }
        timeout--;
    }

    return 0;
}

void RCC_Enable_PortA(void)
{
    RCC->APB2ENR.BITS.IOPAEN = 1;
}

void RCC_Enable_PortB(void)
{
    RCC->APB2ENR.BITS.IOPBEN = 1;
}

void RCC_Enable_PortC(void)
{
    RCC->APB2ENR.BITS.IOPCEN = 1;
}
void RCC_Enable_AFIO(void)
{
    RCC->APB2ENR.BITS.AFIOEN = 1;
}

void RCC_Enable_TIM2(void)
{
    RCC->APB1ENR.BITS.TIM2EN = 1;
}

void RCC_Enable_TIM3(void)
{
    RCC->APB1ENR.BITS.TIM3EN = 1;
}

void RCC_Enable_TIM4(void)
{
    RCC->APB1ENR.BITS.TIM4EN = 1;
}

void RCC_Enable_ADC01(void)
{
    RCC->APB2ENR.BITS.ADC1EN = 1;
}

void RCC_Enable_UART1(void)
{
    RCC->APB2ENR.BITS.USART1EN = 1;
}
void RCC_Enable_SPI1(void)
{
    RCC->APB2ENR.BITS.SPI1EN = 1;
}

void RCC_Enable_I2C1(void)
{
    RCC->APB1ENR.BITS.I2C1EN = 1;
}
void RCC_Enable_USB(void)
{
    RCC->APB1ENR.BITS.USBEN = 1;
}
