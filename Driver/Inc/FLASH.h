#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>


#define FLASH_BASE    0x40022000UL

#define FLASH_KEYR   (*(volatile uint32_t *)(FLASH_BASE + 0x04))
#define FLASH_SR     (*(volatile uint32_t *)(FLASH_BASE + 0x0C))
#define FLASH_CR     (*(volatile uint32_t *)(FLASH_BASE + 0x10))
#define FLASH_AR     (*(volatile uint32_t *)(FLASH_BASE + 0x14))


#define FLASH_KEY1   0x45670123UL
#define FLASH_KEY2   0xCDEF89ABUL


#define FLASH_SR_BSY   (1 << 0)
#define FLASH_SR_PGERR (1 << 2)
#define FLASH_SR_WRPRTERR (1 << 4)
#define FLASH_SR_EOP   (1 << 5)


#define FLASH_CR_PG    (1 << 0)
#define FLASH_CR_PER   (1 << 1)
#define FLASH_CR_STRT  (1 << 6)
#define FLASH_CR_LOCK  (1 << 7)


#define EEPROM_PAGE_ADDR  0x0800F800UL
#define EEPROM_PAGE_SIZE  0x400UL
#define EEPROM_BACKUP_ADDR (EEPROM_PAGE_ADDR + EEPROM_PAGE_SIZE)
#define FLASH_MEMORY_END 0x08010000UL

typedef enum
{
    FLASH_OK = 0,
    FLASH_ERR_TIMEOUT,
    FLASH_ERR_PROGRAM,
    FLASH_ERR_WRITE_PROTECT,
    FLASH_ERR_INVALID_ADDR
} FLASH_Status;

FLASH_Status FLASH_Erase_Page(uint32_t addr);
FLASH_Status FLASH_Write_HW(uint32_t addr, uint16_t data);
FLASH_Status FLASH_Read_HW(uint32_t addr, uint16_t *data);

#endif
