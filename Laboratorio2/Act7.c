/* Laboratorio 2 - Ejercicio2 
• Dibujar un tablero de ajedrez (8x8).Utilizar blanco y negro para los cuadros.
• El tamaño inicial de cada “cuadro” deberá ser del tamaño de un pixel.
• Al presionarla tecla “arriba” el tablero de ajedrez crecerá tal que cada cuadro mida 2x2 pixels.
• Cada que se presione la tecla “arriba” el tamaño de cada cuadro crecerá en potencias de 2.
• De igual manera al presionar la tecla “abajo”, el cuadro decrecerá en potencias de 2.
• Limitar el tamaño mínimo y máximo para que la visualización en la pantalla sea adecuado.
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

void doFade(uint8_t r_start, uint8_t g_start, uint8_t b_start, uint8_t x_fade_start, uint8_t y_fade_start, uint8_t y_fade_size){
  
  for (uint8_t i = 0; i < 28; i++){ //from red to yellow
    g_start += 9;
    if (g_start > 244){
      g_start = 244;
    }
    newRGB = rgbto565(r_start, g_start, b_start);
    et024006_DrawFilledRect(x_fade_start + i,   y_fade_start, 1, y_fade_size, newRGB);
    if(i == 27){
      x_fade_start += i; 
    }
  }


  for (uint8_t i = 0; i < 28; i++){ //from yellow to green
    r_start -= 9;
    g_start += 1;
    if (g_start > 252){
      g_start = 252;
    }
    newRGB = rgbto565(r_start, g_start, b_start);
    et024006_DrawFilledRect(x_fade_start + i,   y_fade_start, 1, y_fade_size, newRGB);
    if(i == 27){
      x_fade_start += i; 
    }
  }

  
  for (uint8_t i = 0; i < 28; i++){ //from green to cyan
    g_start -= (252/28);
    b_start += (255/28);
    newRGB = rgbto565(r_start, g_start, b_start);
    et024006_DrawFilledRect(x_fade_start + i,   y_fade_start, 1, y_fade_size, newRGB);
    if(i == 27){
      x_fade_start += i; 
    }
  }

  
  for (uint8_t i = 0; i < 28; i++){ //from cyan to dark blue
    b_start -= 4;
    newRGB = rgbto565(r_start, g_start, b_start);
    et024006_DrawFilledRect(x_fade_start + i,   y_fade_start, 1, y_fade_size, newRGB);
    if(i == 27){
      x_fade_start += i; 
    }
  }


  for (uint8_t i = 0; i < 28; i++){ //from dark blue to pink
    r_start += 9;
    g_start += 2;
    b_start += 2;
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

/*void plotFade(uint16_t x_fade_start, uint16_t fade_size, uint16_t y_fade_start, uint16_t y_fade_size, et024006_color_t start_fade_color, et024006_color_t end_fade_color){
	et024006_color_t start_mask1 = start_fade_color & 0xFF00;
	et024006_color_t start_mask2 = start_fade_color & 0x00FF;
	et024006_color_t end_mask1 = end_fade_color & 0xFF00;
	et024006_color_t end_mask2 = end_fade_color & 0x00FF;
	uint8_t first_factor_up, second_factor_up;
	et024006_color_t step_first_factor, step_second_factor, next_step_first, next_step_second;
	
	if (start_mask1 > end_mask1){
		step_first_factor = round((start_mask1 - end_mask1) / fade_size);
		first_factor_up = 0;
	} else {
		step_first_factor = round((end_mask1 - start_mask1) / fade_size);
		first_factor_up = 1;
	}
	
	if (start_mask2 > end_mask2){
		step_second_factor = round((start_mask2 - end_mask2) / fade_size);
		second_factor_up = 0;
		} else {
		step_second_factor = round((end_mask2 - start_mask2) / fade_size);
		second_factor_up = 1;
	}
	
	for (uint16_t m = 0; m < fade_size; m++){
		if (first_factor_up == 1 && second_factor_up == 1){
			start_fade_color += (step_second_factor + step_first_factor);
			} else if (first_factor_up == 0 && second_factor_up == 1){
			start_mask1 = start_fade_color & 0xFF00;
			start_mask2 = start_fade_color & 0x00FF;
			
			next_step_first = start_mask1 - step_first_factor;
			next_step_second = start_mask2 + step_second_factor;
			
			start_fade_color = (next_step_second + next_step_first);
			
			} else if (first_factor_up == 1 && second_factor_up == 0){
			start_mask1 = start_fade_color & 0xFF00;
			start_mask2 = start_fade_color & 0x00FF;
			
			next_step_first = start_mask1 + step_first_factor;
			next_step_second = start_mask2 - step_second_factor;
			
			start_fade_color = (next_step_second + next_step_first);
			
			} else{
			start_fade_color -= (step_second_factor + step_first_factor);
		}
		
		et024006_DrawFilledRect(x_fade_start + m,   y_fade_start, 1, y_fade_size, start_fade_color);
	}
}*/

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
  
  plotPatern(0, 0, 45, 150, WHITE, YELLOW, CYAN, GREEN, PINK, RED, BLUE);
  plotPatern(0, 150, 45, 30, BLUE, PINK, YELLOW, RED, CYAN, BLACK, WHITE);
  
  /*for( uint16_t i = 0; i < 160 ; i++ )
  {
  et024006_DrawFilledRect(i,   180, 1, 30, (2*i/10 | (4*i/10<<5) | (2*i/10<<11) ));
  }*/
  /*for( uint16_t i = 0; i < 160 ; i++ )
  {
	  if (i > 139){
		  et024006_DrawFilledRect(2*(160-i)-2,   180, 2, 30, (2*i/10 | (4*i/10<<5) | (2*i/10<<11) ));
	  } else {
		  et024006_DrawFilledRect(179-i,   180, 1, 30, (2*i/10 | (4*i/10<<5) | (2*i/10<<11) ));
	  }
		  
  }*/
  plotFineFade(0,180,180,1,30);
  et024006_DrawFilledRect(180, 180, 15, 30, BLACK);
  et024006_DrawFilledRect(195, 180, 15, 30, RED);
  for (uint16_t m = 0; m < 13; m++){
		  et024006_DrawFilledRect(210+m,   180, 1, 30, color);
		  color = color + 0x00A0;
  }
  et024006_DrawFilledRect(223, 180, 5, 30, YELLOW);
  
  /*for (uint16_t m = 0; m < 45; m++){
	  et024006_DrawFilledRect(m,   210, 1, 30, new_color);
	  new_color -=1449;
  }*/
  //plotFade(228, 8, 180, 30, YELLOW, GREEN);
  //doFade(255, 244, 0, 0, 252, 0, 17, 228, 180, 30); // Fade from yellow to green
  //doFade(0, 252, 0, 0, 0, 255, 17, 247, 180, 30); // Fade from green to cyan
  //doFade(0, 0, 255, 0, 0, 140, 19, 264, 180, 30); // Fade from cyan to dark blue
  //doFade(0, 0, 140, 255, 53, 184, 17, 284, 180, 30); //Fade from dark blue to pink
  doFade(255, 0, 15, 180, 180, 30);
  while(1);
}