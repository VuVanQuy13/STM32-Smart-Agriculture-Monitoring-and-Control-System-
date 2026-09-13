#ifndef DHT_SAMPLE_H
#define DHT_SAMPLE_H

#include "dht11.h"

#define DHT_SAMPLE_MAX_AGE_MS 15000u

typedef struct {
    DHT11_Data value;
    uint8_t valid;
    uint32_t last_good_ms;
} DHTSample;

static inline void DHT_Sample_Update(DHTSample *sample, const DHT11_Data *data,
                                     DHT11_Status status, uint32_t now)
{
    if (sample == 0) return;
    sample->valid = status == DHT11_OK && DHT11_Data_Is_Valid(data);
    if (sample->valid)
    {
        sample->value = *data;
        sample->last_good_ms = now;
    }
}

static inline uint8_t DHT_Sample_Is_Valid(const DHTSample *sample, uint32_t now)
{
    return sample != 0 && sample->valid &&
           DHT11_Data_Is_Valid(&sample->value) &&
           (uint32_t)(now - sample->last_good_ms) < DHT_SAMPLE_MAX_AGE_MS;
}

#endif
