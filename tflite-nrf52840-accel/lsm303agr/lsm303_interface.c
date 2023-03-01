
#include <stdint.h>
#include <stdlib.h>

#include "lsm303_interface.h"
#include "lsm303agr_reg.h"

#include "nrf_log.h"
#include "nrf_gpio.h"
#include "nrf_drv_twi.h"

// Constant defines
#define CONVERT_G_TO_MS2    9.80665f

#define ACC_SAMPLE_TIME_MS  0.912f
#define FLASH_WRITE_TIME_MS 0.5036f

#define ACC_SAMPLE_TIME_US  255
#define FLASH_WRITE_TIME_US 543
#define CORRECTION_TIME_US  400

#define N_AXIS_SAMPLED 3
// Not sure why this address is needed for the ST board, rather than actual accelerometer address
#define LSM303AGR_X_NUCLEO_IKS01A2_ADDRESS     (0x19)

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

// Private function prototypes
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t length);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t length);

stmdev_ctx_t dev_ctx;

static float acceleration_g[N_AXIS_SAMPLED];
int16_t data_raw_acceleration[N_AXIS_SAMPLED];

int32_t sample_interval_real_us = 0;
static bool device_init_correctly = false;

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

    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.handle = NULL;

    // read chip ID
    if (lsm303agr_xl_device_id_get(&dev_ctx, &device_id) < 0) 
    {
        NRF_LOG_INFO("Failed getting device id, \n");
        return false;
    }

    // Check chip ID
    if (device_id != LSM303AGR_ID_XL) 
    {
        NRF_LOG_INFO("Wrong device id. Expected %u. Received %u\n", LSM303AGR_ID_XL, device_id);
        return false;
    }
    else 
    {
        NRF_LOG_INFO("Device ID correct\n");
    }    

    // set data rate for accelermeter
	if (lsm303agr_xl_data_rate_set(&dev_ctx, LSM303AGR_XL_ODR_100Hz) < 0) 
    {
        NRF_LOG_INFO("Accel data rate set failed\n");
		return false;
	}

    // set scale for accelerometer
    if (lsm303agr_xl_full_scale_set(&dev_ctx, LSM303AGR_2g) < 0) 
    {
        NRF_LOG_INFO("Accel scale set failed\n");
		return false;
	}

    // set block update for accelerometer
    if (lsm303agr_xl_block_data_update_set(&dev_ctx, PROPERTY_ENABLE) < 0) 
    {
        NRF_LOG_INFO("Accel block update set failed\n");
		return false;
    }

    // set operating mode for accelerometer
    if (lsm303agr_xl_operating_mode_set(&dev_ctx, LSM303AGR_HR_12bit) < 0) 
    {
        NRF_LOG_INFO("Accel operating mode set failed\n");
		return false;        
    }

    NRF_LOG_INFO("Sensor LSM303AGR init OK\n");
    device_init_correctly = true;

    return true;
}

//************************************************************************************
// Get data from sensor, convert and call callback to handle
// Return 0 on success, non-zero on error
uint8_t read_data_lsm303(void)
{
    uint8_t ready_flag;
    int ret_val = 0;

    lsm303agr_xl_data_ready_get(&dev_ctx, &ready_flag);

    if(ready_flag)
    {
        /* Read acceleration data */
        memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
        lsm303agr_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
        acceleration_g[0] = lsm303agr_from_fs_2g_hr_to_mg(data_raw_acceleration[0]) / 1000.f;
        acceleration_g[1] = lsm303agr_from_fs_2g_hr_to_mg(data_raw_acceleration[1]) / 1000.f;
        acceleration_g[2] = lsm303agr_from_fs_2g_hr_to_mg(data_raw_acceleration[2]) / 1000.f;

        NRF_LOG_INFO("X= %f, Y= %f, Z= %f\r\n", acceleration_g[0], acceleration_g[1], acceleration_g[2]);
    }
    else 
    {
        NRF_LOG_INFO("Data not ready\n");
    }

    return ret_val;
}

//****************************************************************************************
// Setup timing and data handle callback function
// return true on success, false on failure
bool sample_start_lsm303(uint32_t sample_interval_ms)
{
    if(device_init_correctly == false) 
    {
        NRF_LOG_INFO("\r\nERR: Failed to get data, is your accelerometer connected?\r\n");
        return false;
    }

    sample_interval_real_us = (int32_t)(sample_interval_ms * 1000);
    sample_interval_real_us = sample_interval_real_us - (FLASH_WRITE_TIME_US + ACC_SAMPLE_TIME_US + CORRECTION_TIME_US);

    NRF_LOG_INFO("sample_interval_real_us = %d us\n", sample_interval_real_us);

    return true;
}

//*************************************************************************************
// Setup payload header
bool setup_data_sampling_lsm303(void)
{
    return true;
}

//**********************************************************************************************
// Write generic device register (platform dependent)
//
// Params:
// handle    customizable argument. In this examples is used in order to select the correct sensor bus handler.
// reg       register to write
// bufp      pointer to data to write in register reg
// length    number of consecutive register to write
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t length)
{
    uint8_t temp_buf[10] = {0};
    temp_buf[0] = reg;
    memcpy(&temp_buf[1], bufp, length);
    return nrf_drv_twi_tx(&m_twi, LSM303AGR_X_NUCLEO_IKS01A2_ADDRESS, temp_buf, (uint8_t)(length+1), false);
}

//**********************************************************************************************
// Read generic device register (platform dependent)
// 
// Params:
// handle    customizable argument. In this examples is used in order to select the correct sensor bus handler.
// reg       register to read
// bufp      pointer to buffer that store the data read
// length    number of consecutive register to read
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t length)
{
    ret_code_t err_code = nrf_drv_twi_rx(&m_twi, LSM303AGR_X_NUCLEO_IKS01A2_ADDRESS, &reg, (uint8_t)length);
    APP_ERROR_CHECK(err_code);
    return err_code;
}

