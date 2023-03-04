
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
void gesture_detection_output(int32_t gesture)
{
    switch (gesture)
    {
        case 0:  // Wing
            NRF_LOG_INFO("WING:\n\r*         *         *\n\r *       * *       "
        "*\n\r  *     *   *     *\n\r   *   *     *   *\n\r    * *       "
        "* *\n\r     *         *\n\r");
            break;

        case 1:  // Ring
            NRF_LOG_INFO("RING:\n\r          *\n\r       *     *\n\r     *         *\n\r "
        "   *           *\n\r     *         *\n\r       *     *\n\r      "
        "    *\n\r");
            break;

        case 2:  // Slope
            NRF_LOG_INFO("SLOPE:\n\r        *\n\r       *\n\r      *\n\r     *\n\r    "
        "*\n\r   *\n\r  *\n\r * * * * * * * *\n\r");
            break;

        default:
            NRF_LOG_INFO("NONE");
            break;
    }
}

//********************************************************************************************
int main(void)
{
    float accel_data[NUM_ACCEL_DATA_ELEMENTS] = {0};
    int32_t gesture_type = -1;

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
        if (read_data_lsm303(accel_data))
        {   
            gesture_type = run_tf_model(accel_data); // pass new data to analyzer system
            gesture_detection_output(gesture_type);
        }

        NRF_LOG_FLUSH();

        //__SEV();
        //__WFE();
        //__WFE();
    }
}


/** @} */
