#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "ntimer.h"
#include "bsp_iwdg.h"
#include "bsp_rcc.h"
#include "bsp_led.h"
#include "bsp_debug_usart.h"
#include "bsp_chip_id.h"
#include "bsp_crc.h"
#include "bsp_rtc.h"
#include "bsp_io_board.h"
#include "fw_env.h"
#include "mbus_host.h"
#include "radio.h"
#include "main.h"
#include "date.h"
#include "bsp_di.h"
#include "lora_app.h"
#include "bsp_i2c.h"

#include <sys/process.h>
#include <sys/procinit.h>
#include <etimer.h>
#include <sys/autostart.h>
#include <clock.h>

#define osDelay(ms) \
    static struct etimer et; \
    etimer_set(&et, ms/(1000/CLOCK_SECOND)); \
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et))

unsigned int idle_count = 0;

mbus_sensor_t _sensors[SENSOR_MAX_CNT];
int sid_cur = 0;
__IO int is_busy = 0;
int mbus_retry_cnt  = 0;

int is_wakeup = 1;

PROCESS(watchdog_process, "Watchdog");
PROCESS(sensor_process, "Sensor");
#if CONFIG_LORA_TEST_MODE && CONFIG_LORA_TEST_TX
PROCESS(lora_test_tx_process, "LoRa_test_tx");
#endif /* CONFIG_LORA_TEST_MODE && CONFIG_LORA_TEST_TX */
AUTOSTART_PROCESSES(&watchdog_process);

process_event_t event_do_sensor_poll;

void show_banner(void)
{
    printf("\r\n\r\n============================================\r\n");
#if CONFIG_LORA_TEST_MODE
    printf("Project Name: LoRa client (Test Mode)\r\n");
#if CONFIG_LORA_TEST_TX
    printf("Test Mode: TX\r\n");
#else /* CONFIG_LORA_TEST_TX */
    printf("Test Mode: RX\r\n");
#endif /* !CONFIG_LORA_TEST_TX */
#else /* CONFIG_LORA_TEST_MODE*/
    printf("Project Name: LoRa client\r\n");
#endif /* !CONFIG_LORA_TEST_MODE*/
    printf("Author: Aron Li\r\n");
    printf("SW version: 2.3\r\n");
    printf("SystemCoreClock: %d\r\n", SystemCoreClock);
    printf("STM Chip Serial: %x-%x-%x\r\n", SID_GET_UNIQUE32(2), SID_GET_UNIQUE32(1), SID_GET_UNIQUE32(0));
    printf("============================================\r\n");
}

static void _sensor_data_parse(uint8_t *data, int size)
{
    mbus_sensor_t *sensor = &_sensors[sid_cur];

    if (size == (MBUS_MIN_DATA_SIZE + sensor->nb_reg * 2)) {
        switch (sensor->sensor_type) {
        case Sensor_Type_Light_Temp_Humidity:
            sensor->data[0].value = data[3] << 8 | data[4];
            sensor->data[1].value = (data[5] << 8 | data[6]) / 100.0f;
            sensor->data[2].value = (data[7] << 8 | data[8]) / 100.0f;
            break;
        case Sensor_Type_Irradiance:
            sensor->data[0].value = data[3] << 8 | data[4];
            break;
        case Sensor_Type_Soil_Temp_Humidity:
            if (data[3] == 0xFF) {
                // Negative
                sensor->data[0].value = (float) ((data[3] << 8) + data[4] - 0xFFFF - 1) / 100.0f;
            } else {
                //Positive
                sensor->data[0].value = (float) ((data[3] << 8) + data[4]) / 100.0f;
            }
            sensor->data[1].value= (data[5] << 8 | data[6]) / 100.0f;
            break;
        case Sensor_Type_CO2:
            sensor->data[0].value = data[3] << 8 | data[4];
            break;
        case Sensor_Type_Leaf_Humidity:
            sensor->data[0].value = (float) ((data[3] << 8) + data[4]) / 100.0f;
            break;
        case Sensor_Type_VOC:
            sensor->data[0].value = data[3] << 8 | data[4];
            break;
        case Sensor_Type_Air_Temp_Humidity_Light_RM5391B2:
            sensor->data[0].value = (data[3] << 8 | data[4]) / 100.0f;
            sensor->data[1].value = (data[5] << 8 | data[6]) / 100.0f;
            sensor->data[2].value = data[7] << 8 | data[8];
            break;
        case Sensor_Type_Water_Level_DDI:
            sensor->data[0].value = (data[3] << 8 | data[4]);
            break;
        case Sensor_Type_Soil_EC:
            sensor->data[0].value = (data[3] << 8 | data[4]);
            break;
        case Sensor_Type_Ground_Temp_Humidity_EC:
            if (data[3] == 0xFF) {
                // Negative
                sensor->data[0].value = (float) ((data[3] << 8) + data[4] - 0xFFFF - 1) / 100.0f;
            } else {
                //Positive
                sensor->data[0].value = (float) ((data[3] << 8) + data[4]) / 100.0f;
            }
            sensor->data[1].value= (data[5] << 8 | data[6]) / 100.0f;
            sensor->data[2].value= (data[7] << 8 | data[8]);
            break;
        }
    }
}

