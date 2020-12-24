#include "bsp_max44009.h"
#include "bsp_i2c.h"
#include "errno.h"
#include <stdio.h>
#include <math.h>

#if CONFIG_DEMETER_SENSOR_LIGHT

static int i2c_failed_read_count = 0;

static uint32_t _max44009_calc(uint8_t lux_hi, uint8_t lux_low)
{
        uint8_t exp;
        uint8_t remainder;

        exp = (lux_hi & 0xF0) >> 4;
        remainder = (lux_hi & 0x0F) << 4 | (lux_low & 0x0F);

        return (uint32_t) (pow(2, exp) * remainder * 0.045);
}

uint32_t max44009_lux_read(uint32_t *data)
{
        uint8_t lux_hi, lux_low;
        int rv;

	rv = sensor_i2c_buffer_read(MAX44009_ADDR, MAX44009_LUX_HI, 1, &lux_hi);
	if (rv != NCORE_ERR_OK) {
		goto fail;
	}

	rv = sensor_i2c_buffer_read(MAX44009_ADDR, MAX44009_LUX_LOW, 1, &lux_low);

fail:
        if (rv == NCORE_ERR_OK) {
                //printf("HI = %02x, LOW = %02x, result = %u lux\r\n", lux_hi, lux_low, _max44009_calc(lux_hi, lux_low));
                *data = _max44009_calc(lux_hi, lux_low);
                i2c_failed_read_count = 0;
        } else {
                if (++i2c_failed_read_count > 5) {
                        printf("\r\n[I2C][INFO] I2C failed, reboot system\r\n");
                        NVIC_SystemReset();
                }
        }

        return rv;
}

#endif /* CONFIG_DEMETER_SENSOR_LIGHT */

