#include "I2C.h"
#include "RCC.h"
#include "fault_log.h"

#define I2C_WAIT_TIMEOUT 100000UL
#define I2C_PENDING ((1u << 8) | (1u << 9) | (1u << 12))
#define I2C_ERRORS ((1u << 8) | (1u << 9) | (1u << 10) | (1u << 11))

static uint8_t I2C_Wait_Control_Clear(void)
{
    uint32_t timeout = I2C_WAIT_TIMEOUT;
    while (I2C1_CR1 & I2C_PENDING)
    {
        if (timeout-- == 0) return 1;
    }
    return 0;
}

static uint8_t I2C_Wait_Idle(void)
{
    uint32_t timeout = I2C_WAIT_TIMEOUT;
    while (I2C1_SR2 & (1u << 1))
    {
        if (timeout-- == 0) return 1;
    }
    return 0;
}

static void I2C_Reset(void)
{
    /* Reset qua RCC, khong sua CR1 khi START/STOP con dang cho. */
    RCC->APB1RSTR.BITS.I2C1RST = 1;
    RCC->APB1RSTR.BITS.I2C1RST = 0;
    I2C_Init();
}

static uint8_t I2C_Fail(void)
{
    Fault_Log_Record(FAULT_I2C, I2C1_SR1);
    if (!(I2C1_CR1 & I2C_PENDING) && (I2C1_SR2 & 1u))
    {
        if (I2C_Stop() == 0) I2C1_SR1 &= ~I2C_ERRORS;
    }
    else
    {
        I2C_Reset();
    }
    return 1;
}

static uint8_t I2C_Wait_Flag(uint32_t flag)
{
	uint32_t timeout = I2C_WAIT_TIMEOUT;
    while (1)
    {
        uint32_t status = I2C1_SR1;
        if (status & I2C_ERRORS) return 1;
        if (status & flag) return 0;
        if (timeout-- == 0) return 1;
    }
}

void I2C_Init(void){


	I2C1_CR1 &= ~(uint32_t)(1 << 0);
	/* RM0008 requires OAR1 bit 14 even for a master-only bus. */
	I2C1_OAR1 = (1u << 14);
	I2C1_CR2 = 36;
	I2C1_CCR = 180;
	I2C1_TRISE = 37;
	I2C1_CR1 |= (1 << 0);
}
uint8_t I2C_Start(void){

	if (I2C_Wait_Control_Clear() || I2C_Wait_Idle()) return I2C_Fail();
	I2C1_SR1 &= ~I2C_ERRORS;
	I2C1_CR1 |= (1 << 8);
	return I2C_Wait_Flag(0x01) ? I2C_Fail() : 0;

}
uint8_t I2C_Stop(void){
    if (I2C_Wait_Control_Clear() == 0)
    {
        I2C1_CR1 |= (1u << 9);
        if (I2C_Wait_Control_Clear() == 0 && I2C_Wait_Idle() == 0) return 0;
    }
    Fault_Log_Record(FAULT_I2C, 0x10000u | I2C1_SR1);
    I2C_Reset();
    return 1;
}
uint8_t I2C_Send_Addr(uint8_t addr, uint8_t rw){
	/* LCD transport is transmit-only; no partial receive API is exposed. */
	if (addr > 0x7Fu || rw != 0u) return I2C_Fail();
	I2C1_DR = (uint32_t)((addr << 1) | (rw & 0x01));
	if(I2C_Wait_Flag(1 << 1)){
		return I2C_Fail();
	}
	(void)I2C1_SR2;
	return 0;
}

uint8_t I2C_Send_Data(uint8_t data){

	I2C1_DR = data;
	if(I2C_Wait_Flag(1 << 2)){
		return I2C_Fail();
	}
	return 0;

}


