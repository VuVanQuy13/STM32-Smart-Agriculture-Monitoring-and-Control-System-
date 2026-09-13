#include "text_util.h"

uint8_t int_to_str(uint16_t val, char *buf)
{

    if (val == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return 1;
    }

    char tmp[5];
    uint8_t n = 0;


    while (val > 0)
    {
        tmp[n++] = (char)('0' + (val % 10));
        val /= 10;
    }


    for (uint8_t i = 0; i < n; i++)
    {
        buf[i] = tmp[n - 1 - i];
    }
    buf[n] = '\0';
    return n;
}

uint8_t pct_to_str(uint8_t val, char *buf)
{
    if (buf == 0) return 0;
    if (val > 100) val = 100;
    buf[0] = (val == 100) ? '1' : ' ';
    buf[1] = (val >= 10) ? (char)('0' + ((val / 10) % 10)) : ' ';
    buf[2] = (char)('0' + (val % 10));
    buf[3] = '\0';
    return 3;
}
