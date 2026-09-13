#include "lcd_thresholds.h"
#include "lcd_i2c.h"
#include "text_util.h"

#define LCD2_COLS 16


#define PARAM_SOIL     0
#define PARAM_TEMP     1
#define PARAM_HUMIDITY 2
#define PARAM_LIGHT    3

static void append_char(char *buf, uint8_t *pos, char value)
{
    if (*pos < LCD2_COLS)
    {
        buf[*pos] = value;
        (*pos)++;
    }
}

static void append_number(char *buf, uint8_t *pos, uint16_t value)
{
    char num[6];
    uint8_t n = int_to_str(value, num);

    for (uint8_t i = 0; i < n; i++)
    {
        append_char(buf, pos, num[i]);
    }
}

uint8_t LCD2_Show_Thresholds(const Thresholds_t *th, int8_t mark_param)
{
    char buf[LCD2_COLS + 1];
    uint8_t pos;

    if (th == 0)
    {
        return 1;
    }


    pos = 0;
    append_char(buf, &pos, 'S'); append_char(buf, &pos, 'M'); append_char(buf, &pos, '<');
    append_char(buf, &pos, '=');
    append_number(buf, &pos, th->soil);
    append_char(buf, &pos, (mark_param == PARAM_SOIL) ? '*' : ' ');
    append_char(buf, &pos, 'T'); append_char(buf, &pos, '>');
    append_number(buf, &pos, th->temp);
    append_char(buf, &pos, (mark_param == PARAM_TEMP) ? '*' : ' ');
    while (pos < LCD2_COLS) append_char(buf, &pos, ' ');
    buf[pos] = '\0';

    if (LCD_Set_Cursor(LCD2_ADDR, 0, 0) != 0) return 1;
    if (LCD_Print(LCD2_ADDR, buf) != 0) return 1;

    
    pos = 0;
    append_char(buf, &pos, 'H'); append_char(buf, &pos, 'u'); append_char(buf, &pos, '>');
    append_number(buf, &pos, th->humidity);
    append_char(buf, &pos, (mark_param == PARAM_HUMIDITY) ? '*' : ' ');
    append_char(buf, &pos, ' '); append_char(buf, &pos, ' ');
    append_char(buf, &pos, 'L'); append_char(buf, &pos, '<');
    append_number(buf, &pos, th->light);
    append_char(buf, &pos, (mark_param == PARAM_LIGHT) ? '*' : ' ');
    while (pos < LCD2_COLS) append_char(buf, &pos, ' ');
    buf[pos] = '\0';

    if (LCD_Set_Cursor(LCD2_ADDR, 0, 1) != 0) return 1;
    return LCD_Print(LCD2_ADDR, buf);
}
