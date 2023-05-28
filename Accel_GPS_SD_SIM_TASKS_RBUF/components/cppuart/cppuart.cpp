#include "cppuart.h"

namespace CPPUART
{
    Uart::Uart(uart_port_t port){

     _port = port;
    }

    Uart::~Uart()
    {
     uart_driver_delete(_port);
    }

    esp_err_t Uart::uart_init(ub7_baud_rate_t baud, gpio_num_t tx_pin, gpio_num_t rx_pin, gpio_num_t en_pin){

        gpio_set_direction(en_pin, GPIO_MODE_OUTPUT);
	    gpio_set_level(en_pin, true);

        uint32_t baud_rate;
        switch(baud){
	    case UB7_BAUD_4800:
		    baud_rate = 4800;
		    break;
	    case UB7_BAUD_19200:
		    baud_rate = 19200;
		    break;
	    case UB7_BAUD_38400:
		    baud_rate = 38400;
		    break;
	    case UB7_BAUD_57600:
		    baud_rate = 57600;
		    break;
	    case UB7_BAUD_115200:
		    baud_rate = 115200;
		    break;
	    case UB7_BAUD_9600:
	    default:
		    baud_rate = 9600;
		    break;

        }

        esp_err_t status = ESP_OK;

        uart_config_t _uart_conf{};

        _uart_conf.baud_rate = baud_rate;
        _uart_conf.data_bits = UART_DATA_8_BITS;
        _uart_conf.parity    = UART_PARITY_DISABLE;
        _uart_conf.stop_bits = UART_STOP_BITS_1;
        _uart_conf.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
	    _uart_conf.source_clk = UART_SCLK_DEFAULT; // CHANGED

        status |= uart_param_config(_port, &_uart_conf);
        uart_set_pin(_port, tx_pin, rx_pin, 
					UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        status |= uart_driver_install(_port, 2048, 0, 0, NULL, 0);

        return status;

    }

}