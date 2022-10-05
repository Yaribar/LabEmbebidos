/* Laboratorio 2 - Ejercicio 1
• Dibujar un tablero de ajedrez (8x8).Utilizar blanco y negro para los cuadros.
 */

// Libraries
#include "board.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "et024006dhu.h"
#include "delay.h"
#include "avr32_logo.h"
#include "conf_clock.h"
#include "math.h"

// Macros
#define BLACK 0x0000;
#define WHITE 0xFFFF;
#define ET024006_WIDTH 320;
#define ET024006_HEIGHT 240;
#define OFFSET 40;
#define LENGTH 30;

// Initialization of the board
#if BOARD == EVK1105
#include "pwm.h"
avr32_pwm_channel_t pwm_channel6 = {
  .cdty = 100,
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

  pwm_init(&opt);
  pwm_channel6.CMR.calg = PWM_MODE_LEFT_ALIGNED;
  pwm_channel6.CMR.cpol = PWM_POLARITY_HIGH; //PWM_POLARITY_LOW;//PWM_POLARITY_HIGH;
  pwm_channel6.CMR.cpd = PWM_UPDATE_DUTY;
  pwm_channel6.CMR.cpre = AVR32_PWM_CMR_CPRE_MCK_DIV_2;

  pwm_channel_init(6, &pwm_channel6);
  pwm_start_channels(AVR32_PWM_ENA_CHID6_MASK);

}
#endif

// Variables
uint32_t current_color;
uint32_t previous_color;

int main(void)
{
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP); // Set CPU and PBA clock
  et024006_Init( FOSC0, FOSC0 );

  tft_bl_init();   //We set a 0 duty cycle and thus keep the display black*/

  et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, BLACK );   // Clear the display i.e. make it black

  for (int i=0; i<8; i++)
  {
    if (i%2 == 0)
    {
      current_color = WHITE;
    } else if (i%2 == 1)
    {
      current_color = BLACK;
    }
    for (int j = 0; i<8; j++)
    {
      if (previous_color == WHITE)
      {
        current_color = BLACK;
      } else
      {
        current_color = WHITE;
      }
      et024006_DrawFilledRect(LENGTH*j+OFFSET, LENGTH*i, LENGTH, LENGTH, current_color);
      previous_color = current_color;
    }
  }

}
