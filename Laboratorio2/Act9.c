/* Laboratorio 2 - Ejercicio9 
• Desplegar el siguiente patrón de color a lo largo de toda la pantalla.
 */

#include "board.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "et024006dhu.h"
#include "delay.h"
#include "avr32_logo.h"
#include "conf_clock.h"
#include "math.h"

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

#define BROWN			0x8A22
#define YELLOW			0xFFA0
#define CYAN			0x07FF
#define PINK			0xF9B7
#define DARKBLUE		0x0011
#define FULLCIRCLE		255

uint16_t rgb, rgb_plot;

uint16_t rgbto565(uint16_t r, uint16_t g, uint16_t b);
void circleFade(uint16_t r_initial, uint16_t g_initial, uint16_t b_initial, int16_t r_factor, int16_t g_factor, int16_t b_factor, int16_t r_limit, int16_t g_limit, int16_t b_limit, uint16_t initial_radius);


uint16_t rgbto565(uint16_t r, uint16_t g, uint16_t b) {
	rgb = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
	return rgb;
}

void circleFade(uint16_t r_initial, uint16_t g_initial, uint16_t b_initial, int16_t r_factor, int16_t g_factor, int16_t b_factor, int16_t r_limit, int16_t g_limit, int16_t b_limit, uint16_t initial_radius){
	uint16_t r_plot = r_initial;
	uint16_t g_plot = g_initial;
	uint16_t b_plot = b_initial;
	for (uint16_t i = 0; i < 21; i++){
		r_plot += r_factor;
		g_plot += g_factor;
		b_plot += b_factor; 
		if (r_plot > r_limit && r_initial < r_limit){
			r_plot = r_limit;
			} else if (r_plot < r_limit && r_initial > r_limit){
			r_plot = r_limit;
		}
		if (g_plot > g_limit && g_initial < g_limit){
			g_plot = g_limit;
		} else if (g_plot < g_limit && g_initial > g_limit){
			g_plot = g_limit;
		}
		if (b_plot > b_limit && b_initial < b_limit){
			b_plot = b_limit;
		} else if (b_plot < b_limit && b_initial > b_limit){
			b_plot = b_limit;
		}
		rgb_plot = rgbto565(r_plot, g_plot, b_plot);
		et024006_DrawFilledCircle(160,120, (initial_radius - i), rgb_plot, FULLCIRCLE);
	}
}


// Main function
int main(void)
{

  // Set CPU and PBA clock
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

  et024006_Init( FOSC0, FOSC0 );

  /* PWM is fed by PBA bus clock which is by default the same
   * as the CPU speed. We set a 0 duty cycle and thus keep the
   * display black*/
  tft_bl_init();

  // Clear the display i.e. make it red
  et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, RED);
  circleFade(255, 0, 0, 0, 12, 0, 255, 244, 0, 116); // Fade from RED to YELLOW
  circleFade(255, 244, 0, -13, 1, 0, 0, 252, 0, 96); // Fade from YELLOW to GREEN
  circleFade(0, 252, 0, 0, 1, 13, 0, 255, 255, 77); // Fade from GREEN to CYAN
  circleFade(0, 255, 255, 0, -13, -6, 0, 0, 140, 56); // Fade from CYAN to DARKBLUE
  circleFade(0, 0, 140, 13, 3, 2, 255, 53, 184, 37); // Fade from DARKBLUE to PINK

  while(1);
}