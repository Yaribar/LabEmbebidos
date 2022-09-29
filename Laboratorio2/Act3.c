/*****************************************************************************

 */

#include "board.h"
#include "gpio.h"
#include "pm.h"
#include "delay.h"
#include "intc.h"
#include "flashc.h"
#include "power_clocks_lib.h"
#include "et024006dhu.h"
#include "avr32_logo.h"
#include "conf_clock.h"

#define UP      54
#define DOWN    55
#define RIGHT   56
#define LEFT    57
#define ENTER   58

#define LED0    59
#define LED1    60
#define LED2    5
#define LED3    6

#define IRQ70   70
#define IRQ71   71

#define RISING  1

uint8_t widthSquare = 20;
uint8_t heightSquare = 20;
uint16_t valX;
uint16_t valY;

#if BOARD == EVK1105
#include "pwm.h"
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
#endif


__attribute__ ((__interrupt__))

void isr(void)
{

	if(gpio_get_pin_interrupt_flag(UP)){
		if (valY < ET024006_HEIGHT-heightSquare) {
			valY=valY+5;
		}
		gpio_clear_pin_interrupt_flag(UP);
	}
	if(gpio_get_pin_interrupt_flag(DOWN)){
		if (valY > 0) {
			valY=valY-5;
		}

		gpio_clear_pin_interrupt_flag(DOWN);
	}
	if(gpio_get_pin_interrupt_flag(RIGHT)){
		if (valX < ET024006_WIDTH-widthSquare) {
			valX=valX+5;
		}

		gpio_clear_pin_interrupt_flag(RIGHT);
	}
	if(gpio_get_pin_interrupt_flag(LEFT)){
		if (valX > 0) {
			valX=valX-5;
		}

		gpio_clear_pin_interrupt_flag(LEFT);
	}
	if(gpio_get_pin_interrupt_flag(ENTER)){

		gpio_clear_pin_interrupt_flag(ENTER);
	}

}

// Main function
int main(void)
{
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

	et024006_Init( FOSC0, FOSC0 );

	// Configuraci�n de interrupciones
	Disable_global_interrupt();
	INTC_init_interrupts();
	INTC_register_interrupt(&isr, IRQ70, 3);
	INTC_register_interrupt(&isr, IRQ71, 3);

	gpio_enable_pin_interrupt(UP , RISING);
	gpio_enable_pin_interrupt(DOWN , RISING);
	gpio_enable_pin_interrupt(RIGHT , RISING);
	gpio_enable_pin_interrupt(LEFT , RISING);
	gpio_enable_pin_interrupt(ENTER, RISING);

	Enable_global_interrupt();  //Habilita interrupciones globales

	tft_bl_init();

	valX = (ET024006_WIDTH/2) - widthSquare/2;
	valY = (ET024006_HEIGHT/2) - heightSquare/2;
	int valYprev=valX;
	int valXprev=valY;

	while(1){
		if(valX != valXprev || valY != valYprev){
			valYprev=valX;
			valXprev=valY;
			// Clear the display i.e. make it black
			et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, BLACK );
			et024006_DrawFilledRect(valX, valY, widthSquare, heightSquare,RED);
			delay_ms(100);
		}
	}

}
