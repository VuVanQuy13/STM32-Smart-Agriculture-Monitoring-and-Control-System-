#ifndef __LCD_SENSORS_H
#define __LCD_SENSORS_H

#include <stdint.h>


uint8_t LCD1_Show_Sensors(uint16_t adc_light, uint16_t adc_soil,
                          uint8_t dht_humidity, uint8_t dht_temp,
                          uint8_t dht_valid, uint8_t light_valid, uint8_t soil_valid);

#endif
