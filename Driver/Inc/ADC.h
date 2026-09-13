#ifndef __ADC_H
#define __ADC_H
#include <stdint.h>

#define ADC01_BASE     0x40012400
#define ADC02_BASE 		 0x40012800


#define ADD_ADC_SR 		    0x00
#define ADD_ADC_CR1 		0x04
#define ADD_ADC_CR2 		0x08
#define ADD_ADC_SMPR1 	    0x0C
#define ADD_ADC_SMPR2		0x10
#define ADD_ADC_JOFR1 	    0x14
#define ADD_ADC_JOFR2		0x18
#define ADD_ADC_JOFR3 	    0x1C
#define ADD_ADC_JOFR4 	    0x20
#define ADD_ADC_HTR 		0x24
#define ADD_ADC_LTR 		0x28
#define ADD_ADC_SQR1 		0x2C
#define ADD_ADC_SQR2 		0x30
#define ADD_ADC_SQR3 		0x34
#define ADD_ADC_JSQR 		0x38
#define ADD_ADC_JDR1 		0x3C
#define ADD_ADC_JDR2 		0x40
#define ADD_ADC_JDR3 		0x44
#define ADD_ADC_JDR4 		0x48
#define ADD_ADC_DR 		    0x4C


#define ADC01_SR 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_SR    ))
#define ADC01_CR1 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_CR1   ))
#define ADC01_CR2 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_CR2   ))
#define ADC01_SMPR1 	*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_SMPR1 ))
#define ADC01_SMPR2		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_SMPR2 ))
#define ADC01_JOFR1 	*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_JOFR1 ))
#define ADC01_JOFR2		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_JOFR2 ))
#define ADC01_JOFR3 	*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_JOFR3 ))
#define ADC01_JOFR4 	*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_JOFR4 ))
#define ADC01_HTR 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_HTR   ))
#define ADC01_LTR 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_LTR   ))
#define ADC01_SQR1 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_SQR1  ))
#define ADC01_SQR2 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_SQR2  ))
#define ADC01_SQR3 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_SQR3  ))
#define ADC01_JSQR 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_JSQR  ))
#define ADC01_JDR1 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_JDR1  ))
#define ADC01_JDR2 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_JDR2  ))
#define ADC01_JDR3 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_JDR3  ))
#define ADC01_JDR4 		*((volatile uint32_t*)(ADC01_BASE+ADD_ADC_JDR4  ))
#define ADC01_DR 		  *((volatile uint32_t*)(ADC01_BASE+ADD_ADC_DR   ))


uint8_t ADC1_Init(void);
uint8_t ADC1_Read_Channel(uint8_t ch, uint16_t *out);

#endif


