#include "bsp_ina219.h"
#include "bsp_i2c.h"
#include "errno.h"
#include "bsp_rcc.h"

uint32_t calib_val;
uint32_t current_divider_ma;
float power_multiplier_mw;

static uint16_t _htons(uint16_t xx)
{
	uint16_t yy;

	yy = (uint16_t)((xx & 0x00ff) << 8);
	yy |= (uint16_t)((xx & 0xff00) >> 8);

	return yy;
}

int bsp_ina219_shunt_volt_get(float *voltag)
{
	uint16_t data = 0;
	int16_t val;
	int rv;

	rv = sensor_i2c_buffer_read(INA219_SLAVE_ADDR, INA219_SHUNT_VOLT_REG, sizeof(uint16_t), (uint8_t *) &data);
	if (rv != NCORE_ERR_OK) {
		return rv;
	}

	val = (int16_t) _htons(data);
	// LSB is 10uV, convert to mV
	*voltag = val * 0.01;

	return NCORE_ERR_OK;
}

int bsp_ina219_bus_volt_get(float *voltag)
{
	uint16_t data = 0;
	int16_t val;
	int rv;

	rv = sensor_i2c_buffer_read(INA219_SLAVE_ADDR, INA219_BUS_VOLT_REG, sizeof(uint16_t), (uint8_t *) &data);
	if (rv != NCORE_ERR_OK) {
		return rv;
	}

	// Shift to the right 3 to drop CNVR and OVF and multiply by LSB (4mV)
	val = (_htons(data) >> 3) * 4;
	*voltag = val * 0.001;

	return NCORE_ERR_OK;
}

void bsp_ina219_config_get(ina219_config_t *config)
{
	sensor_i2c_buffer_read(INA219_SLAVE_ADDR, INA219_CONF_REG, sizeof(config), (uint8_t *) config);
	config->val = _htons(config->val);
}

void bsp_ina219_config_set(ina219_config_t config)
{
	config.val = _htons(config.val);
	sensor_i2c_byte_write(INA219_SLAVE_ADDR, INA219_CONF_REG, sizeof(config), (uint8_t *) &config);
}

void bsp_ina219_calib_get(uint16_t *data)
{
	sensor_i2c_buffer_read(INA219_SLAVE_ADDR, INA219_CALIB_REG, sizeof(uint16_t), (uint8_t *) data);
	*data = _htons(*data);
}

void bsp_ina219_calib_set(uint16_t data)
{
	uint16_t tmp = _htons(data);
	sensor_i2c_byte_write(INA219_SLAVE_ADDR, INA219_CALIB_REG, sizeof(uint16_t), (uint8_t *) &tmp);
}

/*
 *  @brief  Gets the current value in mA, taking into account the
 *          config settings and current LSB
 *  @return the current reading convereted to milliamps
 */
int bsp_ina219_current_get(float *current)
{
	uint16_t data;
	int16_t val;
	int rv;

	// Sometimes a sharp load will reset the INA219, which will
	// reset the cal register, meaning CURRENT and POWER will
	// not be available ... avoid this by always setting a cal
	// value even if it's an unfortunate extra step
	bsp_ina219_calib_set(calib_val);

	// Now we can safely read the CURRENT register!
	rv = sensor_i2c_buffer_read(INA219_SLAVE_ADDR, INA219_CURRENT_REG, sizeof(uint16_t), (uint8_t *) &data);
	if (rv != NCORE_ERR_OK) {
		return rv;
	}

	val = (int16_t) _htons(data);

	*current = (float) val / current_divider_ma;

	return rv;
}

/*
 *  @brief  Gets the power value in mW, taking into account the
 *          config settings and current LSB
 *  @return power reading converted to milliwatts
 */
int bsp_ina219_power_get(float *power)
{
	uint16_t data;
	int16_t val;
	int rv;

	// Sometimes a sharp load will reset the INA219, which will
	// reset the cal register, meaning CURRENT and POWER will
	// not be available ... avoid this by always setting a cal
	// value even if it's an unfortunate extra step
	bsp_ina219_calib_set(calib_val);

	// Now we can safely read the POWER register!
	rv = sensor_i2c_buffer_read(INA219_SLAVE_ADDR, INA219_PWR_REG, sizeof(uint16_t), (uint8_t *) &data);
	if (rv != NCORE_ERR_OK) {
		return rv;
	}

	val = (int16_t) _htons(data);

	*power = val * power_multiplier_mw;

	return rv;
}

/*
 *  @brief set device to alibration which uses the highest precision for
 *     current measurement (0.1mA), at the expense of
 *     only supporting 16V at 400mA max.
 */
