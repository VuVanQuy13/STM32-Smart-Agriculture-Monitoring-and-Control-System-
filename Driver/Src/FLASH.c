#include "FLASH.h"


#define FLASH_WAIT_TIMEOUT 1000000UL

static FLASH_Status FLASH_Wait_Busy(void)
{
    uint32_t timeout = FLASH_WAIT_TIMEOUT;

    while (FLASH_SR & FLASH_SR_BSY)
    {
        if (timeout == 0)
        {
            return FLASH_ERR_TIMEOUT;
        }
        timeout--;
    }

    return FLASH_OK;
}

static FLASH_Status FLASH_Get_Status(void)
{
    if (FLASH_SR & FLASH_SR_PGERR)
    {
        return FLASH_ERR_PROGRAM;
    }
    if (FLASH_SR & FLASH_SR_WRPRTERR)
    {
        return FLASH_ERR_WRITE_PROTECT;
    }

    return FLASH_OK;
}


static void FLASH_Unlock(void)
{
    FLASH_KEYR = FLASH_KEY1;
    FLASH_KEYR = FLASH_KEY2;
}

static void FLASH_Clear_Status(void)
{
    FLASH_SR = FLASH_SR_EOP | FLASH_SR_PGERR | FLASH_SR_WRPRTERR;
}

FLASH_Status FLASH_Erase_Page(uint32_t addr)
{
    FLASH_Status status;

    if ((addr % EEPROM_PAGE_SIZE) != 0 ||
        addr < EEPROM_PAGE_ADDR || addr > FLASH_MEMORY_END - EEPROM_PAGE_SIZE)
    {
        return FLASH_ERR_INVALID_ADDR;
    }

    status = FLASH_Wait_Busy();
    if (status != FLASH_OK)
    {
        return status;
    }

    FLASH_Unlock();
    FLASH_Clear_Status();

    FLASH_CR |= FLASH_CR_PER;
    FLASH_AR  = addr;
    FLASH_CR |= FLASH_CR_STRT;
    status = FLASH_Wait_Busy();
    if (status == FLASH_OK)
    {
        status = FLASH_Get_Status();
    }

    FLASH_CR &= ~(uint32_t)FLASH_CR_PER;
    FLASH_CR |= FLASH_CR_LOCK;

    return status;
}


FLASH_Status FLASH_Write_HW(uint32_t addr, uint16_t data)
{
    FLASH_Status status;

    if ((addr & 1u) != 0 || addr < EEPROM_PAGE_ADDR || addr > FLASH_MEMORY_END - 2u)
    {
        return FLASH_ERR_INVALID_ADDR;
    }

    status = FLASH_Wait_Busy();
    if (status != FLASH_OK)
    {
        return status;
    }

    FLASH_Unlock();
    FLASH_Clear_Status();

    FLASH_CR |= FLASH_CR_PG;
    *((volatile uint16_t *)addr) = data;
    status = FLASH_Wait_Busy();
    if (status == FLASH_OK)
    {
        status = FLASH_Get_Status();
    }

    FLASH_CR &= ~(uint32_t)FLASH_CR_PG;
    FLASH_CR |= FLASH_CR_LOCK;

    return status;
}

FLASH_Status FLASH_Read_HW(uint32_t addr, uint16_t *data)
{
    if (data == 0 || (addr & 1u) != 0 || addr < EEPROM_PAGE_ADDR ||
        addr > FLASH_MEMORY_END - 2u)
    {
        return FLASH_ERR_INVALID_ADDR;
    }
    *data = *((volatile uint16_t *)addr);
    return FLASH_OK;
}
