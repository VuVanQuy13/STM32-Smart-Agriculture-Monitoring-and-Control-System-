#ifndef FAULT_LOG_H
#define FAULT_LOG_H

#include <stdint.h>

typedef enum {
    FAULT_ADC_LIGHT = 1,
    FAULT_ADC_SOIL,
    FAULT_ADC_EXPIRED,
    FAULT_I2C,
    FAULT_EEPROM,
    FAULT_BOOT_CLOCK,
    FAULT_BOOT_ADC,
    FAULT_DHT_READ,
    FAULT_DHT_EXPIRED,
    FAULT_BUTTON_OVERFLOW
} FaultCode;

typedef struct {
    uint32_t sequence;
    uint32_t code;
    uint32_t detail;
} FaultEntry;

#define FAULT_LOG_CAPACITY 8u
extern volatile FaultEntry fault_log[FAULT_LOG_CAPACITY];
extern volatile uint32_t fault_log_sequence;

/* Chi goi tu main; doc nhat ky bang debugger, khong can UART. */
void Fault_Log_Record(FaultCode code, uint32_t detail);

#endif