void _ina219_set_calib_16v_400ma(void)
{
	ina219_config_t config;
	// Calibration which uses the highest precision for
	// current measurement (0.1mA), at the expense of
	// only supporting 16V at 400mA max.

	// VBUS_MAX = 16V
	// VSHUNT_MAX = 0.04          (Assumes Gain 1, 40mV)
	// RSHUNT = 0.1               (Resistor value in ohms)

	// 1. Determine max possible current
	// MaxPossible_I = VSHUNT_MAX / RSHUNT
	// MaxPossible_I = 0.4A

	// 2. Determine max expected current
	// MaxExpected_I = 0.4A

	// 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
	// MinimumLSB = MaxExpected_I/32767
	// MinimumLSB = 0.0000122              (12uA per bit)
	// MaximumLSB = MaxExpected_I/4096
	// MaximumLSB = 0.0000977              (98uA per bit)

	// 4. Choose an LSB between the min and max values
	//    (Preferrably a roundish number close to MinLSB)
	// CurrentLSB = 0.00005 (50uA per bit)

	// 5. Compute the calibration register
	// Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
	// Cal = 8192 (0x2000)

	calib_val = 8192;

	// 6. Calculate the power LSB
	// PowerLSB = 20 * CurrentLSB
	// PowerLSB = 0.001 (1mW per bit)

	// 7. Compute the maximum current and shunt voltage values before overflow
	//
	// Max_Current = Current_LSB * 32767
	// Max_Current = 1.63835A before overflow
	//
	// If Max_Current > Max_Possible_I then
	//    Max_Current_Before_Overflow = MaxPossible_I
	// Else
	//    Max_Current_Before_Overflow = Max_Current
	// End If
	//
	// Max_Current_Before_Overflow = MaxPossible_I
	// Max_Current_Before_Overflow = 0.4
	//
	// Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
	// Max_ShuntVoltage = 0.04V
	//
	// If Max_ShuntVoltage >= VSHUNT_MAX
	//    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
	// Else
	//    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
	// End If
	//
	// Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
	// Max_ShuntVoltage_Before_Overflow = 0.04V

	// 8. Compute the Maximum Power
	// MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
	// MaximumPower = 0.4 * 16V
	// MaximumPower = 6.4W

	// Set multipliers to convert raw current/power values
	current_divider_ma = 20;    // Current LSB = 50uA per bit (1000/50 = 20)
	power_multiplier_mw = 1.0f; // Power LSB = 1mW per bit

	// Set Calibration register to 'Cal' calculated above
	bsp_ina219_calib_set(calib_val);

	config.fields.brng = INA219_BRNG_16V;
	config.fields.pg = INA219_PG_1_40MV;
	config.fields.badc = INA219_ADCRES_12BIT_532US;
	config.fields.sadc = INA219_ADCRES_12BIT_532US;
	config.fields.mode = INA219_MODE_S_B_VOLT_CONTINUOUS;
	bsp_ina219_config_set(config);
}

/*
 *  @brief  Configures to INA219 to be able to measure up to 16V and 0.8A
 *          of current.  Each unit of current corresponds to 40uA, and each
 *          unit of power corresponds to 800uW. Counter overflow occurs at
 *          1.3A.
 *  @note   These calculations assume a 0.1 ohm resistor is present
 */
void _ina219_set_calib_16v_800ma(void)
{
	ina219_config_t config;
	// By default we use a pretty huge range for the input voltage,
	// which probably isn't the most appropriate choice for system
	// that don't use a lot of power.  But all of the calculations
	// are shown below if you want to change the settings.  You will
	// also need to change any relevant register settings, such as
	// setting the VBUS_MAX to 16V instead of 32V, etc.

	// VBUS_MAX = 16V		(Assumes 32V, can also be set to 16V)
	// VSHUNT_MAX = 0.08	(Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
	// RSHUNT = 0.1			(Resistor value in ohms)

	// 1. Determine max possible current
	// MaxPossible_I = VSHUNT_MAX / RSHUNT
	// MaxPossible_I = 0.8A

	// 2. Determine max expected current
	// MaxExpected_I = 0.8A

	// 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
	// MinimumLSB = MaxExpected_I/32767
	// MinimumLSB = 0.000024415             (24.4uA per bit)
	// MaximumLSB = MaxExpected_I/4096
	// MaximumLSB = 0.000195313              (195uA per bit)

	// 4. Choose an LSB between the min and max values
	//    (Preferrably a roundish number close to MinLSB)
	// CurrentLSB = 0.0000400 (40uA per bit)

	// 5. Compute the calibration register
	// Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
	// Cal = 10240 (0x2800)
	calib_val = 10240;

	// 6. Calculate the power LSB
	// PowerLSB = 20 * CurrentLSB
	// PowerLSB = 0.0008 (800uW per bit)

	// 7. Compute the maximum current and shunt voltage values before overflow
	//
	// Max_Current = Current_LSB * 32767
	// Max_Current = 1.31068A before overflow
	//
	// If Max_Current > Max_Possible_I then
	//    Max_Current_Before_Overflow = MaxPossible_I = 0.8A
	// Else
	//    Max_Current_Before_Overflow = Max_Current
	// End If
	//
	// ... In this case, we're good though since Max_Current is less than
	// MaxPossible_I
	//
	// Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
	// Max_ShuntVoltage = 0.08V
	//
	// If Max_ShuntVoltage >= VSHUNT_MAX
	//    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX = 0.08
	// Else
	//    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
	// End If

	// 8. Compute the Maximum Power
	// MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
	// MaximumPower = 0.8 * 16V
	// MaximumPower = 12.8W

	// Set multipliers to convert raw current/power values
	current_divider_ma = 25;	 // Current LSB = 40uA per bit (1000/40 = 25)
	power_multiplier_mw = 0.8f; // Power LSB = 800uW per bit

	// Set Calibration register to 'Cal' calculated above
	bsp_ina219_calib_set(calib_val);

	config.fields.brng = INA219_BRNG_16V;
	config.fields.pg = INA219_PG_2_80MV;
	config.fields.badc = INA219_ADCRES_12BIT_532US;
	config.fields.sadc = INA219_ADCRES_12BIT_532US;
	config.fields.mode = INA219_MODE_S_B_VOLT_CONTINUOUS;
	bsp_ina219_config_set(config);
}

