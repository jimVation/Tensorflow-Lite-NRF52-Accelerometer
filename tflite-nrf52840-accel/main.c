
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
        NRF_LOG_FLUSH();
        //__SEV();
        //__WFE();
        //__WFE();

        nrf_delay_ms(1000);

        read_data_lsm303();
    }
}


/** @} */
