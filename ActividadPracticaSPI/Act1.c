/*
2- Un dispositivo de comunicaciones inalámbricas atendido por el puerto USART 1 del microcontrolador. 
El sistema debe transmitir al dispositivo un bloque de 2048 caracteres almacenados en un arreglo llamado WIFI. 
La comunicación se iniciará cuando se oprima la tecla RIGHT del Touch y tendrá las siguientes especificaciones:

            -Baudrate: 38400
            -Longitud: 8 bits
            -Paridad Par
            -Bits de Parada: 2
*/

#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "usart.h"

#define EXAMPLE_TARGET_PBACLK_FREQ_HZ FOSC0  // PBA clock target frequency, in Hz

//USART1

#define EXAMPLE_USART                 (&AVR32_USART0)
#define EXAMPLE_USART_RX_PIN          AVR32_USART0_RXD_0_0_PIN
#define EXAMPLE_USART_RX_FUNCTION     AVR32_USART0_RXD_0_0_FUNCTION
#define EXAMPLE_USART_TX_PIN          AVR32_USART0_TXD_0_0_PIN
#define EXAMPLE_USART_TX_FUNCTION     AVR32_USART0_TXD_0_0_FUNCTION
#define EXAMPLE_USART_CLOCK_MASK      AVR32_USART0_CLK_PBA
#define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA

const char *WIFI[200]; //2048 caracteres
uint16_t wifi_size = sizeof(WIFI) / sizeof(WIFI[0]);



int main(void)
{
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

	static const gpio_map_t USART_GPIO_MAP =
	{
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	};

	// USART options.
	static const usart_options_t USART_OPTIONS =
	{
		.baudrate     = 38400,
		.charlength   = 8,
		.paritytype   = USART_EVEN_PARITY,
		.stopbits     = USART_2_STOPBITS,
		.channelmode  = USART_NORMAL_CHMODE
	};

	// Assign GPIO to USART.
	gpio_enable_module(USART_GPIO_MAP, sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	// Initialize USART in RS232 mode.
	usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, EXAMPLE_TARGET_PBACLK_FREQ_HZ);

	for (int i = 48; i<=126; i++)
	{
		WIFI[i]=(char)i;
	}
	//for(int i = 0;i< wifi_size;i++){
		usart_write_line(EXAMPLE_USART, "Hello\r\n");
	//}

	// Press enter to continue.
	while (usart_get_echo_line(EXAMPLE_USART) == USART_FAILURE);  // Get and echo characters until end of line.

	usart_write_line(EXAMPLE_USART, "Goodbye.\r\n");

	while (true);
}


