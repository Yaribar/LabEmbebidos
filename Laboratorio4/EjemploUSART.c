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
#include "board.h"
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "et024006dhu.h"
#include "conf_clock.h"
#include "gpio.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
#include "pwm.h"



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

unsigned int photo[400];
// Gameplay
struct gamer
{
	char name[50];
	int turn;
	int modality;
	int finish;
};

struct gamer player_one;

avr32_pwm_channel_t pwm_channel6 = {
/*
  .cmr = ((PWM_MODE_LEFT_ALIGNED << AVR32_PWM_CMR_CALG_OFFSET)
    | (PWM_POLARITY_HIGH << AVR32_PWM_CMR_CPOL_OFFSET)
    | (PWM_UPDATE_DUTY << AVR32_PWM_CMR_CPD_OFFSET)
    | AVR32_PWM_CMR_CPRE_MCK_DIV_2),
    */
  .cdty = 100,
  //.cdty = 0,
  .cprd = 100
};

static void tft_bl_init(void)
{

  pwm_opt_t opt = {
    .diva = 0,
    .divb = 0,
    .prea = 0,
    .preb = 0
  };
  /* MCK = OSC0 = 12MHz
   * Desired output 60kHz
   * Chosen MCK_DIV_2
   * CPRD = 12MHz / (60kHz * 2) = 100
   *
   * The duty cycle is 100% (CPRD = CDTY)
   * */
  pwm_init(&opt);
  pwm_channel6.CMR.calg = PWM_MODE_LEFT_ALIGNED;
  pwm_channel6.CMR.cpol = PWM_POLARITY_HIGH; //PWM_POLARITY_LOW;//PWM_POLARITY_HIGH;
  pwm_channel6.CMR.cpd = PWM_UPDATE_DUTY;
  pwm_channel6.CMR.cpre = AVR32_PWM_CMR_CPRE_MCK_DIV_2;

  pwm_channel_init(6, &pwm_channel6);
  pwm_start_channels(AVR32_PWM_ENA_CHID6_MASK);

}

void usart_read_line(char *lineRead,size_t len);
char* convertIntegerToChar(int N);
void readImage(char *string_in, char *delimiter, unsigned int output_array[]);

int main(void)
{
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
	
	et024006_Init( FOSC0, FOSC0 );
	tft_bl_init();

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


	usart_write_line(EXAMPLE_USART,"Ingresa imagen: \n");
	char img[3000];
	
	usart_read_line(img,1050);
	readImage(img,",",photo);
	
	et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, WHITE );
	usart_write_line(EXAMPLE_USART,"Aqui la foto: ");
	et024006_PutPixmap(photo, 20, 0, 0, 0, 0, 20, 20);
	
	while(1);

	
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

void readImage(char *string_in, char *delimiter,unsigned int output_array[])
{
	char* pch; //create buffer
	pch = strtok(string_in, delimiter); //begin parsing the values
	uint8_t i = 0;
	while (pch != NULL)
	{
		output_array[i] = strtol(pch, NULL, 0);
		pch = strtok(NULL, delimiter);
		i++;
	}
}