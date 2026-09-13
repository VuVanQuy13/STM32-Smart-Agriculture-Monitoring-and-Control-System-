#include "EXTI.h"

uint8_t get_Pin_Number(uint16_t gpio_pin)
{
    if (gpio_pin == 0 || (gpio_pin & (gpio_pin - 1u)) != 0) return 0xFF;
    for (uint8_t i = 0; i < 16; i++)
    {
        if (gpio_pin & (1 << i))
        {
            return i;
        }
    }
    return 0xFF;
}

void EXTI_Init(uint16_t gpio_pin, volatile GPIO_Typedef *Port, uint8_t type)
{
    uint8_t port_code;
    uint8_t pin;
    if (type > EXTI_BOTH_MODE) return;
    if (Port == GPIOA)
    {
        port_code = 0;
    }
    else if (Port == GPIOB)
    {
        port_code = 1;
    }
    else if (Port == GPIOC)
    {
        port_code = 2;
    }
    else
    {
        return;
    }

    pin = get_Pin_Number(gpio_pin);
    if (pin > 15)
    {
        return;
    }

    uint32_t shift = (pin % 4) * 4;

    if (pin < 4)
    {
        AFIO->EXTICR1.REG &= (uint32_t)~(0xF << shift);
        AFIO->EXTICR1.REG |= (uint32_t)(port_code << shift);
    }
    else if (pin < 8)
    {
        AFIO->EXTICR2.REG &= (uint32_t)~(0xF << shift);
        AFIO->EXTICR2.REG |= (uint32_t)(port_code << shift);
    }
    else if (pin < 12)
    {
        AFIO->EXTICR3.REG &= (uint32_t)~(0xF << shift);
        AFIO->EXTICR3.REG |= (uint32_t)(port_code << shift);
    }
    else
    {
        AFIO->EXTICR4.REG &= (uint32_t)~(0xF << shift);
        AFIO->EXTICR4.REG |= (uint32_t)(port_code << shift);
    }

    EXTI->IMR.REG |= (1 << pin);

    if (type == EXTI_RISING_MODE)
    {
        EXTI->RTSR.REG |= (1 << pin);
        EXTI->FTSR.REG &= ~(1 << pin);
    }
    else if (type == EXTI_FALLING_MODE)
    {
        EXTI->FTSR.REG |= (1 << pin);
        EXTI->RTSR.REG &= ~(1 << pin);
    }
    else
    {
        EXTI->RTSR.REG |= (1 << pin);
        EXTI->FTSR.REG |= (1 << pin);
    }

    if (pin <= 4)
    {
        NVIC_ISER0 = (1u << (6 + pin));
    }
    else if (pin <= 9)
    {
        NVIC_ISER0 = (1u << 23);
    }
    else if (pin <= 15)
    {
        NVIC_ISER1 = (1u << (40 - 32));
    }
}

void NVIC_UART_En(void)
{
    NVIC_ISER1 = (1u << (37 - 32));
}

void NVIC_USB_En(void)
{
    NVIC_ISER0 = (1u << 20);
}
