/* Actividad 5 
Modificar el tablero del ejercicio 2 para que el color de cada cuadro sea diferente y cambie de manera aleatoria continuamente
*/
#include "board.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "et024006dhu.h"
#include "delay.h"
#include "avr32_logo.h"
#include "conf_clock.h"
#include "math.h"
#include "time.h"

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

#define BROWN 0x8A22
#define UP		QT1081_TOUCH_SENSOR_UP
#define DOWN	QT1081_TOUCH_SENSOR_DOWN

uint32_t chessboard[8][8] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1};
uint32_t i, j, k, l, current_size, prev_size;
uint8_t current_up_state, prev_up_state, current_down_state, prev_down_state, lower = 0;
uint64_t upper = 0xFFFF;

void plotChessboard(uint32_t chessboard_size){
	uint32_t new_size = pow(2, chessboard_size);
	for (i = 0; i < 8; i++){
		for (j = 0; j < 8; j++){
            uint64_t new_color = (rand()%(upper-lower+1))+lower;
			if (chessboard[i][j] == 0){
				et024006_DrawFilledRect(new_size*i , new_size*j, new_size, new_size, new_color );
				} else {
				et024006_DrawFilledRect(new_size*i , new_size*j, new_size, new_size, new_color );
			}
		}
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

  // Clear the display i.e. make it brown
  et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, BROWN );
  srand(time(NULL));
  while(1){
	  current_up_state = gpio_get_pin_value(UP);
	  current_down_state = gpio_get_pin_value(DOWN);
      
	  
	  if (current_up_state != prev_up_state){
		  for (k = 0; k < 3000; k++){
			  current_up_state = gpio_get_pin_value(UP);
		  }
		  
		  if (current_size == 4){
			  current_size = 4;
			  } else {
			  current_size++;
		  }
		  prev_up_state = current_up_state;
	  }
	  
	  if (current_down_state != prev_down_state){
		  for (l = 0; l < 3000; l++){
			  current_down_state = gpio_get_pin_value(DOWN);
		  }
		  
		  if (current_size == 0){
			  current_size = 0;
			  } else {
			  current_size--;
		  }
		  prev_down_state = current_down_state;
	  }
	  
	  if (current_size != prev_size){
		  prev_size = current_size;
		  et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, BROWN );
	  }
	  
	  plotChessboard(current_size);
  }
}