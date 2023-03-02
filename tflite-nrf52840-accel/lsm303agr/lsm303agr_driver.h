#ifndef LSM303_DRIVER_H
#define LSM303_DRIVER_H

typedef enum
{
  LSM303AGR_XL_POWER_DOWN                      = 0,
  LSM303AGR_XL_ODR_1Hz                         = 1,
  LSM303AGR_XL_ODR_10Hz                        = 2,
  LSM303AGR_XL_ODR_25Hz                        = 3,
  LSM303AGR_XL_ODR_50Hz                        = 4,
  LSM303AGR_XL_ODR_100Hz                       = 5,
  LSM303AGR_XL_ODR_200Hz                       = 6,
  LSM303AGR_XL_ODR_400Hz                       = 7,
  LSM303AGR_XL_ODR_1kHz620_LP                  = 8,
  LSM303AGR_XL_ODR_1kHz344_NM_HP_5kHz376_LP    = 9,
} lsm303agr_odr_a_t;

typedef enum
{
  LSM303AGR_2g   = 0,
  LSM303AGR_4g   = 1,
  LSM303AGR_8g   = 2,
  LSM303AGR_16g  = 3,
} lsm303agr_fs_a_t;

typedef enum
{
  LSM303AGR_HR_12bit   = 0,
  LSM303AGR_NM_10bit   = 1,
  LSM303AGR_LP_8bit    = 2,
} lsm303agr_op_md_a_t;

#define PROPERTY_DISABLE                (0U)
#define PROPERTY_ENABLE                 (1U)

// Register Definitions
#define LSM303AGR_CTRL_REG1_A                0x20U
typedef struct
{
  uint8_t xen                    : 1;
  uint8_t yen                    : 1;
  uint8_t zen                    : 1;
  uint8_t lpen                   : 1;
  uint8_t odr                    : 4;
} lsm303agr_ctrl_reg1_a_t;

#define LSM303AGR_CTRL_REG4_A                0x23U
typedef struct
{
  uint8_t spi_enable             : 1;
  uint8_t st                     : 2;
  uint8_t hr                     : 1;
  uint8_t fs                     : 2;
  uint8_t ble                    : 1;
  uint8_t bdu                    : 1;
} lsm303agr_ctrl_reg4_a_t;

#define LSM303AGR_STATUS_REG_A              0x27U
typedef struct
{
  uint8_t xda                    : 1;
  uint8_t yda                    : 1;
  uint8_t zda                    : 1;
  uint8_t zyxda                  : 1;
  uint8_t _xor                   : 1;
  uint8_t yor                    : 1;
  uint8_t zor                    : 1;
  uint8_t zyxor                  : 1;
} lsm303agr_status_reg_a_t;

#define LSM303AGR_OUT_X_L_A                 0x28U
#define LSM303AGR_OUT_X_H_A                 0x29U
#define LSM303AGR_OUT_Y_L_A                 0x2AU
#define LSM303AGR_OUT_Y_H_A                 0x2BU
#define LSM303AGR_OUT_Z_L_A                 0x2CU
#define LSM303AGR_OUT_Z_H_A                 0x2DU
#define LSM303AGR_FIFO_CTRL_REG_A           0x2EU
#define LSM303AGR_WHO_AM_I_A                0x0FU


// Device Identification (Who am I)
#define LSM303AGR_ID_XL            0x33U

float lsm303agr_from_fs_2g_hr_to_mg(int16_t lsb);
uint32_t lsm303agr_xl_data_rate_set(lsm303agr_odr_a_t val);
uint32_t lsm303agr_xl_block_data_update_set(uint8_t val);
uint32_t lsm303agr_xl_full_scale_set(lsm303agr_fs_a_t val);
uint32_t lsm303agr_xl_operating_mode_set(lsm303agr_op_md_a_t val);
uint32_t lsm303agr_xl_data_ready_get(uint8_t *val);
uint32_t lsm303agr_acceleration_raw_get(int16_t *val);

#endif
