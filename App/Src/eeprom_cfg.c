#include "eeprom_cfg.h"
#include "FLASH.h"
#include "fault_log.h"

#define RECORD_MAGIC 0x5AA5u
#define RECORD_VERSION 1u
#define RECORD_WORDS 9u
#define WORD_MAGIC 4u
#define WORD_VERSION 5u
#define WORD_SEQ_LO 6u
#define WORD_SEQ_HI 7u
#define WORD_CRC 8u

typedef struct {
    Thresholds_t values;
    uint32_t sequence;
    uint8_t legacy;
} ConfigRecord;

static void Thresholds_Set_Defaults(Thresholds_t *t)
{
    t->soil     = THRESH_DEFAULT_SOIL;
    t->temp     = THRESH_DEFAULT_TEMP;
    t->humidity = THRESH_DEFAULT_HUMIDITY;
    t->light    = THRESH_DEFAULT_LIGHT;
}

static uint8_t Thresholds_Are_Valid(const Thresholds_t *t)
{
    if (t->soil > 4095u || t->light > 4095u)
    {
        return 0;
    }
    if (t->temp > 100u || t->humidity > 100u)
    {
        return 0;
    }

    return 1;
}

static uint8_t Thresholds_Equal(const Thresholds_t *a, const Thresholds_t *b)
{
    return a->soil == b->soil && a->temp == b->temp &&
           a->humidity == b->humidity && a->light == b->light;
}

static uint16_t Record_CRC(const uint16_t *words)
{
    uint16_t crc = 0xFFFFu;
    for (uint8_t i = 0; i < WORD_CRC; i++)
    {
        if (i == WORD_MAGIC) continue;
        for (uint8_t byte = 0; byte < 2; byte++)
        {
            crc ^= (uint16_t)((uint16_t)((words[i] >> (byte * 8)) & 0xFFu) << 8);
            for (uint8_t bit = 0; bit < 8; bit++)
            {
                uint32_t shifted = (uint32_t)crc << 1;
                crc = (uint16_t)((crc & 0x8000u) ? shifted ^ 0x1021u : shifted);
            }
        }
    }
    return crc;
}

static uint8_t Record_Read(uint32_t addr, ConfigRecord *record)
{
    uint16_t words[RECORD_WORDS];
    for (uint8_t i = 0; i < RECORD_WORDS; i++)
    {
        if (FLASH_Read_HW(addr + i * 2u, &words[i]) != FLASH_OK) return 0;
    }
    record->values.soil = words[0];
    record->values.temp = words[1];
    record->values.humidity = words[2];
    record->values.light = words[3];
    if (!Thresholds_Are_Valid(&record->values)) return 0;

    record->legacy = (addr == EEPROM_PAGE_ADDR && words[WORD_MAGIC] == EEPROM_MAGIC);
    if (record->legacy)
    {
        record->sequence = 0;
        return 1;
    }
    if (words[WORD_MAGIC] != RECORD_MAGIC || words[WORD_VERSION] != RECORD_VERSION ||
        words[WORD_CRC] != Record_CRC(words)) return 0;
    record->sequence = (uint32_t)words[WORD_SEQ_LO] | ((uint32_t)words[WORD_SEQ_HI] << 16);
    return 1;
}

static uint32_t Record_Latest(ConfigRecord *record)
{
    ConfigRecord a;
    ConfigRecord b;
    uint8_t valid_a = Record_Read(EEPROM_PAGE_ADDR, &a);
    uint8_t valid_b = Record_Read(EEPROM_BACKUP_ADDR, &b);
    if (!valid_a && !valid_b) return 0;
    if (valid_b && (!valid_a || a.legacy ||
        (b.sequence != a.sequence && (uint32_t)(b.sequence - a.sequence) < 0x80000000u)))
    {
        *record = b;
        return EEPROM_BACKUP_ADDR;
    }
    *record = a;
    return EEPROM_PAGE_ADDR;
}

uint8_t EEPROM_Save(const Thresholds_t *t)
{
    ConfigRecord active;
    ConfigRecord verify;
    uint16_t words[RECORD_WORDS];
    uint32_t stage = 0;

    if (t == 0 || !Thresholds_Are_Valid(t))
    {
        Fault_Log_Record(FAULT_EEPROM, 0);
        return 1;
    }
    uint32_t source = Record_Latest(&active);
    if (source && !active.legacy && Thresholds_Equal(t, &active.values)) return 0;
    uint32_t target = (source == EEPROM_PAGE_ADDR) ? EEPROM_BACKUP_ADDR : EEPROM_PAGE_ADDR;
    uint32_t sequence = source ? active.sequence + 1u : 1u;
    words[0] = t->soil;
    words[1] = t->temp;
    words[2] = t->humidity;
    words[3] = t->light;
    words[WORD_MAGIC] = RECORD_MAGIC;
    words[WORD_VERSION] = RECORD_VERSION;
    words[WORD_SEQ_LO] = (uint16_t)sequence;
    words[WORD_SEQ_HI] = (uint16_t)(sequence >> 16);
    words[WORD_CRC] = Record_CRC(words);

    /* Chi xoa trang du phong; ban da commit van nguyen khi mat dien. */
    stage = 1;
    if (FLASH_Erase_Page(target) != FLASH_OK) goto failed;
    for (uint8_t i = 0; i < RECORD_WORDS; i++)
    {
        if (i == WORD_MAGIC) continue;
        stage = 2u + i;
        if (FLASH_Write_HW(target + i * 2u, words[i]) != FLASH_OK) goto failed;
    }
    stage = 11;
    for (uint8_t i = 0; i < RECORD_WORDS; i++)
    {
        uint16_t value;
        if (i == WORD_MAGIC) continue;
        if (FLASH_Read_HW(target + i * 2u, &value) != FLASH_OK || value != words[i]) goto failed;
    }
    stage = 12;
    if (FLASH_Write_HW(target + OFF_MAGIC, RECORD_MAGIC) != FLASH_OK) goto failed;
    stage = 13;
    if (!Record_Read(target, &verify) || verify.sequence != sequence ||
        !Thresholds_Equal(t, &verify.values)) goto failed;
    return 0;

failed:
    Fault_Log_Record(FAULT_EEPROM, stage);
    return 1;
}

void EEPROM_Load(Thresholds_t *t)
{
    ConfigRecord loaded;
    
    if (t == 0)
    {
        return;
    }

    if (Record_Latest(&loaded))
    {
        *t = loaded.values;
        return;
    }

    Thresholds_Set_Defaults(t);
}
