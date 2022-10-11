/* Laboratorio 3 - main clock 
• Frame work among us
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
#define SHIELD			0xDEFF
#define FLOOR			0x9D12
#define REDBAND			0x8A0B
#define BLUEBAND		0x5412
#define CORRIDOR		0x84F4
#define CORRIDORWALL	0x436F
#define FULLCIRCLE		255


//
void newMask(et024006_color_t new_mask_color);
void background(void);


void newMask(et024006_color_t new_mask_color){
	uint16_t y = 0, prev_y, prev_length;
	for (double i = 0; i <= 320; i++){
		prev_y = y;
		y = round(sqrt(14400*(1-(pow((i-160)/160, 2)))));
		if (i <= 160){
			if (y - prev_y > 1 && i > 1){
				for (uint16_t j = y - prev_y; j > 0; j--){
					et024006_DrawHorizLine( 0, 120 - j - prev_y, i, new_mask_color);
					et024006_DrawHorizLine( 0, 120 + j + prev_y, i, new_mask_color);	
				}
			}
			et024006_DrawHorizLine( 0, 120-y, i, new_mask_color);
			et024006_DrawHorizLine( 0, 120+y, i, new_mask_color);
		} else {
			if (prev_y - y> 1 && i < 320){
				for (uint16_t j = prev_y - y; j > 0; j--){
					et024006_DrawHorizLine( i, 120 + j - prev_y, 320 - i, new_mask_color);
					et024006_DrawHorizLine( i, 120 - j + prev_y, 320 - i, new_mask_color);
				}
			}
			et024006_DrawHorizLine( i, 120-y, 320 - i, new_mask_color);
			et024006_DrawHorizLine( i, 120+y, 320 - i, new_mask_color);
		}
	}
}


void background(void){
	//Floor
	et024006_DrawFilledRect(0,80, 320, 160, FLOOR);
	//Corridor
	uint16_t floor_lenght = 18, wall_length = 80;
	for (uint16_t j = 0; j < 32; j++){
		et024006_DrawHorizLine( 151 - j, 49 + j, floor_lenght, CORRIDOR);
		if (j == 16){
			et024006_DrawHorizLine( 151 - j, 49 + j, floor_lenght, 0x3229);
		}
		floor_lenght+=2;
	}
	for (uint16_t j = 0; j < 16; j++){
		et024006_DrawHorizLine( 120 + 2*j, j, floor_lenght, CORRIDOR);
		if (j == 8){
			et024006_DrawHorizLine( 120 + 2*j, j, floor_lenght, 0x3229);
		}
		floor_lenght-=4;
	}
	for (uint16_t j = 0; j < 16; j++){
		et024006_DrawVertLine( 120 + 2*j, j, wall_length, CORRIDORWALL);
		et024006_DrawVertLine( 120 + 2*j + 1, j, wall_length - 1, CORRIDORWALL);
		wall_length-=3;
	}
	for (uint16_t j = 0; j < 16; j++){
		et024006_DrawVertLine( 169 + 2*j, 17 - j, wall_length+1, CORRIDORWALL);
		et024006_DrawVertLine( 169 + 2*j - 1, 17 - j, wall_length+1, CORRIDORWALL);
		wall_length+=3;
	}
	//Wall
	et024006_DrawFilledRect(0,0, 120, 40, BLUEBAND);
	et024006_DrawFilledRect(200,0, 120, 40, BLUEBAND);
	et024006_DrawFilledRect(0,40, 120, 40, REDBAND);
	et024006_DrawFilledRect(200,40, 120, 40, REDBAND);
	
	//Door
	et024006_DrawHorizLine(0, 80, 120, BLACK);
	et024006_DrawHorizLine(200, 80, 120, BLACK);
	et024006_DrawVertLine(120, 0, 80, BLACK);
	et024006_DrawVertLine(200, 0, 80, BLACK);
 	et024006_DrawHorizLine(120, 0, 80, BLACK);
	
	//Small Door
	et024006_DrawLine(120, 80, 152, 48, BLACK);
	et024006_DrawLine(168, 48, 200, 80, BLACK);
	et024006_DrawLine(120, 0, 152, 16, BLACK);
	et024006_DrawLine(168, 16, 200, 0, BLACK);
	et024006_DrawVertLine(152, 16, 32, BLACK);
	et024006_DrawVertLine(168, 16, 32, BLACK);
	et024006_DrawHorizLine(152, 48, 16, BLACK);
	et024006_DrawHorizLine(152, 16, 16, BLACK);
	
	//left table
	et024006_DrawFilledRect(0, 120, 60, 10, BLUE);
	et024006_DrawFilledRect(0, 131, 10, 10, BLUE);
	et024006_DrawFilledRect(40, 131, 30, 10, BLUE);
	
	//right table
	et024006_DrawFilledRect(260, 120, 60, 10, BLUE);
	et024006_DrawFilledRect(250, 131, 30, 10, BLUE);
	et024006_DrawFilledRect(310, 131, 10, 10, BLUE);
	
	//front table
	et024006_DrawFilledRect(125, 141, 70, 10, BLUE);
	et024006_DrawFilledRect(115, 152, 30, 10, BLUE);
	et024006_DrawFilledRect(175, 152, 30, 10, BLUE);
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
  et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, 0x52EB);
  background();
  newMask(RED);
  
  
  while(1);
}