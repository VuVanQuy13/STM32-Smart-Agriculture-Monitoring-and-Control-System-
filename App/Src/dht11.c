#include "dht11.h"
#include "GPIO.h"
#include "TIM.h"
#include "irq.h"

#define DHT11_PORT   GPIOA
#define DHT11_PIN    GPIO_PIN_0


static void DHT11_Pin_Output(void)
{
    GPIO_Config(DHT11_PORT, DHT11_PIN, GPIO_MODE_OUTPUT_OD);
}


static void DHT11_Pin_Input(void)
{
    GPIO_Config(DHT11_PORT, DHT11_PIN, GPIO_MODE_INPUT_PU);
}


static uint16_t DHT11_Wait_Level(uint8_t level, uint16_t timeout_us)
{
    uint16_t start = TIM4_Get_us();
    uint32_t polls_left = 4096u;
    while (GPIO_Read_Pin(DHT11_PORT, DHT11_PIN) != level)
    {
        /* Independent bound also terminates when TIM4 no longer advances. */
        if (--polls_left == 0 || (uint16_t)(TIM4_Get_us() - start) >= timeout_us)
        {
            return 0xFFFF;
        }
    }
    return (uint16_t)(TIM4_Get_us() - start);
}


/* Measured windows include polling tolerance; verify margins on the board. */
#define DHT11_RESPONSE_MIN_US 60u
#define DHT11_RESPONSE_MAX_US 100u
#define DHT11_DATA_LOW_MIN_US 35u
#define DHT11_DATA_LOW_MAX_US 70u
#define DHT11_ZERO_MIN_US 15u
#define DHT11_ZERO_MAX_US 40u
#define DHT11_ONE_MIN_US 55u
#define DHT11_ONE_MAX_US 90u

static DHT11_Status DHT11_Wait_Pulse(uint8_t end_level, uint16_t min_us,
                                    uint16_t max_us, uint16_t *duration)
{
    uint16_t width = DHT11_Wait_Level(end_level, 100);
    if (width == 0xFFFF) return DHT11_ERR_TIMEOUT;
    if (width < min_us || width > max_us) return DHT11_ERR_TIMING;
    if (duration != 0) *duration = width;
    return DHT11_OK;
}

DHT11_Status DHT11_Read(DHT11_Data *out)
{
    uint8_t bytes[5] = {0, 0, 0, 0, 0};
    int i;
    uint32_t primask;
    DHT11_Status status = DHT11_OK;

    if (out == 0)
    {
        return DHT11_ERR_INVALID_ARG;
    }

    GPIO_Write_Pin(DHT11_PORT, DHT11_PIN, 0);
    DHT11_Pin_Output();
    if (delay_ms(20) != 0)
    {
        DHT11_Pin_Input();
        return DHT11_ERR_DELAY;
    }
    primask = IRQ_Save();
    TIM4_Start_Free();
    /* Open-drain high releases DATA; the external pull-up raises the bus. */
    GPIO_Write_Pin(DHT11_PORT, DHT11_PIN, 1);
    if (DHT11_Wait_Level(1, 100) == 0xFFFF) { status = DHT11_ERR_TIMEOUT; goto done; }

    if (DHT11_Wait_Level(0, 100) == 0xFFFF) { status = DHT11_ERR_TIMEOUT; goto done; }
    status = DHT11_Wait_Pulse(1, DHT11_RESPONSE_MIN_US, DHT11_RESPONSE_MAX_US, 0);
    if (status != DHT11_OK) goto done;
    status = DHT11_Wait_Pulse(0, DHT11_RESPONSE_MIN_US, DHT11_RESPONSE_MAX_US, 0);
    if (status != DHT11_OK) goto done;


    for (i = 0; i < 40; i++)
    {

        status = DHT11_Wait_Pulse(1, DHT11_DATA_LOW_MIN_US, DHT11_DATA_LOW_MAX_US, 0);
        if (status != DHT11_OK) goto done;


        uint16_t high_us;
        status = DHT11_Wait_Pulse(0, DHT11_ZERO_MIN_US, DHT11_ONE_MAX_US, &high_us);
        if (status != DHT11_OK) goto done;
        if (high_us > DHT11_ZERO_MAX_US && high_us < DHT11_ONE_MIN_US)
        {
            status = DHT11_ERR_TIMING;
            goto done;
        }

        bytes[i / 8] <<= 1;
        if (high_us >= DHT11_ONE_MIN_US)
        {
            bytes[i / 8] |= 1;
        }
    }

done:
    DHT11_Pin_Input();
    IRQ_Restore(primask);

    if (status != DHT11_OK)
    {
        return status;
    }


    uint8_t sum = (uint8_t)(bytes[0] + bytes[1] + bytes[2] + bytes[3]);
    if (sum != bytes[4])
    {
        return DHT11_ERR_CHECKSUM;
    }

    DHT11_Data candidate = { bytes[0], bytes[2] };
    /* Reject unsupported signed/invalid fractional encodings before truncation. */
    if (!DHT11_Data_Is_Valid(&candidate) || bytes[1] > 9u || bytes[3] > 9u ||
        (bytes[0] == DHT11_MAX_HUMIDITY && bytes[1] != 0) ||
        (bytes[2] == DHT11_MAX_TEMP_C && bytes[3] != 0))
    {
        return DHT11_ERR_RANGE;
    }
    *out = candidate;
    return DHT11_OK;
}
