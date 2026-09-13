#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdint.h>






void Scheduler_Init(void);
uint8_t Scheduler_Consume_Sensor_Ready(void);

#endif