void my_master_rx_cb(uint8_t *data, int size)
{
    uint16_t crc;
    uint8_t addr;

    if (size < 4) {
        printf("[Modbus][INFO] Size error\r\n");
        return ;
    }

    crc = CRC16_Modbus(data, size);
    if (crc != 0) {
        printf("[Modbus][INFO] CRC error\r\n");
        return ;
    }

    addr = data[0];
    if (addr == _sensors[sid_cur].addr) {
        _sensors[sid_cur].is_recv = 1;
        _sensor_data_parse(data, size);
    }

    is_busy = 0;
}


void sensor_dump(void)
{
}


void app_wakeup(void)
{
    bsp_io_board_power_on();
    sid_cur = 0;
}

void wakeup_sensor_process(void)
{
    process_post(&sensor_process, event_do_sensor_poll, NULL);
}

/**
  * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
  *         and select PLL as system clock source.
  * @param  None
  * @retval None
  */
void SYSCLKConfig_STOP(void)
{
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08);
}

void time_show(void)
{
    struct rtc_time time;

    to_tm(RTC_GetCounter(), &time);
    printf("%04d/%02d/%02d %02d:%02d:%02d\r\n",
        time.tm_year, time.tm_mon, time.tm_mday,
        time.tm_hour, time.tm_min, time.tm_sec);
}

#if CONFIG_LORA_TEST_MODE && CONFIG_LORA_TEST_TX
PROCESS_THREAD(lora_test_tx_process, ev, data)
{
    PROCESS_BEGIN();
    while(1) {
        lora_mac_test_tx_send();
        osDelay(2000);
    }
    PROCESS_END();
}
#endif /* CONFIG_LORA_TEST_MODE && CONFIG_LORA_TEST_TX */

PROCESS_THREAD(watchdog_process, ev, data)
{
    PROCESS_BEGIN();
    while(1) {
        LED1_TOGGLE;
        iwdg_feed();
        osDelay(1000);
    }
    PROCESS_END();
}

PROCESS_THREAD(sensor_process, ev, data)
{
    PROCESS_BEGIN();
    static struct etimer mbus_timer;
    static uint32_t mbus_timeout;
    static int mbus_is_timeout;
    etimer_set(&mbus_timer, CLOCK_CONF_SECOND);
    event_do_sensor_poll = process_alloc_event();
    while (1) {
        printf("Send read (addr = %02x)\r\n", _sensors[sid_cur].addr);
        mbus_is_timeout = 0;

        mbus_timeout = MBUS_LONG_TIMEOUT;
        while (is_busy) {
            if ((mbus_timeout--) == 0) {
                break;
            }
        }

        // Clear is_recv flag
        _sensors[sid_cur].is_recv = 0;

        is_busy = 1;
        if (_sensors[sid_cur].sensor_type == Sensor_Type_CO2) {
            mbus_host_send(_sensors[sid_cur].addr, 0x4, _sensors[sid_cur].start_addr, _sensors[sid_cur].nb_reg);
        } else {
            mbus_host_send(_sensors[sid_cur].addr, 0x3, _sensors[sid_cur].start_addr, _sensors[sid_cur].nb_reg);
        }

        mbus_timeout = MBUS_LONG_TIMEOUT;
        while (is_busy) {
            if ((mbus_timeout--) == 0) {
                mbus_is_timeout = 1;
                break;
            }
        }

        if (!mbus_is_timeout) {
            sid_cur++;
            mbus_retry_cnt = 0;
        } else {
            ++mbus_retry_cnt;
            if (mbus_retry_cnt >= 2) {
                sid_cur++;
                mbus_retry_cnt = 0;
            }
        }

        if (sid_cur == SENSOR_MAX_CNT) {
            sensor_dump();
            etimer_stop(&mbus_timer);
            bsp_io_board_power_off();

            printf("Read finish! Wait next poll event\r\n");
            PROCESS_WAIT_EVENT_UNTIL(ev == event_do_sensor_poll);

            // Restart to read sensor
            bsp_io_board_power_on();
            printf("Restart to read sensor data, power on sensor...waiting 10 secs\r\n");
            osDelay(10000);
            sid_cur = 0;
            etimer_set(&mbus_timer, CLOCK_CONF_SECOND);
        } else {
            printf("Wait next timer event\r\n");
            etimer_reset(&mbus_timer);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        }
    }
    PROCESS_END();
}

