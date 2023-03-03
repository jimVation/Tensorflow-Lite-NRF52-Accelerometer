#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lsm303_interface.h"

// A buffer holding the last 200 sets of 3-channel values
float save_data[600] = {0.0f};

// Most recent position in the save_data buffer
int next_data_index = 0;

// True if there is not yet enough data to run inference
bool pending_initial_data = true;

bool update_accel_data(float* data_to_analyze, float* new_data, int num_samples_requested, bool reset_buffer)
{
    // Clear the buffer if required, e.g. after a successful prediction
    if (reset_buffer) 
    {
        memset(save_data, 0, 600 * sizeof(float));
        next_data_index = 0;
        pending_initial_data = true;
    }

    // Move new data into local array
    save_data[next_data_index++] = new_data[0];
    save_data[next_data_index++] = new_data[1];
    save_data[next_data_index++] = new_data[2];

    // check for end of array
    if (next_data_index >= 600)
    {
        next_data_index = 0; // loop around to beginning of array
    }


    // Check if we are ready for prediction or still waiting more initial data on start up
    if (pending_initial_data)
    {
        if (next_data_index >= 200)
        {   // there is enough collected data to continue
            pending_initial_data = false;
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
            ring_array_index += 600;
        }
        data_to_analyze[i] = save_data[ring_array_index];
    }
    return true;
}