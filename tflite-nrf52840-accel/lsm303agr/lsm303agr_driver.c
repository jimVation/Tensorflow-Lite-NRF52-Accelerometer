#include <stdint.h>

#include "lsm303agr_driver.h"
#include "lsm303_interface.h"  // for platform read/write

//************************************************************************************
float lsm303agr_from_fs_2g_hr_to_mg(int16_t lsb)
{
  return ((float)lsb / 16.0f) * 0.98f;
}

//*************************************************************************************
/**
  * @brief  Output data rate selection.[set]
  *
  * @param  val    Change the values of odr in reg CTRL_REG1_A
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
uint32_t lsm303agr_xl_data_rate_set(lsm303agr_odr_a_t val)
{
  lsm303agr_ctrl_reg1_a_t ctrl_reg1_a;
  uint32_t ret;

  ret = platform_read(LSM303AGR_CTRL_REG1_A, (uint8_t *)&ctrl_reg1_a);

  if (ret == 0)
  {
    ctrl_reg1_a.odr = (uint8_t)val;
    ctrl_reg1_a.xen = 1;
    ctrl_reg1_a.yen = 1;
    ctrl_reg1_a.zen = 1;
    ret = platform_write(LSM303AGR_CTRL_REG1_A, (uint8_t *)&ctrl_reg1_a);
  }

  return ret;
}

//*************************************************************************************
/**
  * @brief  Block data update.[set]
  *
  * @param  val    Change the values of bdu in reg CTRL_REG4_A
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
uint32_t lsm303agr_xl_block_data_update_set(uint8_t val)
{
  lsm303agr_ctrl_reg4_a_t ctrl_reg4_a;
  uint32_t ret;

  ret = platform_read(LSM303AGR_CTRL_REG4_A, (uint8_t *)&ctrl_reg4_a);

  if (ret == 0)
  {
    ctrl_reg4_a.bdu = (uint8_t)val;
    ret = platform_write(LSM303AGR_CTRL_REG4_A, (uint8_t *)&ctrl_reg4_a);
  }

  return ret;
}

//*************************************************************************************
/**
  * @brief  Full-scale configuration.[set]
  *
  * @param  val    Change the values of fs in reg CTRL_REG4_A
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
uint32_t lsm303agr_xl_full_scale_set(lsm303agr_fs_a_t val)
{
  lsm303agr_ctrl_reg4_a_t ctrl_reg4_a;
  uint32_t ret;

  ret = platform_read(LSM303AGR_CTRL_REG4_A, (uint8_t *)&ctrl_reg4_a);

  if (ret == 0)
  {
    ctrl_reg4_a.fs = (uint8_t)val;  // Apply val parameter
    ret = platform_write(LSM303AGR_CTRL_REG4_A, (uint8_t *)&ctrl_reg4_a);
  }

  return ret;
}

//*************************************************************************************
/**
  * @brief  Operating mode selection.[set]
  *
  * @param  val    Change the values of lpen in reg
  *                CTRL_REG1_A and HR in reg CTRL_REG4_A
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
uint32_t lsm303agr_xl_operating_mode_set(lsm303agr_op_md_a_t val)
{
  lsm303agr_ctrl_reg1_a_t ctrl_reg1_a;
  lsm303agr_ctrl_reg4_a_t ctrl_reg4_a;
  uint32_t ret;

  uint8_t lpen, hr;

  if (val == LSM303AGR_HR_12bit)
  {
    lpen = 0;
    hr   = 1;
  }

  else if (val == LSM303AGR_NM_10bit)
  {
    lpen = 0;
    hr   = 0;
  }

  else
  {
    lpen = 1;
    hr   = 0;
  }

  ret = platform_read(LSM303AGR_CTRL_REG1_A, (uint8_t *)&ctrl_reg1_a);
  ctrl_reg1_a.lpen = (uint8_t)lpen;

  if (ret == 0)
  {
    ret = platform_write(LSM303AGR_CTRL_REG1_A, (uint8_t *)&ctrl_reg1_a);
  }

  if (ret == 0)
  {
    ret = platform_read(LSM303AGR_CTRL_REG4_A, (uint8_t *)&ctrl_reg4_a);
  }

  if (ret == 0)
  {
    ctrl_reg4_a.hr = hr;
    ret = platform_write(LSM303AGR_CTRL_REG4_A, (uint8_t *)&ctrl_reg4_a);
  }

  return ret;
}

//************************************************************************************
/**
  * @brief  Acceleration set of data available.[get]
  *
  * @param  val    Get the values of zyxda in reg STATUS_REG_A.(ptr)
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
uint32_t lsm303agr_xl_data_ready_get(uint8_t *val)
{
  lsm303agr_status_reg_a_t status_reg_a;
  uint32_t ret;

  ret = platform_read(LSM303AGR_STATUS_REG_A, (uint8_t *)&status_reg_a);
  *val = status_reg_a.zyxda;

  return ret;
}

//************************************************************************************
/**
  * @brief  Acceleration output value.[get]
  *
  * @param  val    Buffer that stores data read.(ptr)
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
uint32_t lsm303agr_acceleration_raw_get(int16_t *val)
{
  uint8_t read_value;
  uint32_t ret;

  platform_read(LSM303AGR_OUT_X_L_A, &read_value);
  val[0] = 0x00FF & ((int16_t)read_value);
  platform_read(LSM303AGR_OUT_X_H_A, &read_value);
  val[0] |= (int16_t)read_value << 8;
  platform_read(LSM303AGR_OUT_Y_L_A, &read_value);
  val[1] = 0x00FF & ((int16_t)read_value);
  platform_read(LSM303AGR_OUT_Y_H_A, &read_value);
  val[1] |= (int16_t)read_value << 8;
  platform_read(LSM303AGR_OUT_Z_L_A, &read_value);
  val[2] = 0x00FF & ((int16_t)read_value);
  ret = platform_read(LSM303AGR_OUT_Z_H_A, &read_value);
  val[2] |= (int16_t)read_value << 8;

  return ret;
}