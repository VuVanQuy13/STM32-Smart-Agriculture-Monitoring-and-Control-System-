#include "GPIO.h"

void GPIO_Config(volatile GPIO_Typedef *GPIOx, uint16_t pin, uint32_t mode)
{
    GPIO_Config_Speed(GPIOx, pin, mode, GPIO_SPEED_50MHZ);
}

void GPIO_Config_Speed(volatile GPIO_Typedef *GPIOx, uint16_t pin,
                       uint32_t mode, uint32_t speed)
{
    uint32_t position = 0;
    uint32_t config = 0;

    if (GPIOx == 0 || pin == 0)
    {
        return;
    }
    if (mode > GPIO_MODE_INPUT_PD || speed < GPIO_SPEED_10MHZ || speed > GPIO_SPEED_50MHZ)
    {
        return;
    }

    for (position = 0; position < 16; position++)
    {
        if (pin & (1 << position))
        {

            if (position < 8)
            {

                GPIOx->CRL.REG &= ~(0xFu << (position * 4));
            }
            else
            {

                GPIOx->CRH.REG &= ~(0xFu << ((position - 8) * 4));
            }
            switch (mode)
            {
            case GPIO_MODE_OUTPUT_PP:
                config = speed | (0x00 << 2);
                break;
            case GPIO_MODE_OUTPUT_OD:
                config = speed | (0x01 << 2);
                break;
            case GPIO_MODE_AF_PP:
                config = speed | (0x02 << 2);
                break;
            case GPIO_MODE_AF_OD:
                config = speed | (0x03 << 2);
                break;
            case GPIO_MODE_INPUT_ANALOG:
                config = (0x00 << 2);
                break;
            case GPIO_MODE_INPUT_FLOATING:
                config = (0x01 << 2);
                break;
            case GPIO_MODE_INPUT_PU:
                config = (0x02 << 2);
                GPIOx->ODR.REG |= (1 << position);
                break;
            case GPIO_MODE_INPUT_PD:
                config = (0x02 << 2);
                GPIOx->ODR.REG &= ~(1 << position);
                break;
            default:
                return;
            }
            if (mode == GPIO_MODE_INPUT_FLOATING ||
                mode == GPIO_MODE_INPUT_PD || mode == GPIO_MODE_INPUT_PU)
            {
                config |= 0x00;
            }

            if (position < 8)
            {
                GPIOx->CRL.REG |= (config << (position * 4));
            }
            else
            {
                GPIOx->CRH.REG |= (config << (position - 8) * 4);
            }
        }
    }
}

void GPIO_Write_Pin(volatile GPIO_Typedef *GPIOx, uint16_t pin, uint8_t state)
{

    if (state)
    {
        GPIOx->BSRR.REG = pin;
    }
    else
    {
        GPIOx->BRR.REG = pin;
    }
}
uint8_t GPIO_Read_Pin(volatile GPIO_Typedef *GPIOx, uint16_t pin)
{

    return ((GPIOx->IDR.REG & pin) ? 1 : 0);
}
