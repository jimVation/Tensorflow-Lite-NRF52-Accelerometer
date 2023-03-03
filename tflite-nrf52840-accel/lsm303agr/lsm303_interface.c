
#include <stdint.h>
#include <stdlib.h>

#include "lsm303_interface.h"
#include "lsm303agr_driver.h"

#include "nrf_log.h"
#include "nrf_gpio.h"
#include "nrf_drv_twi.h"

#define NUM_AXIS 3
// Address is in 7 bit form, rather than 8 bit form. Shift left 1 to match accelerometer address given in data sheet.
#define LSM303AGR_X_NUCLEO_IKS01A2_ADDRESS     (0x19)

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

//********************************************************************************************
void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_lm303_config = {
       .scl                = NRF_GPIO_PIN_MAP(0, 27),
       .sda                = NRF_GPIO_PIN_MAP(0, 26),
       .frequency          = NRF_DRV_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_lm303_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

//*********************************************************************************
// Setup I2C config and accelerometer convert value
bool init_lsm303(void)
{
    uint8_t device_id;

    // read chip ID
    if (platform_read(LSM303AGR_WHO_AM_I_A, &device_id) < 0)
    {
        NRF_LOG_INFO("Failed getting device id");
        return false;
    }

    // Check chip ID
    if (device_id != LSM303AGR_ID_XL) 
    {
        NRF_LOG_INFO("Wrong device id. Expected 0x%x. Received 0x%x", LSM303AGR_ID_XL, device_id);
        return false;
    }
    else 
    {
        NRF_LOG_INFO("Device ID correct, 0x%x", LSM303AGR_ID_XL);
    }    

    // set data rate for accelermeter
	if (lsm303agr_xl_data_rate_set(LSM303AGR_XL_ODR_25Hz) < 0) 
    {
        NRF_LOG_INFO("Accel data rate set failed");
		return false;
	}

    // set scale for accelerometer
    if (lsm303agr_xl_full_scale_set(LSM303AGR_2g) < 0) 
    {
        NRF_LOG_INFO("Accel scale set failed");
		return false;
	}

    // set block update for accelerometer
    if (lsm303agr_xl_block_data_update_set(PROPERTY_ENABLE) < 0) 
    {
        NRF_LOG_INFO("Accel block update set failed");
		return false;
    }

    // set operating mode for accelerometer
    if (lsm303agr_xl_operating_mode_set(LSM303AGR_HR_12bit) < 0) 
    {
        NRF_LOG_INFO("Accel operating mode set failed");
		return false;        
    }

    NRF_LOG_INFO("Sensor LSM303AGR init OK");

    return true;
}

//************************************************************************************
// Get data from sensor, convert and call callback to handle
// Return true on new data obtained
bool read_data_lsm303(float* accel_data)
{
    uint8_t ready_flag = 0;
    int16_t data_raw_acceleration[NUM_AXIS];

    lsm303agr_xl_data_ready_get(&ready_flag);

    if(ready_flag)
    {
        // Read acceleration data
        memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));  // Zero each time in case the read fails
        lsm303agr_acceleration_raw_get(data_raw_acceleration);
        accel_data[0] = lsm303agr_from_fs_2g_hr_to_mg(data_raw_acceleration[0]);
        accel_data[1] = lsm303agr_from_fs_2g_hr_to_mg(data_raw_acceleration[1]);
        accel_data[2] = lsm303agr_from_fs_2g_hr_to_mg(data_raw_acceleration[2]);

        //NRF_LOG_INFO("X = " NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(accel_data[0]));
        //NRF_LOG_INFO("Y = " NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(accel_data[1]));
        //NRF_LOG_INFO("Z = " NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(accel_data[2]));
        //NRF_LOG_INFO(""); // Blank line

        return true;
    }

    return false;
}

//**********************************************************************************************
// Write generic device register (platform dependent)
//
// Params:
// reg       register to write
// bufp      pointer to data to write in register reg
// length    number of consecutive register to write
uint32_t platform_write(uint8_t reg, uint8_t *bufp)
{
    uint8_t temp_buf[2] = {0};
    temp_buf[0] = reg;
    memcpy(&temp_buf[1], bufp, 1);
    return nrf_drv_twi_tx(&m_twi, LSM303AGR_X_NUCLEO_IKS01A2_ADDRESS, temp_buf, 2, false);
}

//**********************************************************************************************
// Read one byte from target device register (platform dependent)
// 
// Params:
// read_register_address       register to read
// bufp      pointer to buffer to store incoming data
uint32_t platform_read(uint8_t read_register_address, uint8_t *bufp)
{
    // write address of register to read
    nrf_drv_twi_tx(&m_twi, LSM303AGR_X_NUCLEO_IKS01A2_ADDRESS, &read_register_address, 1, false);
    // read back register contents
    ret_code_t err_code = nrf_drv_twi_rx(&m_twi, LSM303AGR_X_NUCLEO_IKS01A2_ADDRESS, bufp, 1);
    APP_ERROR_CHECK(err_code);
    return err_code;
}


