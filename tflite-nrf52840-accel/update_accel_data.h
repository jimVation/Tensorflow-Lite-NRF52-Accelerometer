#ifndef UPDATE_ACCEL_DATA_H
#define UPDATE_ACCEL_DATA_H

#include <stdbool.h>

bool update_accel_data(float* data_to_analyze, float* new_data, int num_samples_requested, bool reset_buffer);

#endif