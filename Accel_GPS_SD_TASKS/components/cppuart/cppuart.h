#ifndef CPPUART_H
#define CPPUART_H

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_intr_alloc.h"

/**
 * @brief Baud rate enum.
 * 
 * @details
 *     - 9600 bps is the default
 *     - Currently no baud change supported
 */
enum ub7_baud_rate_t {
    UB7_BAUD_4800,
    UB7_BAUD_9600,
    UB7_BAUD_19200,
    UB7_BAUD_38400,
    UB7_BAUD_57600,
    UB7_BAUD_115200
};

namespace CPPUART
{

    class Uart
    {
    private:
        uart_port_t _port{};
        ub7_baud_rate_t _baud;


    public: 
        Uart(uart_port_t port);
        ~Uart();
        esp_err_t uart_init(ub7_baud_rate_t baud, gpio_num_t tx_pin, gpio_num_t rx_pin, gpio_num_t en_pin);
    };


}

#endif