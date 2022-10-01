/* Laboratorio 2 - Ejercicio7 
• Desplegar en todo el display la prueba de color utilizada en los sistemas de
video.
• Tomar en cuenta que en dicha prueba de color hay barras que muestran
degradados de color y en escala de grises.
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

#define BROWN	0x8A22
#define YELLOW	0xFFA0
#define CYAN	0x07FF
#define PINK	0xF9B7
#define DARKBLUE	0x0011

uint32_t i, j, k, l, m, current_size, prev_size;
et024006_color_t color = 0xF800, new_color = 0xFFA0;
uint8_t current_up_state, prev_up_state, current_down_state, prev_down_state;
uint16_t rgb, r_up = 1, g_up = 1, b_up = 1, r_factor, g_factor, b_factor, newRGB;

uint16_t rgbto565(uint16_t r, uint16_t g, uint16_t b);

void plotPatern(uint16_t x_offset, uint16_t y_offset, uint16_t x_size, uint16_t y_size, et024006_color_t first_element, et024006_color_t second_element, et024006_color_t third_element, et024006_color_t fourth_element, et024006_color_t fifth_element, et024006_color_t sixth_element, et024006_color_t seventh_element){
	for (uint8_t i = 0; i < 7; i++){
		if (i == 0){
			et024006_DrawFilledRect(x_size*i + x_offset , y_size*j + y_offset, x_size, y_size, first_element );
		} else if (i == 1){
			et024006_DrawFilledRect(x_size*i + x_offset , y_size*j + y_offset, x_size, y_size, second_element );
		} else if (i == 2){
		et024006_DrawFilledRect(x_size*i + x_offset , y_size*j + y_offset, x_size, y_size, third_element );
		} else if (i == 3){
		et024006_DrawFilledRect(x_size*i + x_offset , y_size*j + y_offset, x_size, y_size, fourth_element );
		} else if (i == 4){
		et024006_DrawFilledRect(x_size*i + x_offset , y_size*j + y_offset, x_size, y_size, fifth_element );
		} else if (i == 5){
		et024006_DrawFilledRect(x_size*i + x_offset , y_size*j + y_offset, x_size, y_size, sixth_element );
		} else {
		et024006_DrawFilledRect(x_size*i + x_offset , y_size*j + y_offset, x_size + 5, y_size, seventh_element );
		}
	}
}

void plotFineFade(uint16_t xfade_start, uint16_t yfade_start, uint16_t xfade_end, uint16_t x_size, uint16_t y_size){
	for( uint16_t i = xfade_start; i < xfade_end ; i++ )
	{
		if (i > xfade_end-41){
			et024006_DrawFilledRect(2*(xfade_end-20-i)-2,   yfade_start, x_size + 1, y_size, (2*i/10 | (4*i/10<<5) | (2*i/10<<11) ));
			} else {
			et024006_DrawFilledRect(xfade_end-1-i,   yfade_start, x_size, y_size, (2*i/10 | (4*i/10<<5) | (2*i/10<<11) ));
		}
		
	}
}


uint16_t rgbto565(uint16_t r, uint16_t g, uint16_t b) {
	rgb = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
	//printf("%i\n", rgb);
	return rgb;
}

void doFade(uint16_t r_in, uint16_t g_in, uint16_t b_in, uint16_t x_fade_start, uint16_t y_fade_start, uint16_t y_fade_size){
  uint16_t r_start = r_in;
  uint16_t g_start = g_in;
  uint16_t b_start = b_in;
  for (uint8_t i = 0; i < 28; i++){ //from red to yellow
    g_start += 9;
    if (g_start > 244){
      g_start = 244;
    }
    newRGB = rgbto565(r_start, g_start, b_start);
    et024006_DrawFilledRect(x_fade_start + i,   y_fade_start, 1, y_fade_size, newRGB);
    
  }
  x_fade_start += 27; 

  for (uint8_t i = 0; i < 28; i++){ //from yellow to green
    r_start -= 9;
    g_start += 1;
    if (g_start > 252){
      g_start = 252;
    }
    newRGB = rgbto565(r_start, g_start, b_start);
    et024006_DrawFilledRect(x_fade_start + i,   y_fade_start, 1, y_fade_size, newRGB); 
  }
  x_fade_start += 27;
  b_start = 0; 

  for (uint8_t i = 0; i < 28; i++){ //from green to cyan
    g_start += 1;
    b_start += 10;
	if (g_start > 255){
		g_start = 255;
	}
	if (b_start > 255){
		b_start = 255;
	}
    newRGB = rgbto565(r_start, g_start, b_start);
    et024006_DrawFilledRect(x_fade_start + i,   y_fade_start, 1, y_fade_size, newRGB);
  }
  x_fade_start += 27;
  r_start = 0; 
  
  for (uint8_t i = 0; i < 28; i++){ //from cyan to dark blue
    g_start -= 10;
	b_start -= 1;
	if (b_start < 140){
		b_start = 140;
	}
	if (g_start < 0){
		g_start = 0;
	}
    newRGB = rgbto565(r_start, g_start, b_start);
    et024006_DrawFilledRect(x_fade_start + i,   y_fade_start, 1, y_fade_size, newRGB);
  }
	x_fade_start += 27; 

  for (uint8_t i = 0; i < 28; i++){ //from dark blue to pink
    r_start += 9;
    g_start += 2;
    b_start += 2;
    if (r_start > 255){
	    r_start = 255;
    }
	if (g_start > 53){
      g_start = 53;
    }
    if (b_start > 184){
      b_start = 184;
    }
    newRGB = rgbto565(r_start, g_start, b_start);
    et024006_DrawFilledRect(x_fade_start + i,   y_fade_start, 1, y_fade_size, newRGB);
  }
}

void blacktoWhite12(uint16_t x_fade_start, uint16_t y_fade_start, uint16_t y_fade_size){
	uint16_t r_b2w = 0;
	uint16_t g_b2w = 0;
	uint16_t b_b2w = 0;
	uint16_t to_multiple = 0;
	for (uint16_t i = 0; i < 180; i++){ //from red to yellow
		r_b2w += 8;
		g_b2w += 8;
		b_b2w += 8;
		if (i % 15 == 0){
			to_multiple = i/21;
			r_b2w = 21*to_multiple;
			g_b2w = 21*to_multiple;
			b_b2w = 21*to_multiple;
		}
		if (r_b2w > 255){
			r_b2w = 255;
			g_b2w = 255;
			b_b2w = 255;
		}
		newRGB = rgbto565(r_b2w, g_b2w, b_b2w);
		et024006_DrawFilledRect(x_fade_start + i,   y_fade_start, 1, y_fade_size, newRGB);
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
  et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, BROWN);
  
  // Functions to do proper screen test
  plotPatern(0, 0, 45, 150, WHITE, YELLOW, CYAN, GREEN, PINK, RED, BLUE);
  plotPatern(0, 150, 45, 30, BLUE, PINK, YELLOW, RED, CYAN, BLACK, WHITE);
  plotFineFade(0,180,180,1,30);
  et024006_DrawFilledRect(180, 180, 5, 30, BLACK);
  doFade(255, 0, 0, 185, 180, 30);
  et024006_DrawFilledRect(291, 180, 2, 30, DARKBLUE);  ////CAMBIAR COLOR
  blacktoWhite12(0, 210, 30);
  et024006_DrawFilledRect(180, 210, 160, 30, BLACK);
  while(1);
}