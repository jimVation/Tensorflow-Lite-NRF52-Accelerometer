#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "nrf_log.h"

#include "lsm303_interface.h"
#include "slope_micro_features_data.h"
#include "ring_micro_features_data.h"

#define NUM_STORED_DATA_ELEMENTS  384

// A buffer holding the last 200 sets of 3-channel values
float save_data[NUM_STORED_DATA_ELEMENTS] = {0.0f};  // Initialize to an unlikely number

// Most recent position in the save_data buffer
int next_data_index = 0;

// True if there is not yet enough data to run inference
bool waiting_for_initial_data = true;

typedef enum 
{
    NORMAL_ANALYZE_MOODE = 0,
    TEST_SLOPE_DATA,
    TEST_RING_DATA,
}  data_mode_and_type_t;

data_mode_and_type_t  analyze_mode = NORMAL_ANALYZE_MOODE;


bool update_accel_data(float* data_to_analyze, float* new_data, int num_samples_requested, bool reset_buffer)
{
    // Clear the buffer if required, e.g. after a successful prediction
    if (reset_buffer) 
    {
        memset(save_data, 0, NUM_STORED_DATA_ELEMENTS * sizeof(float));
        next_data_index = 0;
        waiting_for_initial_data = true;
    }

    // Move new data into local array
    save_data[next_data_index++] = new_data[0];
    save_data[next_data_index++] = new_data[1];
    save_data[next_data_index++] = new_data[2];

    // check for end of array
    if (next_data_index >= NUM_STORED_DATA_ELEMENTS)
    {
        next_data_index = 0; // loop around to beginning of array
    }


    // Check if we are ready for prediction or still waiting more initial data on start up
    if (waiting_for_initial_data)
    {
        if (next_data_index >= 200)
        {   // there is enough collected data to continue
            waiting_for_initial_data = false;
        } 
        else
        {   // not enough data collected yet
            return false;
        }
    }

    // Copy the requested number of bytes to the provided input tensor
    for (int i = 0; i < num_samples_requested; ++i) 
    {
        int ring_array_index = next_data_index + i - num_samples_requested;

        if (ring_array_index < 0) 
        {
            ring_array_index += NUM_STORED_DATA_ELEMENTS;
        }

        switch (analyze_mode)
        {
            case NORMAL_ANALYZE_MOODE:
                data_to_analyze[i] = save_data[ring_array_index];
                break;

            case TEST_SLOPE_DATA:
                data_to_analyze[i] = g_slope_micro_f2e59fea_nohash_1_data[ring_array_index];
                break;

            case TEST_RING_DATA:
                data_to_analyze[i] = g_ring_micro_f9643d42_nohash_4_data[ring_array_index];
                break;
            
            default:
                NRF_LOG_INFO("Invalid analyze mode");
                break;
        }
    }
    return true;
}