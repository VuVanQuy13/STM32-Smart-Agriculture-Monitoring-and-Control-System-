#ifndef __RELAY_CTRL_H
#define __RELAY_CTRL_H

#include <stdint.h>
#include "eeprom_cfg.h"


void Relay_Init(void);




void Relay_Toggle_Mode(void);



void Relay_Toggle_Pump_Manual(void);
void Relay_Toggle_Light_Manual(void);
void Relay_Toggle_Fan_Manual(void);


void Relay_Update(const Thresholds_t *th,
                  uint16_t adc_light, uint16_t adc_soil,
                  uint8_t dht_humidity, uint8_t dht_temp, uint8_t dht_valid,
                  uint8_t light_valid, uint8_t soil_valid);


void Relay_Refresh_Mode_Led(void);


uint8_t Relay_Is_Manual(void);

#endif
