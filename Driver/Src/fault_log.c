#include "fault_log.h"

volatile FaultEntry fault_log[FAULT_LOG_CAPACITY];
volatile uint32_t fault_log_sequence;

void Fault_Log_Record(FaultCode code, uint32_t detail)
{
    uint32_t sequence = fault_log_sequence + 1u;
    volatile FaultEntry *entry = &fault_log[fault_log_sequence % FAULT_LOG_CAPACITY];
    entry->code = (uint32_t)code;
    entry->detail = detail;
    entry->sequence = sequence;
    fault_log_sequence = sequence;
}
