#ifndef __TEXT_UTIL_H
#define __TEXT_UTIL_H

#include <stdint.h>


uint8_t int_to_str(uint16_t val, char *buf);


/* buf can it nhat 4 byte: 3 ky tu va ky tu ket thuc. */
uint8_t pct_to_str(uint8_t val, char *buf);

#endif
