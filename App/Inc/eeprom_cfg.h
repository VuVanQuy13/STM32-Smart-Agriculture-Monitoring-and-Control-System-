#ifndef __EEPROM_CFG_H
#define __EEPROM_CFG_H

#include <stdint.h>


typedef struct {
    uint16_t soil;
    uint16_t temp;
    uint16_t humidity;
    uint16_t light;
} Thresholds_t;


#define THRESH_DEFAULT_SOIL     1500u
#define THRESH_DEFAULT_TEMP       30u
#define THRESH_DEFAULT_HUMIDITY   85u
#define THRESH_DEFAULT_LIGHT    2000u


#define EEPROM_MAGIC  0xA5A5u


#define OFF_SOIL     0x00u
#define OFF_TEMP     0x02u
#define OFF_HUMIDITY 0x04u
#define OFF_LIGHT    0x06u
#define OFF_MAGIC    0x08u


uint8_t EEPROM_Save(const Thresholds_t *t);


void EEPROM_Load(Thresholds_t *t);

#endif
