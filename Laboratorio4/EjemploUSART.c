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
#include "string.h"

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

#define RIGHT QT1081_TOUCH_SENSOR_RIGHT

// Gameplay
struct gamer
{
	char name[50];
	int turn;
	int modality;
	int finish;
};

struct gamer player_one;

void usart_read_line(char *lineRead,size_t len);

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
	
	memset(player_one.name, 0, 50); //Esta funcion limpia el arreglo de caracteres. Primero se le pasa el array, con que se llena y la longitud del array.
	strncpy(player_one.name, "Yarib", 50);

	
	usart_write_line(EXAMPLE_USART,"\n");
	usart_write_line(EXAMPLE_USART,player_one.name);
	usart_write_line(EXAMPLE_USART," please select the number of the option that describes how you would like to accomodate your ships:\n1.- Manual\n2.- Automatic\n");
	
	usart_write_line(EXAMPLE_USART,"Ingresa un nombre: \n");
	
	usart_read_line(player_one.name,50);
	
	usart_write_line(EXAMPLE_USART,"El nombre que ingresaste es: ");
	usart_write_line(EXAMPLE_USART,player_one.name);
	
	while(1);

    player_one.modality=atoi(usart_read_line(EXAMPLE_USART)); //Atoi converts char array to int number
	
}

void usart_read_line(char *lineRead, size_t len){
	char line[len];
	int  i = 0;
	memset(lineRead, 0, len); 
	
	while(1) {
		line[i] = usart_getchar(EXAMPLE_USART); //Si se llega a utilizar el buffer "line" hay que limpiarlo antes o cualquier otro buffer
		if (line[i] == '\n') break;
		i++;
	}
	line[i] = 0;
	
	strncpy(lineRead, line, len);
}



char* convertIntegerToChar(int N)
{
 
    // Count digits in number N
    int m = N;
    int digit = 0;
    while (m) {
 
        // Increment number of digits
        digit++;
        m /= 10;
    }
 
    // Declare char array for result
    char* arr;
    // Declare duplicate char array
    char arr1[digit];
    // Memory allocation of array
    arr = (char*)malloc(digit);
 
    // Separating integer into digits and
    // accommodate it to character array
    int index = 0;
    while (N) {
 
        // Separate last digit from
        // the number and add ASCII
        // value of character '0' is 48
        arr1[++index] = N % 10 + '0';
 
        // Truncate the last
        // digit from the number
        N /= 10;
    }
 
    // Reverse the array for result
    int i;
    for (i = 0; i < index; i++) {
        arr[i] = arr1[index - i];
    }
 
    // Char array truncate by null
    arr[i] = '\0';
 
    // Return char array
    return (char*)arr;
}
