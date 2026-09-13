#include "lcd_sensors.h"
#include "lcd_i2c.h"
#include "text_util.h"

#define LCD1_COLS 16


#define ADC_MAX 4095


static uint8_t adc_to_pct(uint16_t raw)
{
    return (uint8_t)(((uint32_t)raw * 100) / ADC_MAX);
}

uint8_t LCD1_Show_Sensors(uint16_t adc_light, uint16_t adc_soil,
                          uint8_t dht_humidity, uint8_t dht_temp,
                          uint8_t dht_valid, uint8_t light_valid, uint8_t soil_valid)
{
    soil_valid = soil_valid && adc_soil <= ADC_MAX;
    light_valid = light_valid && adc_light <= ADC_MAX;
    uint8_t soil_pct  = soil_valid ? adc_to_pct(adc_soil) : 0;
    uint8_t light_pct = light_valid ? adc_to_pct(adc_light) : 0;

    char buf[LCD1_COLS + 1];
    char num[6];


    uint8_t pos = 0;
    buf[pos++] = 'S'; buf[pos++] = 'M'; buf[pos++] = ':';
    pct_to_str(soil_pct, &num[0]);
    for (uint8_t i = 0; i < 3; i++) buf[pos++] = soil_valid ? num[i] : '-';
    buf[pos++] = '%';
    buf[pos++] = ' ';
    buf[pos++] = 'L'; buf[pos++] = ':';
    pct_to_str(light_pct, &num[0]);
    for (uint8_t i = 0; i < 3; i++) buf[pos++] = light_valid ? num[i] : '-';
    buf[pos++] = '%';

    while (pos < LCD1_COLS) buf[pos++] = ' ';
    buf[pos] = '\0';

    if (LCD_Set_Cursor(LCD1_ADDR, 0, 0) != 0) return 1;
    if (LCD_Print(LCD1_ADDR, buf) != 0) return 1;


    pos = 0;
    buf[pos++] = 'T'; buf[pos++] = ':';
    if (dht_valid)
    {
        pct_to_str(dht_temp, &num[0]);
        for (uint8_t i = 0; i < 3; i++) buf[pos++] = num[i];
    }
    else
    {
        buf[pos++] = '-'; buf[pos++] = '-'; buf[pos++] = '-';
    }
    buf[pos++] = 'C';
    buf[pos++] = ' ';
    buf[pos++] = 'H'; buf[pos++] = 'u'; buf[pos++] = ':';
    if (dht_valid)
    {
        pct_to_str(dht_humidity, &num[0]);
        for (uint8_t i = 0; i < 3; i++) buf[pos++] = num[i];
    }
    else
    {
        buf[pos++] = '-'; buf[pos++] = '-'; buf[pos++] = '-';
    }
    buf[pos++] = '%';
    while (pos < LCD1_COLS) buf[pos++] = ' ';
    buf[pos] = '\0';

    if (LCD_Set_Cursor(LCD1_ADDR, 0, 1) != 0) return 1;
    return LCD_Print(LCD1_ADDR, buf);
}
