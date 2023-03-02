
#include <stdbool.h>
#include <stdint.h>

#include "boards.h"
#include "app_timer.h"
#include "nordic_common.h"
#include "nrf_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_delay.h"

#include "TFLite_Top.h"
#include "lsm303_interface.h"

#define NUM_ACCEL_DATA_ELEMENTS    (128 * 3)

//********************************************************************************************
/**@brief Function for initializing low frequency clock.
 */
void clock_initialization()
{
    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
        // Do nothing.
    }
}

//********************************************************************************************
int main(void)
{
    float accel_data[NUM_ACCEL_DATA_ELEMENTS] = {0};
    uint32_t next_data_index = 0;

    clock_initialization();

    uint32_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("Tensorflow Lite accelerometer example started");

    twi_init();
    init_lsm303();
    NRF_LOG_INFO("Accelerometer setup complete");

    setup_tf_system();
    NRF_LOG_INFO("Tensorflow setup complete");


    while (true)
    {
        if (read_data_lsm303(&accel_data[next_data_index]))
        {    
        // call inference every time there is new data
            next_data_index += 3;

            if ((next_data_index + 1) >= NUM_ACCEL_DATA_ELEMENTS)
            {
                next_data_index = 0;
            }
        }

        NRF_LOG_FLUSH();

        //__SEV();
        //__WFE();
        //__WFE();
    }
}


/** @} */
