#include "relay_ctrl.h"
#include "GPIO.h"
#include "RCC.h"
#include "analog_sample.h"


#define PIN_RELAY_PUMP   GPIO_PIN_2
#define PIN_RELAY_LIGHT  GPIO_PIN_3
#define PIN_RELAY_FAN    GPIO_PIN_4
#define PIN_LED_MODE     GPIO_PIN_1

#define ADC_HYSTERESIS       50u
#define TEMP_HYSTERESIS       1u
#define HUMIDITY_HYSTERESIS   1u

typedef enum { MODE_AUTO = 0, MODE_MANUAL } SystemMode;
static volatile SystemMode s_mode = MODE_AUTO;

void Relay_Init(void)
{

    RCC_Enable_PortA();
    RCC_Enable_PortB();


    GPIO_Write_Pin(GPIOA, PIN_RELAY_PUMP,  0);
    GPIO_Write_Pin(GPIOA, PIN_RELAY_LIGHT, 0);
    GPIO_Write_Pin(GPIOA, PIN_RELAY_FAN,   0);
    GPIO_Config(GPIOA, PIN_RELAY_PUMP,  GPIO_MODE_OUTPUT_PP);
    GPIO_Config(GPIOA, PIN_RELAY_LIGHT, GPIO_MODE_OUTPUT_PP);
    GPIO_Config(GPIOA, PIN_RELAY_FAN,   GPIO_MODE_OUTPUT_PP);


    GPIO_Config(GPIOB, PIN_LED_MODE, GPIO_MODE_OUTPUT_PP);
    GPIO_Write_Pin(GPIOB, PIN_LED_MODE, 1);


}

void Relay_Toggle_Mode(void)
{
    s_mode = (s_mode == MODE_AUTO) ? MODE_MANUAL : MODE_AUTO;
}

static void Relay_Toggle_Pin(volatile GPIO_Typedef *port, uint16_t pin)
{
    uint8_t state = (port->ODR.REG & pin) ? 0 : 1;
    GPIO_Write_Pin(port, pin, state);
}

void Relay_Toggle_Pump_Manual(void)
{
    if (s_mode == MODE_MANUAL)
    {
        Relay_Toggle_Pin(GPIOA, PIN_RELAY_PUMP);
    }
}

void Relay_Toggle_Light_Manual(void)
{
    if (s_mode == MODE_MANUAL)
    {
        Relay_Toggle_Pin(GPIOA, PIN_RELAY_LIGHT);
    }
}

void Relay_Toggle_Fan_Manual(void)
{
    if (s_mode == MODE_MANUAL)
    {
        Relay_Toggle_Pin(GPIOA, PIN_RELAY_FAN);
    }
}

static uint8_t Relay_Analog_On(uint16_t value, uint16_t threshold,
                               uint8_t valid, uint8_t was_on, uint8_t inclusive)
{
    uint32_t off_threshold = (uint32_t)threshold + ADC_HYSTERESIS;
    if (!valid || value > ANALOG_MAX_VALUE || threshold > ANALOG_MAX_VALUE) return 0;
    if (off_threshold > ANALOG_MAX_VALUE) off_threshold = ANALOG_MAX_VALUE;
    /* Uu tien tat tai bien tren, tranh dao lien tuc o muc 4095. */
    if (value >= off_threshold) return 0;
    if (was_on) return 1;
    return inclusive ? (value <= threshold) : (value < threshold);
}

void Relay_Update(const Thresholds_t *th,
                  uint16_t adc_light, uint16_t adc_soil,
                  uint8_t dht_humidity, uint8_t dht_temp, uint8_t dht_valid,
                  uint8_t light_valid, uint8_t soil_valid)
{
    uint8_t pump_on;
    uint8_t light_on;
    uint8_t fan_on;

    if (s_mode == MODE_MANUAL) {

        return;
    }
    if (th == 0)
    {
        return;
    }

    pump_on = ((GPIOA)->ODR.REG & PIN_RELAY_PUMP) ? 1 : 0;
    pump_on = Relay_Analog_On(adc_soil, th->soil, soil_valid, pump_on, 1);

    light_on = ((GPIOA)->ODR.REG & PIN_RELAY_LIGHT) ? 1 : 0;
    light_on = Relay_Analog_On(adc_light, th->light, light_valid, light_on, 0);


    fan_on = 0;
    if (dht_valid) {
        if ((GPIOA)->ODR.REG & PIN_RELAY_FAN)
        {
            uint8_t temp_off = (th->temp > TEMP_HYSTERESIS) ? (uint8_t)(th->temp - TEMP_HYSTERESIS) : 0;
            uint8_t humidity_off = (th->humidity > HUMIDITY_HYSTERESIS) ? (uint8_t)(th->humidity - HUMIDITY_HYSTERESIS) : 0;
            fan_on = (dht_temp > temp_off || dht_humidity > humidity_off) ? 1 : 0;
        }
        else if (dht_temp > th->temp || dht_humidity > th->humidity)
        {
            fan_on = 1;
        }
    }

    GPIO_Write_Pin(GPIOA, PIN_RELAY_PUMP,  pump_on);
    GPIO_Write_Pin(GPIOA, PIN_RELAY_LIGHT, light_on);
    GPIO_Write_Pin(GPIOA, PIN_RELAY_FAN,   fan_on);
}

void Relay_Refresh_Mode_Led(void)
{

    GPIO_Write_Pin(GPIOB, PIN_LED_MODE, (s_mode == MODE_AUTO) ? 1 : 0);
}

uint8_t Relay_Is_Manual(void)
{
    return (s_mode == MODE_MANUAL);
}