int main(void)
{
#if CONFIG_RCC_HSI || CONFIG_RCC_HSE
    rcc_init();
#endif /* CONFIG_RCC_HSI || CONFIG_RCC_HSE */

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    // Eanble PWR clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);

    led_init();
    crc_init();
    debug_usart_init();
    di_init();
    show_banner();
    // bsp_rtc_init();
    bsp_io_board_init();
    // Disable watchdog for low power mode
    iwdg_init();
    clock_init();
    fw_env_open();
    fw_printenv();
    // Init I2C
    sensor_i2c_init();

    // Reset Wakeup FLAG
    PWR_ClearFlag (PWR_FLAG_WU);
    // Enable Wakup pin
    PWR_WakeUpPinCmd (ENABLE);

    // Power off io board
    bsp_io_board_power_on();

    //Modbus
    memset(_sensors, 0, sizeof(_sensors));

#if MODEL_DEFAULT
    /* Sensor_Type_Water_Level_DDI */
    _sensors[0].addr = 0x1;
    _sensors[0].sensor_type = Sensor_Type_Water_Level_DDI;
    _sensors[0].is_connect = 0;
    _sensors[0].is_recv = 0;
    _sensors[0].fail_cnt = 0;
    _sensors[0].nb_reg = 1;
    _sensors[0].nb_data = 1;
    _sensors[0].start_addr = 0x0000;
    _sensors[0].data = (sensing_data_t *) calloc(_sensors[0].nb_data, sizeof(sensing_data_t));
    _sensors[0].data[0].dc = Device_Class_Water_Level_DDI;

    /* Sensor_Type_Soil_EC */
    _sensors[1].addr = 0x2;
    _sensors[1].sensor_type = Sensor_Type_Soil_EC;
    _sensors[1].is_connect = 0;
    _sensors[1].is_recv = 0;
    _sensors[1].fail_cnt = 0;
    _sensors[1].nb_reg = 1;
    _sensors[1].nb_data = 1;
    _sensors[1].start_addr = 0x0002;
    _sensors[1].data = (sensing_data_t *) calloc(_sensors[1].nb_data, sizeof(sensing_data_t));
    _sensors[1].data[0].dc = Device_Class_EC;
#elif MODEL_SOIL_TEMP_HUMI_EC
    /* Sensor_Type_Ground_Temp_Humidity_EC */
    _sensors[0].addr = 0x1;
    _sensors[0].sensor_type = Sensor_Type_Ground_Temp_Humidity_EC;
    _sensors[0].is_connect = 0;
    _sensors[0].is_recv = 0;
    _sensors[0].fail_cnt = 0;
    _sensors[0].nb_reg = 3;
    _sensors[0].nb_data = 3;
    _sensors[0].start_addr = 0x0000;
    _sensors[0].data = (sensing_data_t *) calloc(_sensors[0].nb_data, sizeof(sensing_data_t));
    _sensors[0].data[0].dc = Device_Class_Temperature;
    _sensors[0].data[0].dc = Device_Class_Humidity;
    _sensors[0].data[0].dc = Device_Class_EC;
#endif

    mbus_host_init();
    mbus_host_register_rx_cb(my_master_rx_cb);

    process_init();
    process_start(&etimer_process, NULL);
    autostart_start(autostart_processes);

    lora_app_init();

#if CONFIG_LORA_TEST_MODE
#if CONFIG_LORA_TEST_TX
    process_start(&lora_test_tx_process, NULL);
#endif /* CONFIG_LORA_TEST_TX */
#else /* CONFIG_LORA_TEST_MODE */
                // Start sensr process
    process_start(&sensor_process, NULL);
#endif /* !CONFIG_LORA_TEST_MODE */

    printf("Processes running\r\n");

    while(1) {
        do {
        } while(process_run() > 0);
        idle_count++;
        /* Idle! */
        /* Stop processor clock */
        /* asm("wfi"::); */
    }
    return 0;
}

