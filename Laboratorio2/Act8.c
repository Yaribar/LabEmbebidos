/* Laboratorio 8
• Diseñar una composición basada en el arte de Piet Mondrian abarcando la
totalidad del área del LCD.
• Elementos: fondo blanco, líneas horizontales y verticales negras. Tres colores:
azul, amarillo y rojo.
• Utilizarlas directivas de líneas y rectángulos.
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

#define MAXH 30
#define MAXW 60
#define MIN 5
#define RED 0xF800
#define YELLOW 0xFFE0

void drawBorder(uint16_t x, uint16_t y, uint16_t w);
void drawFirst (uint16_t x, uint16_t y, uint8_t m_x, uint8_t m_y);

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
  drawBorder(0,0,2);
  
  drawFirst(2,2,0,0);

}

// Bordes exteriores
void drawBorder(uint16_t x, uint16_t y, uint16_t w)
{
	et024006_DrawFilledRect(x,y,ET024006_WIDTH,ET024006_HEIGHT, BLACK);
	et024006_DrawFilledRect(x+w, y+w, ET024006_WIDTH-2*w, ET024006_HEIGHT-2*w, WHITE);
}

void drawFirst (uint16_t x, uint16_t y, uint8_t m_x, uint8_t m_y)
{
	uint8_t em_x = 0;
	uint8_t em_y = 0;
	if (m_x == 0 && m_y == 0) // estandar
	{
		uint8_t em_x = 0;
		uint8_t em_y = 0;
	} else if (m_x && m_y)
	{
		x = 320-x;
		y = 240-y;
		uint8_t em_x = 1;
		uint8_t em_y = 1;
	}
	else if (m_x)
	{
		x = 320-x;
		uint8_t em_x = 1;
	} else if (m_y)
	{
		y = 240-y;
		uint8_t em_y = 1;		
	} 
	
		et024006_DrawFilledRect(x+80-em_x*6,y, 6, 8, BLACK); // Primer cuadro en esquina sup izq
		et024006_DrawFilledRect(x+220-em_x*6,y, 6, 236/2, BLACK);
		et024006_DrawFilledRect(x+226-em_x*90,y, 90, 8, YELLOW);
		et024006_DrawFilledRect(x-em_x*316,y+8, 316, 2, BLACK); // Inicio de segunda fila
		et024006_DrawFilledRect(x-em_x*58,y+32, 58, 2, BLACK);
		et024006_DrawFilledRect(x-em_x*58,y+69, 58, 2, BLACK);
		et024006_DrawFilledRect(x-em_x*58,y+69, 58, 2, BLACK);
		et024006_DrawFilledRect(x-em_x*142,y+106, 142, 2, BLACK);
		et024006_DrawFilledRect(x+58-em_x*6,y+10, 6, 98, BLACK);
		et024006_DrawFilledRect(x-em_x*316,y+118, 316, 2, BLACK);
		et024006_DrawFilledRect(x+64-em_x*156,y+10, 156, 42, RED);
		et024006_DrawFilledRect(x+64-em_x*162,y+52, 162, 2, BLACK);
		et024006_DrawFilledRect(x+64-em_x*76,y+54, 76, 20, BLACK);
		et024006_DrawFilledRect(x+140-em_x*6,y+54, 6, 64, BLACK);
		et024006_DrawFilledRect(x-em_x*58,y+71, 58, 35, YELLOW);
		et024006_DrawFilledRect(x+146-em_x*76,y+62, 76, 2, BLACK);
		et024006_DrawFilledRect(x+146-em_x*76,y+72, 76, 2, BLACK);
		et024006_DrawFilledRect(x+146-em_x*76,y+74, 76, 10, BLACK);
		et024006_DrawFilledRect(x+146-em_x*35,y+84, 35, 34, YELLOW);
		et024006_DrawFilledRect(x+181-em_x*6,y+84, 6, 34, BLACK);
		et024006_DrawFilledRect(x+226-em_x*90,y+10, 90, 21, YELLOW);
		et024006_DrawFilledRect(x+226-em_x*90,y+31, 90, 2, BLACK);
		et024006_DrawFilledRect(x+226-em_x*90,y+62, 90, 2, BLACK);
		et024006_DrawFilledRect(x+226-em_x*90,y+72, 90, 2, BLACK);
		et024006_DrawFilledRect(x+226-em_x*90,y+74, 90, 25, BLUE);
		et024006_DrawFilledRect(x+226-em_x*90,y+99, 90, 2, BLACK);
		et024006_DrawFilledRect(x+226-em_x*90,y+101, 90, 17, RED);		
}
