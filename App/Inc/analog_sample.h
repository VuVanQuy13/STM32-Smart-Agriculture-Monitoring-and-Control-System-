#ifndef ANALOG_SAMPLE_H
#define ANALOG_SAMPLE_H

#include <stdint.h>

#define ANALOG_MAX_VALUE 4095u
#define ANALOG_MAX_AGE_MS 15000u

typedef struct {
    uint16_t value;
    uint8_t valid;
    uint32_t last_good_ms;
} AnalogSample;

static inline void Analog_Sample_Update(AnalogSample *sample, uint16_t value,
                                       uint8_t status, uint32_t now)
{
    if (sample == 0) return;
    sample->valid = (status == 0 && value <= ANALOG_MAX_VALUE);
    if (sample->valid)
    {
        sample->value = value;
        sample->last_good_ms = now;
    }
}

static inline uint8_t Analog_Sample_Is_Valid(const AnalogSample *sample, uint32_t now)
{
    return sample != 0 && sample->valid && sample->value <= ANALOG_MAX_VALUE &&
           (uint32_t)(now - sample->last_good_ms) < ANALOG_MAX_AGE_MS;
}

#endif
