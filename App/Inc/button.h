#ifndef __BUTTON_H
#define __BUTTON_H

#include <stdint.h>


typedef enum
{
    BTN_EDIT = 0,
    BTN_RST,
    BTN_SAVE,
    BTN_UP,
    BTN_DOWN,
    BTN_NEXT,
    BTN_PREV,
    BTN_TOGGLE_PUMP,
    BTN_TOGGLE_LIGHT,
    BTN_TOGGLE_FAN,
    BTN_MODE,
    BTN_COUNT
} Button_Id;


#define BUTTON_QUEUE_CAPACITY 32u


/* Starts 20 ms stable press/release filtering via the SysTick application hook. */
void Button_Init(void);


uint8_t Button_Pop(Button_Id *id);
uint32_t Button_Consume_Overflow(void);

#endif
