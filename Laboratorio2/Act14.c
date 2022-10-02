/* Laboratorio 2 - Ejercicio14 
• Implementar dos figuras de geometría imposible.
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

uint16_t x_center_figure1 = 160, y_center_figure1 = 60, minimum_units = 13;
uint16_t x_center_figure2 = 160, y_center_figure2 = 180, minimum_units2 = 13;
void impossibleFigure1(uint16_t x_center, uint16_t y_center, uint16_t minimum_unit);
void impossibleFigure2(uint16_t x_center_two, uint16_t y_center_two, uint16_t minimum_unit_two); 

void impossibleFigure1(uint16_t x_center, uint16_t y_center, uint16_t minimum_unit){
	et024006_DrawHorizLine(x_center - (2*minimum_unit + (minimum_unit-1)/2), y_center - (3*minimum_unit), 5*minimum_unit, BLACK); //top line
	et024006_DrawLine(x_center - (2*minimum_unit + (minimum_unit-1)/2), y_center - (3*minimum_unit), x_center - (5*minimum_unit + (minimum_unit-1)/2), y_center + (3*minimum_unit), BLACK); //Far right line
	et024006_DrawHorizLine(x_center - (minimum_unit + (minimum_unit-1)/2), y_center - 2*minimum_unit, 3*minimum_unit, BLACK); //top 3 square line
	et024006_DrawLine(x_center - (minimum_unit + (minimum_unit-1)/2), y_center - 2*minimum_unit, x_center - 4*minimum_unit, y_center + 3*minimum_unit, BLACK); //line / from top 3 square line to bottom 11
	et024006_DrawLine(x_center - (2*minimum_unit + (minimum_unit-1)/2), y_center - (3*minimum_unit), x_center - (minimum_unit + (minimum_unit-1)/2), y_center - 2*minimum_unit, BLACK); //line \ from top line to top 3 square line
	et024006_DrawHorizLine(x_center - (minimum_unit-1)/2, y_center - minimum_unit, minimum_unit, BLACK); // top 1 square line
	et024006_DrawLine(x_center - (minimum_unit-1)/2, y_center - minimum_unit, x_center - (2*(minimum_unit)), y_center + (2*minimum_unit), BLACK);
	et024006_DrawLine(x_center + (minimum_unit-1)/2, y_center - minimum_unit, x_center + (minimum_unit + (minimum_unit-1)/2), y_center + minimum_unit, BLACK);
	et024006_DrawLine(x_center + (minimum_unit + (minimum_unit-1)/2), y_center - 2*minimum_unit, x_center + (minimum_unit-1)/2, y_center - minimum_unit, BLACK); //line / from top 3 square line to top 1 square line
	et024006_DrawLine(x_center + (minimum_unit + (minimum_unit-1)/2), y_center - 2*minimum_unit, x_center + (3*minimum_unit), y_center + minimum_unit, BLACK);
	et024006_DrawHorizLine(x_center - (minimum_unit + (minimum_unit-1)/2), y_center + minimum_unit, 4*minimum_unit + (minimum_unit-1)/2, BLACK); //bottom 4 and half square line
	et024006_DrawHorizLine(x_center - 2*minimum_unit, y_center + 2*minimum_unit, 7*minimum_unit, BLACK); //bottom 7 square line
	et024006_DrawLine(x_center + (2*minimum_unit + (minimum_unit-1)/2), y_center - (3*minimum_unit), x_center + (5*minimum_unit + (minimum_unit-1)/2), y_center + (3*minimum_unit), BLACK); //Far left line
	et024006_DrawHorizLine(x_center - (5*minimum_unit + (minimum_unit-1)/2), y_center + (3*minimum_unit), 11*minimum_unit, BLACK); // bottom 11 square line
}

void impossibleFigure2(uint16_t x_center_two, uint16_t y_center_two, uint16_t minimum_unit_two){
	et024006_DrawFilledCircle(x_center_two, y_center_two, 4*minimum_unit_two, BLACK, FULLCIRCLE);
	et024006_DrawFilledCircle(x_center_two, y_center_two, 4*minimum_unit_two - 1, WHITE, FULLCIRCLE);
	
	et024006_DrawFilledCircle(x_center_two - minimum_unit_two-1, y_center_two, 3*minimum_unit_two, BLACK, FULLCIRCLE);
	et024006_DrawFilledCircle(x_center_two - minimum_unit_two-1, y_center_two, 3*minimum_unit_two -1, WHITE, FULLCIRCLE);
	et024006_DrawFilledCircle(x_center_two - minimum_unit_two- 2, y_center_two - 2*minimum_unit_two + 5, 2*minimum_unit_two -1, WHITE, FULLCIRCLE);
	et024006_DrawFilledCircle(x_center_two - 3*minimum_unit_two, y_center_two - 2*minimum_unit_two + 1, 5, WHITE, FULLCIRCLE);
	
	et024006_DrawFilledCircle(x_center_two + minimum_unit_two, y_center_two, 3*minimum_unit_two, BLACK, FULLCIRCLE);
	et024006_DrawFilledCircle(x_center_two + minimum_unit_two, y_center_two, 3*minimum_unit_two -1, WHITE, FULLCIRCLE);
	et024006_DrawFilledCircle(x_center_two + minimum_unit_two+1, y_center_two + 2*minimum_unit_two - 5, 2*minimum_unit_two -1, WHITE, FULLCIRCLE);
	et024006_DrawFilledCircle(x_center_two + 3*minimum_unit_two, y_center_two + 2*minimum_unit_two - 1, 5, WHITE, FULLCIRCLE);
	
	et024006_DrawFilledCircle(x_center_two, y_center_two, 3*minimum_unit_two, BLACK, FULLCIRCLE);
	et024006_DrawFilledCircle(x_center_two, y_center_two, 3*minimum_unit_two - 1, WHITE, FULLCIRCLE);
	
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
  impossibleFigure1(x_center_figure1, y_center_figure1, minimum_units);
  impossibleFigure2(x_center_figure2, y_center_figure2, minimum_units2);
  while(1);
}