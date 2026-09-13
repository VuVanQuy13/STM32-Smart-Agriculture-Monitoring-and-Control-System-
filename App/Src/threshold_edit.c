#include "threshold_edit.h"
#include "button.h"
#include "lcd_thresholds.h"
#include "lcd_i2c.h"
#include "TIM.h"


#define PARAM_SOIL     0
#define PARAM_TEMP     1
#define PARAM_HUMIDITY 2
#define PARAM_LIGHT    3
#define PARAM_COUNT    4


static const uint16_t s_min[PARAM_COUNT]  = { 0,    0,   0,   0    };
static const uint16_t s_max[PARAM_COUNT]  = { 4095, 100, 100, 4095 };
static const uint16_t s_step[PARAM_COUNT] = { 50,   1,   1,   50   };

static uint8_t       s_editing   = 0;
static uint8_t       s_cur_param = 0;
static Thresholds_t  s_edit_buf;
static uint8_t s_message_active;
static uint32_t s_message_started;


static uint16_t get_field(const Thresholds_t *t, uint8_t idx)
{
    switch (idx)
    {
        case PARAM_SOIL:     return t->soil;
        case PARAM_TEMP:     return t->temp;
        case PARAM_HUMIDITY: return t->humidity;
        case PARAM_LIGHT:    return t->light;
        default:             return 0;
    }
}

static void set_field(Thresholds_t *t, uint8_t idx, uint16_t val)
{
    switch (idx)
    {
        case PARAM_SOIL:     t->soil     = val; break;
        case PARAM_TEMP:     t->temp     = val; break;
        case PARAM_HUMIDITY: t->humidity = val; break;
        case PARAM_LIGHT:    t->light    = val; break;
        default: break;
    }
}


static uint8_t show_message(const char *line1, const char *line2)
{
    const char *lines[2] = {line1, line2};
    for (uint8_t row = 0; row < 2; row++)
    {
        char text[17];
        uint8_t col = 0;
        while (col < 16 && lines[row][col] != '\0')
        {
            text[col] = lines[row][col];
            col++;
        }
        while (col < 16) text[col++] = ' ';
        text[16] = '\0';
        if (LCD_Set_Cursor(LCD2_ADDR, 0, row) || LCD_Print(LCD2_ADDR, text)) return 1;
    }
    s_message_started = millis();
    s_message_active = 1;
    return 0;
}

uint8_t Threshold_Edit_Process(Thresholds_t *thresholds)
{
    if (thresholds == 0) return 1;
    if (s_message_active && (uint32_t)(millis() - s_message_started) >= 1000u)
    {
        s_message_active = 0;
        return LCD2_Show_Thresholds(s_editing ? &s_edit_buf : thresholds,
                                   s_editing ? (int8_t)s_cur_param : -1);
    }
    return 0;
}

uint8_t Threshold_Edit_Has_Message(void)
{
    return s_message_active;
}

uint8_t Threshold_Edit_Handle_Button(Thresholds_t *thresholds, Button_Id id)
{
    uint8_t status = 0;

    if (thresholds == 0 || (unsigned)id >= BTN_COUNT)
    {
        return 1;
    }

    if (id > BTN_PREV) return 0;
    if (s_message_active)
    {
        s_message_active = 0;
        if (LCD2_Show_Thresholds(s_editing ? &s_edit_buf : thresholds,
                                 s_editing ? (int8_t)s_cur_param : -1) != 0)
        {
            return 1;
        }
    }
    if (id == BTN_EDIT)
    {
        if (!s_editing)
        {
            s_editing   = 1;
            s_cur_param = PARAM_SOIL;
            s_edit_buf  = *thresholds;
            status |= LCD2_Show_Thresholds(&s_edit_buf, (int8_t)s_cur_param);
        }
    }


    uint8_t up   = id == BTN_UP;
    uint8_t down = id == BTN_DOWN;
    uint8_t next = id == BTN_NEXT;
    uint8_t prev = id == BTN_PREV;

    if (s_editing)
    {
        uint8_t changed = 0;

        if (up)
        {
            uint16_t val = get_field(&s_edit_buf, s_cur_param);
            uint16_t step = s_step[s_cur_param];
            uint16_t max  = s_max[s_cur_param];

            val = (val + step > max) ? max : (uint16_t)(val + step);
            set_field(&s_edit_buf, s_cur_param, val);
            changed = 1;
        }
        if (down)
        {
            uint16_t val = get_field(&s_edit_buf, s_cur_param);
            uint16_t step = s_step[s_cur_param];
            uint16_t min  = s_min[s_cur_param];

            val = (val < min + step) ? min : (uint16_t)(val - step);
            set_field(&s_edit_buf, s_cur_param, val);
            changed = 1;
        }
        if (next)
        {
            s_cur_param = (uint8_t)((s_cur_param + 1) % PARAM_COUNT);
            changed = 1;
        }
        if (prev)
        {
            s_cur_param = (uint8_t)((s_cur_param + PARAM_COUNT - 1) % PARAM_COUNT);
            changed = 1;
        }

        if (changed)
        {
            status |= LCD2_Show_Thresholds(&s_edit_buf, (int8_t)s_cur_param);
        }
    }


    if (id == BTN_SAVE)
    {
        if (s_editing)
        {
            if (EEPROM_Save(&s_edit_buf) == 0)
            {
                *thresholds = s_edit_buf;
                s_editing = 0;
                status |= show_message("Luu cai dat...", "");
            }
            else
            {
                status |= show_message("Loi luu Flash", "");
            }
        }
    }


    if (id == BTN_RST)
    {
        Thresholds_t defaults;

        defaults.soil     = THRESH_DEFAULT_SOIL;
        defaults.temp     = THRESH_DEFAULT_TEMP;
        defaults.humidity = THRESH_DEFAULT_HUMIDITY;
        defaults.light    = THRESH_DEFAULT_LIGHT;

        if (EEPROM_Save(&defaults) == 0)
        {
            *thresholds = defaults;
            s_editing = 0;
            status |= show_message("Da cai dat ve", "che do mac dinh");
        }
        else
        {
            status |= show_message("Loi luu Flash", "");
        }
    }

    return status;
}

uint8_t Threshold_Edit_Is_Active(void)
{
    return s_editing;
}
