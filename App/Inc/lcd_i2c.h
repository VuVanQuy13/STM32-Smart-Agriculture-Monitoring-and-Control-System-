#ifndef __LCD_I2C_H
#define __LCD_I2C_H

#include <stdint.h>


#define LCD_RS   (1 << 0)
#define LCD_RW   (1 << 1)
#define LCD_EN   (1 << 2)
#define LCD_BL   (1 << 3)

#define LCD1_ADDR 0x27
#define LCD2_ADDR 0x26


uint8_t LCD_I2C_Write(uint8_t addr, uint8_t data);
uint8_t LCD_Pulse_Enable(uint8_t addr, uint8_t data);
uint8_t LCD_Send_Nibble(uint8_t addr, uint8_t nibble, uint8_t rs);
uint8_t LCD_Send_Byte(uint8_t addr, uint8_t byte, uint8_t rs);


uint8_t LCD_Init(uint8_t addr);


uint8_t LCD_Clear(uint8_t addr);
uint8_t LCD_Set_Cursor(uint8_t addr, uint8_t col, uint8_t row);
uint8_t LCD_Print(uint8_t addr, const char *str);

#endif
