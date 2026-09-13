#include "ADC.h"
#include "TIM.h"


#define SMP_55_5_CYCLES 0x5
#define ADC_WAIT_TIMEOUT 100000UL

uint8_t ADC1_Init(void)
{

    ADC01_CR2 &= ~(uint32_t)(1 << 0);


    ADC01_SMPR2 &= ~(uint32_t)(0x7 << 15);
    ADC01_SMPR2 |=  (uint32_t)(SMP_55_5_CYCLES << 15);
    ADC01_SMPR2 &= ~(uint32_t)(0x7 << 18);
    ADC01_SMPR2 |=  (uint32_t)(SMP_55_5_CYCLES << 18);


    ADC01_SQR1 &= ~(uint32_t)(0xF << 20);


    ADC01_CR2 |= (uint32_t)(0x7 << 17);
    ADC01_CR2 |= (uint32_t)(1 << 20);


    ADC01_CR2 |= (1 << 0);
    if (delay_ms(1) != 0)
    {
        return 1;
    }


    ADC01_CR2 |= (1 << 3);
    {
        uint32_t timeout = ADC_WAIT_TIMEOUT;
        while (ADC01_CR2 & (1 << 3))
        {
            if (timeout == 0)
            {
                return 1;
            }
            timeout--;
        }
    }


    ADC01_CR2 |= (1 << 2);
    {
        uint32_t timeout = ADC_WAIT_TIMEOUT;
        while (ADC01_CR2 & (1 << 2))
        {
            if (timeout == 0)
            {
                return 1;
            }
            timeout--;
        }
    }


    return 0;
}

uint8_t ADC1_Read_Channel(uint8_t ch, uint16_t *out)
{
    uint32_t timeout = ADC_WAIT_TIMEOUT;
    volatile uint32_t discard;

    if (out == 0)
    {
        return 1;
    }
    if (ch > 17)
    {
        return 1;
    }

    ADC01_SQR3 = (uint32_t)(ch & 0x1F);

    if (ADC01_SR & (1 << 1))
    {
        discard = ADC01_DR;
        (void)discard;
    }

    ADC01_CR2 |= (1 << 22);


    while (!(ADC01_SR & (1 << 1)))
    {
        if (timeout == 0)
        {
            return 1;
        }
        timeout--;
    }


    *out = (uint16_t)(ADC01_DR & 0x0FFF);
    return 0;
}
