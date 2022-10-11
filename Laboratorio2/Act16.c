/* Laboratorio 2 - Ejercicio16 
• Implementar dos figuras de un Tangram.
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
#define ORANGE			0xFC60
#define FULLCIRCLE		255

uint16_t base = 160, height = 60, minimum_units = 13;

//only symmetrical triangles allowed hence only base is a parameter, orientation tells if it faces up(1) or down (0), mirror tells if it starts from left-right(1) or right(left)(0)
void makeTriangle(uint16_t triangle_x_start, uint16_t triangle_y_start, uint16_t triangle_base, uint16_t orientation, uint16_t mirror, et024006_color_t triangle_color);
void tangramFigure1(uint16_t x_center_figure_one, uint16_t y_center_figure_one);
void tangramFigure2(uint16_t x_center_figure_two, uint16_t y_center_figure_two);

void makeTriangle(uint16_t triangle_x_start, uint16_t triangle_y_start, uint16_t triangle_base, uint16_t orientation, uint16_t mirror, et024006_color_t triangle_color){
	uint16_t triangle_length = triangle_base;
	if (orientation){
		if (!mirror){
			for (uint16_t i = 0; i < triangle_base; i++){
				et024006_DrawHorizLine(triangle_x_start + i, triangle_y_start - i, (triangle_length - i), triangle_color);
			}
		} else{
			for (uint16_t i = 0; i < triangle_base; i++){
				et024006_DrawHorizLine(triangle_x_start - triangle_base, triangle_y_start - i, (triangle_length - i), triangle_color);
			}
		}
	} else {
		if (!mirror){
			for (uint16_t i = 0; i < triangle_base; i++){
				et024006_DrawHorizLine(triangle_x_start + i, triangle_y_start + i, (triangle_length - i), triangle_color);
			}
		} else{
			for (uint16_t i = 0; i < triangle_base; i++){
				et024006_DrawHorizLine(triangle_x_start - triangle_base, triangle_y_start + i, (triangle_length - i), triangle_color);
			}
		}
	}
}

void tangramFigure1(uint16_t x_center_figure_one, uint16_t y_center_figure_one){
	makeTriangle(x_center_figure_one + minimum_units - 1, y_center_figure_one - minimum_units, minimum_units, 0, 1, ORANGE);
	makeTriangle(x_center_figure_one - minimum_units - 1, y_center_figure_one - minimum_units, minimum_units, 0, 0, ORANGE);
	makeTriangle(x_center_figure_one - 1, y_center_figure_one, minimum_units, 1, 1, GREEN);
	makeTriangle(x_center_figure_one - 2*minimum_units, y_center_figure_one, minimum_units, 1, 0, GREEN);
	et024006_DrawFilledRect(x_center_figure_one, y_center_figure_one -  + 2*minimum_units, minimum_units, minimum_units, YELLOW);
	makeTriangle(x_center_figure_one + 2*minimum_units, y_center_figure_one - 2*minimum_units, minimum_units, 1, 1, RED);
	makeTriangle(x_center_figure_one, y_center_figure_one - 2*minimum_units, minimum_units, 1, 0, RED);
	makeTriangle(x_center_figure_one - 1, y_center_figure_one, minimum_units, 0, 1, BLUE);
	makeTriangle(x_center_figure_one - 2*minimum_units - 1, y_center_figure_one + minimum_units, minimum_units, 1, 0, BLUE);
	makeTriangle(x_center_figure_one - 2*minimum_units - 1, y_center_figure_one + minimum_units + 1, minimum_units, 0, 0, RED);
	makeTriangle(x_center_figure_one - minimum_units + 3, y_center_figure_one + 3*minimum_units - 8, minimum_units, 1, 1, DARKBLUE);
}

void tangramFigure2(uint16_t x_center_figure_two, uint16_t y_center_figure_two){
	makeTriangle(x_center_figure_two, y_center_figure_two, 2*minimum_units, 1, 0, GREEN);
	makeTriangle(x_center_figure_two, y_center_figure_two - 2*minimum_units, minimum_units, 0, 1, DARKBLUE);
	makeTriangle(x_center_figure_two - minimum_units, y_center_figure_two - 2*minimum_units - 1, minimum_units, 1, 0, DARKBLUE);
	makeTriangle(x_center_figure_two + minimum_units, y_center_figure_two - 3*minimum_units - 1, minimum_units, 0, 1, DARKBLUE);
	makeTriangle(x_center_figure_two + minimum_units, y_center_figure_two - minimum_units, 2*minimum_units, 0, 1, ORANGE);
	makeTriangle(x_center_figure_two + 5, y_center_figure_two + 1, 21, 0, 0, RED);
	makeTriangle(x_center_figure_two + 18, y_center_figure_two + 14, 7, 1, 1, BLUE);
	makeTriangle(x_center_figure_two + 18, y_center_figure_two + 14, 7, 0, 1, BLUE);
	makeTriangle(x_center_figure_two - 19, y_center_figure_two + 13, 7, 1, 0, RED);
	makeTriangle(x_center_figure_two - 19, y_center_figure_two + 13, 7, 0, 0, RED);
	et024006_DrawFilledRect(x_center_figure_two + 2*minimum_units, y_center_figure_two - 2*minimum_units, minimum_units, minimum_units, YELLOW);
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
  et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, WHITE);
  tangramFigure1(160, 60);
  tangramFigure2(160, 180);
  
  while(1);
}
