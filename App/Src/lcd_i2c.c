#include "lcd_i2c.h"
#include "I2C.h"
#include "TIM.h"


uint8_t LCD_I2C_Write(uint8_t addr, uint8_t data)
{
    if (I2C_Start())
    {
        return 1;
    }
    if (I2C_Send_Addr(addr, 0))
    {
        return 1;
    }
    if (I2C_Send_Data(data))
    {
        return 1;
    }
    return I2C_Stop();
}


uint8_t LCD_Pulse_Enable(uint8_t addr, uint8_t data)
{
    if (LCD_I2C_Write(addr, data) != 0) return 1;
    if (LCD_I2C_Write(addr, data | LCD_EN) != 0) return 1;
    if (delay_us(1) != 0) return 1;
    if (LCD_I2C_Write(addr, data & ~(uint8_t)LCD_EN) != 0) return 1;
    if (delay_us(50) != 0) return 1;
    return 0;
}


uint8_t LCD_Send_Nibble(uint8_t addr, uint8_t nibble, uint8_t rs)
{
    uint8_t data = (uint8_t)((nibble << 4) & 0xF0);
    data |= LCD_BL;
    if (rs)
    {
        data |= LCD_RS;
    }
    return LCD_Pulse_Enable(addr, data);
}


uint8_t LCD_Send_Byte(uint8_t addr, uint8_t byte, uint8_t rs)
{
    if (LCD_Send_Nibble(addr, (uint8_t)(byte >> 4), rs) != 0) return 1;
    return LCD_Send_Nibble(addr, (uint8_t)(byte & 0x0F), rs);
}


uint8_t LCD_Init(uint8_t addr)
{
    if (delay_ms(50) != 0) return 1;

    if (LCD_Send_Nibble(addr, 0x03, 0) != 0) return 1;
    if (delay_ms(5) != 0) return 1;
    if (LCD_Send_Nibble(addr, 0x03, 0) != 0) return 1;
    if (delay_us(150) != 0) return 1;
    if (LCD_Send_Nibble(addr, 0x03, 0) != 0) return 1;
    if (delay_us(150) != 0) return 1;
    if (LCD_Send_Nibble(addr, 0x02, 0) != 0) return 1;
    if (delay_us(150) != 0) return 1;

    if (LCD_Send_Byte(addr, 0x28, 0) != 0) return 1;
    if (LCD_Send_Byte(addr, 0x0C, 0) != 0) return 1;
    if (LCD_Send_Byte(addr, 0x06, 0) != 0) return 1;
    if (LCD_Send_Byte(addr, 0x01, 0) != 0) return 1;
    return delay_ms(2);
}


uint8_t LCD_Clear(uint8_t addr)
{
    if (LCD_Send_Byte(addr, 0x01, 0) != 0) return 1;
    return delay_ms(2);
}


uint8_t LCD_Set_Cursor(uint8_t addr, uint8_t col, uint8_t row)
{
    if (col > 15 || row > 1) return 1;
    uint8_t ddram = (uint8_t)(col + (row == 0 ? 0x00 : 0x40));
    return LCD_Send_Byte(addr, (uint8_t)(0x80 | ddram), 0);
}


uint8_t LCD_Print(uint8_t addr, const char *str)
{
    if (str == 0) return 1;
    while (*str)
    {
        if (LCD_Send_Byte(addr, (uint8_t)(*str), 1) != 0) return 1;
        str++;
    }
    return 0;
}