/*
 *  @brief  Configures to INA219 to be able to measure up to 16V and 1A
 *          of current.  Each unit of current corresponds to 40uA, and each
 *          unit of power corresponds to 800uW. Counter overflow occurs at
 *          1.3A.
 *  @note   These calculations assume a 0.1 ohm resistor is present
 */
void _ina219_set_calib_16v_1a(void)
{
	ina219_config_t config;
	// By default we use a pretty huge range for the input voltage,
	// which probably isn't the most appropriate choice for system
	// that don't use a lot of power.  But all of the calculations
	// are shown below if you want to change the settings.  You will
	// also need to change any relevant register settings, such as
	// setting the VBUS_MAX to 16V instead of 32V, etc.

	// VBUS_MAX = 16V		(Assumes 32V, can also be set to 16V)
	// VSHUNT_MAX = 0.32	(Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
	// RSHUNT = 0.1			(Resistor value in ohms)

	// 1. Determine max possible current
	// MaxPossible_I = VSHUNT_MAX / RSHUNT
	// MaxPossible_I = 3.2A

	// 2. Determine max expected current
	// MaxExpected_I = 1.0A

	// 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
	// MinimumLSB = MaxExpected_I/32767
	// MinimumLSB = 0.0000305             (30.5uA per bit)
	// MaximumLSB = MaxExpected_I/4096
	// MaximumLSB = 0.000244              (244uA per bit)

	// 4. Choose an LSB between the min and max values
	//    (Preferrably a roundish number close to MinLSB)
	// CurrentLSB = 0.0000400 (40uA per bit)

	// 5. Compute the calibration register
	// Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
	// Cal = 10240 (0x2800)
	calib_val = 10240;

	// 6. Calculate the power LSB
	// PowerLSB = 20 * CurrentLSB
	// PowerLSB = 0.0008 (800uW per bit)

	// 7. Compute the maximum current and shunt voltage values before overflow
	//
	// Max_Current = Current_LSB * 32767
	// Max_Current = 1.31068A before overflow
	//
	// If Max_Current > Max_Possible_I then
	//    Max_Current_Before_Overflow = MaxPossible_I
	// Else
	//    Max_Current_Before_Overflow = Max_Current
	// End If
	//
	// ... In this case, we're good though since Max_Current is less than
	// MaxPossible_I
	//
	// Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
	// Max_ShuntVoltage = 0.131068V
	//
	// If Max_ShuntVoltage >= VSHUNT_MAX
	//    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
	// Else
	//    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
	// End If

	// 8. Compute the Maximum Power
	// MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
	// MaximumPower = 1.31068 * 32V
	// MaximumPower = 41.94176W

	// Set multipliers to convert raw current/power values
	current_divider_ma = 25;	 // Current LSB = 40uA per bit (1000/40 = 25)
	power_multiplier_mw = 0.8f; // Power LSB = 800uW per bit

	// Set Calibration register to 'Cal' calculated above
	bsp_ina219_calib_set(calib_val);

	config.fields.brng = INA219_BRNG_16V;
	config.fields.pg = INA219_PG_8_320MV;
	config.fields.badc = INA219_ADCRES_12BIT_532US;
	config.fields.sadc = INA219_ADCRES_12BIT_532US;
	config.fields.mode = INA219_MODE_S_B_VOLT_CONTINUOUS;
	bsp_ina219_config_set(config);
}

void bsp_ina219_init(void)
{
	current_divider_ma = 0;
	power_multiplier_mw = 0.0f;

	_ina219_set_calib_16v_800ma();
}

