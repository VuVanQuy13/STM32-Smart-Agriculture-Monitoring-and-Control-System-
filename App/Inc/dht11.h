#ifndef __DHT11_H
#define __DHT11_H

#include <stdint.h>


typedef enum
{
    DHT11_OK = 0,
    DHT11_ERR_TIMEOUT,
    DHT11_ERR_CHECKSUM,
    DHT11_ERR_INVALID_ARG,
    DHT11_ERR_DELAY,
    DHT11_ERR_RANGE,
    DHT11_ERR_TIMING
} DHT11_Status;

typedef struct
{
    uint8_t humidity;
    uint8_t temperature;
} DHT11_Data;

/* This application's unsigned, whole-degree readings support 0..60 C. */
#define DHT11_MAX_TEMP_C 60u
#define DHT11_MAX_HUMIDITY 100u

static inline uint8_t DHT11_Data_Is_Valid(const DHT11_Data *data)
{
    return data != 0 && data->humidity <= DHT11_MAX_HUMIDITY &&
           data->temperature <= DHT11_MAX_TEMP_C;
}

DHT11_Status DHT11_Read(DHT11_Data *out);

#endif
