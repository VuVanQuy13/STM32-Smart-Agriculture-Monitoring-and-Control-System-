#include <stdint.h>
#include "RCC.h"
#include "GPIO.h"
#include "TIM.h"
#include "ADC.h"
#include "button.h"
#include "scheduler.h"
#include "I2C.h"
#include "lcd_i2c.h"
#include "dht11.h"
#include "eeprom_cfg.h"
#include "relay_ctrl.h"
#include "lcd_sensors.h"
#include "lcd_thresholds.h"
#include "threshold_edit.h"
#include "analog_sample.h"
#include "fault_log.h"
#include "dht_sample.h"

#define CH_LIGHT 5
#define CH_SOIL  6

void main(void)
{
    Relay_Init();
    if (RCC_Config_72Mhz() != 0)
    {
        Fault_Log_Record(FAULT_BOOT_CLOCK, 1);
        while (1) { }
    }


    SysTick_Init();
    RCC_Enable_TIM2();
    RCC_Enable_TIM4();


    RCC_Enable_PortA();
    GPIO_Config(GPIOA, GPIO_PIN_5, GPIO_MODE_INPUT_ANALOG);
    GPIO_Config(GPIOA, GPIO_PIN_6, GPIO_MODE_INPUT_ANALOG);


    RCC_Enable_ADC01();
    if (ADC1_Init() != 0)
    {
        Fault_Log_Record(FAULT_BOOT_ADC, 1);
        while (1) { }
    }


    GPIO_Config(GPIOA, GPIO_PIN_0, GPIO_MODE_INPUT_FLOATING);


    RCC_Enable_PortB();
    GPIO_Config(GPIOB, GPIO_PIN_6, GPIO_MODE_AF_OD);
    GPIO_Config(GPIOB, GPIO_PIN_7, GPIO_MODE_AF_OD);
    RCC_Enable_I2C1();
    I2C_Init();


    uint8_t lcd1_ok = (LCD_Init(LCD1_ADDR) == 0);
    if (lcd1_ok && (LCD_Set_Cursor(LCD1_ADDR, 0, 0) != 0 ||
                    LCD_Print(LCD1_ADDR, "Hello") != 0))
    {
        lcd1_ok = 0;
    }


    uint8_t lcd2_ok = (LCD_Init(LCD2_ADDR) == 0);


    Button_Init();
    Scheduler_Init();


    Thresholds_t thresholds;
    EEPROM_Load(&thresholds);


    if (lcd2_ok && LCD2_Show_Thresholds(&thresholds, -1) != 0)
    {
        lcd2_ok = 0;
    }


    RCC_Enable_PortC();
    GPIO_Config_Speed(GPIOC, GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_2MHZ);

    uint32_t t_blink = 0;
    uint8_t  led_state = 0;
    AnalogSample light_sample = {0};
    AnalogSample soil_sample = {0};
    DHTSample dht_sample = {0};
    uint8_t displayed_validity = 0xFFu;


    while (1)
    {
        uint32_t now = millis();


        if (now - t_blink >= 500)
        {
            t_blink = now;
            led_state ^= 1;
            GPIO_Write_Pin(GPIOC, GPIO_PIN_13, led_state);
        }


        Button_Id button;
        if (Button_Pop(&button))
        {
            switch (button)
            {
                case BTN_MODE:
                    Relay_Toggle_Mode();
                    Relay_Refresh_Mode_Led();
                    break;
                case BTN_TOGGLE_PUMP: Relay_Toggle_Pump_Manual(); break;
                case BTN_TOGGLE_LIGHT: Relay_Toggle_Light_Manual(); break;
                case BTN_TOGGLE_FAN: Relay_Toggle_Fan_Manual(); break;
                default:
                    if (lcd2_ok && Threshold_Edit_Handle_Button(&thresholds, button)) lcd2_ok = 0;
                    break;
            }
        }
        uint32_t dropped = Button_Consume_Overflow();
        if (dropped) Fault_Log_Record(FAULT_BUTTON_OVERFLOW, dropped);


        if (lcd2_ok && Threshold_Edit_Process(&thresholds) != 0)
        {
            lcd2_ok = 0;
        }


        uint8_t sensor_refresh = Scheduler_Consume_Sensor_Ready();
        if (sensor_refresh)
        {
            uint16_t next_light = 0;
            uint16_t next_soil = 0;
            uint8_t light_status = ADC1_Read_Channel(CH_LIGHT, &next_light);
            Analog_Sample_Update(&light_sample, next_light, light_status, millis());
            uint8_t soil_status = ADC1_Read_Channel(CH_SOIL, &next_soil);
            Analog_Sample_Update(&soil_sample, next_soil, soil_status, millis());
            if (light_status) Fault_Log_Record(FAULT_ADC_LIGHT, light_status);
            if (soil_status) Fault_Log_Record(FAULT_ADC_SOIL, soil_status);

            DHT11_Data d = {0};
            DHT11_Status dht_status = DHT11_Read(&d);
            DHT_Sample_Update(&dht_sample, &d, dht_status, millis());
            if (dht_status != DHT11_OK) Fault_Log_Record(FAULT_DHT_READ, dht_status);


            if (lcd2_ok && !Threshold_Edit_Is_Active() && !Threshold_Edit_Has_Message())
            {
                lcd2_ok = (LCD2_Show_Thresholds(&thresholds, -1) == 0);
            }
        }

        now = millis();
        uint8_t light_valid = Analog_Sample_Is_Valid(&light_sample, now);
        uint8_t soil_valid = Analog_Sample_Is_Valid(&soil_sample, now);
        uint8_t dht_valid = DHT_Sample_Is_Valid(&dht_sample, now);
        if (dht_sample.valid && !dht_valid)
        {
            dht_sample.valid = 0;
            Fault_Log_Record(FAULT_DHT_EXPIRED, now - dht_sample.last_good_ms);
        }
        if ((light_sample.valid && !light_valid) || (soil_sample.valid && !soil_valid))
        {
            Fault_Log_Record(FAULT_ADC_EXPIRED,
                             (uint32_t)(light_sample.valid && !light_valid) |
                             ((uint32_t)(soil_sample.valid && !soil_valid) << 1));
            light_sample.valid = light_valid;
            soil_sample.valid = soil_valid;
        }
        Relay_Update(&thresholds, light_sample.value, soil_sample.value,
                     dht_sample.value.humidity, dht_sample.value.temperature,
                     dht_valid, light_valid, soil_valid);

        uint8_t validity = (uint8_t)(light_valid | (soil_valid << 1) | (dht_valid << 2));
        if (lcd1_ok && (sensor_refresh || validity != displayed_validity))
        {
            lcd1_ok = (LCD1_Show_Sensors(light_sample.value, soil_sample.value,
                                       dht_sample.value.humidity, dht_sample.value.temperature, dht_valid,
                                       light_valid, soil_valid) == 0);
            displayed_validity = validity;
        }
    }
}
