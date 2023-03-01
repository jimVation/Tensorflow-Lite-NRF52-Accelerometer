#ifndef LSM303_INTERFACE_H
#define LSM303_INTERFACE_H

#include <stdbool.h>
#include <stdint.h>

void twi_init (void);
bool init_lsm303(void);
uint8_t read_data_lsm303(void);
bool sample_start_lsm303(uint32_t sample_interval_ms);
bool setup_data_sampling_lsm303(void);

#endif
