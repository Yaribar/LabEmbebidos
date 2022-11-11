// *********************** Libraries **********************
#include <avr32/io.h>
#include "pm.h"     // Todos
#include "flashc.h" // Todos
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "et024006dhu.h"
#include "conf_clock.h"
#include "gpio.h"
#include "usart.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pwm.h"

// ***********************   Macros  **********************

#define EXAMPLE_TARGET_PBACLK_FREQ_HZ FOSC0 // PBA clock target frequency, in Hz

// USART1

#define EXAMPLE_USART (&AVR32_USART0)
#define EXAMPLE_USART_RX_PIN AVR32_USART0_RXD_0_0_PIN
#define EXAMPLE_USART_RX_FUNCTION AVR32_USART0_RXD_0_0_FUNCTION
#define EXAMPLE_USART_TX_PIN AVR32_USART0_TXD_0_0_PIN
#define EXAMPLE_USART_TX_FUNCTION AVR32_USART0_TXD_0_0_FUNCTION
#define EXAMPLE_USART_CLOCK_MASK AVR32_USART0_CLK_PBA
#define EXAMPLE_PDCA_CLOCK_HSB AVR32_PDCA_CLK_HSB
#define EXAMPLE_PDCA_CLOCK_PB AVR32_PDCA_CLK_PBA

// GPIO
#define UP 54
#define DOWN 55
#define RIGHT 56
#define LEFT 57
#define ENTER 58

// INTC
#define IRQ70 70 // 54 (UP) y 55 (DOWN)
#define IRQ71 71 // 56 (RIGHT), 57 (LEFT) y 58 (ENTER)
#define A 0
#define B 1
#define C 2

// ***********************  Variables  ********************

uint8_t fu = 0;
uint8_t fd = 0;
uint8_t fe = 0;
uint8_t fr = 0;
uint8_t fl = 0;

// ******************** Prototype Functions ****************

void usart_read_line(char *lineRead, size_t len);

// ******************** Display Functions ****************

avr32_pwm_channel_t pwm_channel6 = {
    /*
      .cmr = ((PWM_MODE_LEFT_ALIGNED << AVR32_PWM_CMR_CALG_OFFSET)
        | (PWM_POLARITY_HIGH << AVR32_PWM_CMR_CPOL_OFFSET)
        | (PWM_UPDATE_DUTY << AVR32_PWM_CMR_CPD_OFFSET)
        | AVR32_PWM_CMR_CPRE_MCK_DIV_2),
        */
    .cdty = 100,
    //.cdty = 0,
    .cprd = 100};

static void tft_bl_init(void)
{

    pwm_opt_t opt = {
        .diva = 0,
        .divb = 0,
        .prea = 0,
        .preb = 0};
    /* MCK = OSC0 = 12MHz
     * Desired output 60kHz
     * Chosen MCK_DIV_2
     * CPRD = 12MHz / (60kHz * 2) = 100
     *
     * The duty cycle is 100% (CPRD = CDTY)
     * */
    pwm_init(&opt);
    pwm_channel6.CMR.calg = PWM_MODE_LEFT_ALIGNED;
    pwm_channel6.CMR.cpol = PWM_POLARITY_HIGH; // PWM_POLARITY_LOW;//PWM_POLARITY_HIGH;
    pwm_channel6.CMR.cpd = PWM_UPDATE_DUTY;
    pwm_channel6.CMR.cpre = AVR32_PWM_CMR_CPRE_MCK_DIV_2;

    pwm_channel_init(6, &pwm_channel6);
    pwm_start_channels(AVR32_PWM_ENA_CHID6_MASK);
}




__attribute__((__interrupt__)) void isr(void)
{

    if (gpio_get_pin_interrupt_flag(UP))
    {
        fu = 1;
        gpio_clear_pin_interrupt_flag(UP);
    }
    if (gpio_get_pin_interrupt_flag(DOWN))
    {
        fd = 1;
        gpio_clear_pin_interrupt_flag(DOWN);
    }
    if (gpio_get_pin_interrupt_flag(RIGHT))
    {
        fr = 1;
        gpio_clear_pin_interrupt_flag(RIGHT);
    }
    if (gpio_get_pin_interrupt_flag(LEFT))
    {
        fl = 1;
        gpio_clear_pin_interrupt_flag(LEFT);
    }
    if (gpio_get_pin_interrupt_flag(ENTER))
    {
        fe = 1;
        gpio_clear_pin_interrupt_flag(ENTER);
    }
}

int main()
{

    pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

    et024006_Init(FOSC0, FOSC0);
    tft_bl_init();

    static const gpio_map_t USART_GPIO_MAP =
        {
            {EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
            {EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}};

    // USART options.
    static const usart_options_t USART_OPTIONS =
        {
            .baudrate = 38400,
            .charlength = 8,
            .paritytype = USART_EVEN_PARITY,
            .stopbits = USART_2_STOPBITS,
            .channelmode = USART_NORMAL_CHMODE};

    // Assign GPIO to USART.
    gpio_enable_module(USART_GPIO_MAP, sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

    // Initialize USART in RS232 mode.
    usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, EXAMPLE_TARGET_PBACLK_FREQ_HZ);

    et024006_DrawFilledRect(0, 0, ET024006_WIDTH, ET024006_HEIGHT, WHITE);

    while (1)
    {

        if (fl)
        {
            usart_write_line(EXAMPLE_USART,"Ingrese codigo de seguridad: \n");
            
        }
        if (fe)
        {
            
        }
        
    }

    return 0;
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