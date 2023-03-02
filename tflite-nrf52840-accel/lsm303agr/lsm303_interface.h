#ifndef LSM303_INTERFACE_H
#define LSM303_INTERFACE_H

#include <stdbool.h>
#include <stdint.h>

void twi_init (void);
bool init_lsm303(void);
bool read_data_lsm303(float* accel_data);

uint32_t platform_write(uint8_t reg, uint8_t *bufp);
uint32_t platform_read(uint8_t read_register_address, uint8_t *bufp);

#endif
