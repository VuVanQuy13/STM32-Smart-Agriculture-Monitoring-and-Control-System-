#include "button.h"
#include "RCC.h"
#include "GPIO.h"
#include "EXTI.h"
#include "AFIO.h"
#include "TIM.h"
#include "irq.h"


#define DEBOUNCE_MS 20u

static volatile Button_Id button_queue[BUTTON_QUEUE_CAPACITY];
static volatile uint32_t queue_head, queue_tail, dropped_events;
typedef struct {
    uint32_t changed_ms;
    uint8_t raw_high;
    uint8_t stable_high;
} ButtonState;
static volatile ButtonState button_state[BTN_COUNT];
static volatile uint8_t initialized;

static void Button_Enqueue(Button_Id id)
{
    if ((unsigned)id >= BTN_COUNT) return;
    if ((uint32_t)(queue_head - queue_tail) >= BUTTON_QUEUE_CAPACITY)
    {
        /* Keep accepted events in order; count rejected newest events. */
        if (dropped_events != UINT32_MAX) dropped_events++;
        return;
    }
    button_queue[queue_head % BUTTON_QUEUE_CAPACITY] = id;
    queue_head++;
}


typedef struct
{
    volatile GPIO_Typedef *port;
    uint16_t pin;
    uint8_t exti_line;
} ButtonMap;

static const ButtonMap btn_map[BTN_COUNT] = {
    [BTN_EDIT]         = { GPIOA, GPIO_PIN_8,  8  },
    [BTN_RST]          = { GPIOA, GPIO_PIN_9,  9  },
    [BTN_SAVE]         = { GPIOA, GPIO_PIN_10, 10 },
    [BTN_UP]           = { GPIOB, GPIO_PIN_12, 12 },
    [BTN_DOWN]         = { GPIOB, GPIO_PIN_13, 13 },
    [BTN_NEXT]         = { GPIOB, GPIO_PIN_14, 14 },
    [BTN_PREV]         = { GPIOB, GPIO_PIN_15, 15 },
    [BTN_TOGGLE_PUMP]  = { GPIOB, GPIO_PIN_3,  3  },
    [BTN_TOGGLE_LIGHT] = { GPIOB, GPIO_PIN_4,  4  },
    [BTN_TOGGLE_FAN]   = { GPIOB, GPIO_PIN_5,  5  },
    [BTN_MODE]         = { GPIOB, GPIO_PIN_0,  0  },
};


static void handle_exti(uint8_t line)
{
    if (!initialized) return;
    uint32_t mask = IRQ_Save();
    for (uint8_t i = 0; i < BTN_COUNT; i++)
    {
        if (btn_map[i].exti_line == line)
        {
            button_state[i].raw_high = GPIO_Read_Pin(btn_map[i].port, btn_map[i].pin);
            button_state[i].changed_ms = millis();
            break;
        }
    }
    IRQ_Restore(mask);
}

void SysTick_App_Tick(void)
{
    if (!initialized) return;
    uint32_t mask = IRQ_Save();
    uint32_t now = millis();
    for (uint8_t i = 0; i < BTN_COUNT; i++)
    {
        uint8_t high = GPIO_Read_Pin(btn_map[i].port, btn_map[i].pin);
        if (high != button_state[i].raw_high)
        {
            button_state[i].raw_high = high;
            button_state[i].changed_ms = now;
        }
        else if (high != button_state[i].stable_high &&
                 (uint32_t)(now - button_state[i].changed_ms) >= DEBOUNCE_MS)
        {
            /* A stable release must precede the next accepted press. */
            button_state[i].stable_high = high;
            if (!high) Button_Enqueue((Button_Id)i);
        }
    }
    IRQ_Restore(mask);
}

void Button_Init(void)
{
    uint32_t mask = IRQ_Save();
    initialized = 0;
    queue_head = queue_tail = dropped_events = 0;

    RCC_Enable_PortA();
    RCC_Enable_PortB();
    RCC_Enable_AFIO();
    AFIO_Disable_JTAG_Enable_SWD();


    for (uint8_t i = 0; i < BTN_COUNT; i++)
    {
        GPIO_Config(btn_map[i].port, btn_map[i].pin, GPIO_MODE_INPUT_PU);
        button_state[i].raw_high = GPIO_Read_Pin(btn_map[i].port, btn_map[i].pin);
        button_state[i].stable_high = button_state[i].raw_high;
        button_state[i].changed_ms = millis();
        EXTI_Init(btn_map[i].pin, btn_map[i].port, EXTI_BOTH_MODE);
        EXTI->PR.REG = btn_map[i].pin;
    }
    initialized = 1;
    IRQ_Restore(mask);
}

uint8_t Button_Pop(Button_Id *id)
{
    if (id == 0) return 0;
    uint32_t mask = IRQ_Save();
    uint8_t ready = queue_head != queue_tail;
    if (ready)
    {
        *id = button_queue[queue_tail % BUTTON_QUEUE_CAPACITY];
        queue_tail++;
    }
    IRQ_Restore(mask);
    return ready;
}

uint32_t Button_Consume_Overflow(void)
{
    uint32_t mask = IRQ_Save();
    uint32_t count = dropped_events;
    dropped_events = 0;
    IRQ_Restore(mask);
    return count;
}


void EXTI0_IRQHandler(void)
{
    if (EXTI->PR.REG & (1 << 0))
    {
        EXTI->PR.REG = (1 << 0);
        handle_exti(0);
    }
}

void EXTI3_IRQHandler(void)
{
    if (EXTI->PR.REG & (1 << 3))
    {
        EXTI->PR.REG = (1 << 3);
        handle_exti(3);
    }
}

void EXTI4_IRQHandler(void)
{
    if (EXTI->PR.REG & (1 << 4))
    {
        EXTI->PR.REG = (1 << 4);
        handle_exti(4);
    }
}

void EXTI9_5_IRQHandler(void)
{
    for (uint8_t line = 5; line <= 9; line++)
    {
        if (EXTI->PR.REG & (1 << line))
        {
            EXTI->PR.REG = (1 << line);
            handle_exti(line);
        }
    }
}

void EXTI15_10_IRQHandler(void)
{
    for (uint8_t line = 10; line <= 15; line++)
    {
        if (EXTI->PR.REG & (1 << line))
        {
            EXTI->PR.REG = (1 << line);
            handle_exti(line);
        }
    }
}
