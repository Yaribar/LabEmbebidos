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
    .cprd = 100};

static void tft_bl_init(void)
{

    pwm_opt_t opt = {
        .diva = 0,
        .divb = 0,
        .prea = 0,
        .preb = 0};
    /* MCK = OSC0 = 12MHz
     * Desired output 60kHz
     * Chosen MCK_DIV_2
     * CPRD = 12MHz / (60kHz * 2) = 100
     *
     * The duty cycle is 100% (CPRD = CDTY)
     * */
    pwm_init(&opt);
    pwm_channel6.CMR.calg = PWM_MODE_LEFT_ALIGNED;
    pwm_channel6.CMR.cpol = PWM_POLARITY_HIGH; // PWM_POLARITY_LOW;//PWM_POLARITY_HIGH;
    pwm_channel6.CMR.cpd = PWM_UPDATE_DUTY;
    pwm_channel6.CMR.cpre = AVR32_PWM_CMR_CPRE_MCK_DIV_2;

    pwm_channel_init(6, &pwm_channel6);
    pwm_start_channels(AVR32_PWM_ENA_CHID6_MASK);
}
#endif

#define MAX_LINES 10
uint8_t num_lines = 0;

void makeBlackTriangle(uint16_t x0, uint16_t y0, uint16_t lb, uint16_t lh, uint16_t x_ort, uint16_t y_ort, uint16_t orient);

int main(void)
{

    // Set CPU and PBA clock
    pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

    et024006_Init(FOSC0, FOSC0);

    /* PWM is fed by PBA bus clock which is by default the same
     * as the CPU speed. We set a 0 duty cycle and thus keep the
     * display black*/
    tft_bl_init();

    // Clear the display i.e. make it brown
    et024006_DrawFilledRect(0, 0, ET024006_WIDTH, ET024006_HEIGHT, WHITE);

    makeBlackTriangle(0, 240, 160, 120, 80, 60, 1);
    makeBlackTriangle(160, 240, 160, 120, 80, 60, 1);
    makeBlackTriangle(80, 120, 160, 120, 80, 60, 1);
    makeBlackTriangle(80, 120, 160, 120, 80, 60, 0);
    // et024006_DrawLine(0,240,80,120, BLACK);
}

// Punto de vértice inferior izquierdo
void makeBlackTriangle(uint16_t x0, uint16_t y0, uint16_t lb, uint16_t lh, uint16_t x_ort, uint16_t y_ort, uint16_t orient)
{
    uint16_t ten_percentage_x = lb / 10;              // No cambia
    uint16_t ten_percentage_y = y_ort / 10;           // No cambia
    uint16_t ten_percentage_diag = (lh + y_ort) / 10; // No cambia
    uint16_t new_offset_y = y0;
    uint16_t new_offset_x = x0;
    uint16_t new_final_y = y_ort;
    uint16_t new_final_x = x0 + lb / 2;
    uint16_t new_length = lb;
    uint16_t diag_final_y = lh;
    uint16_t diag_offset_y = lh;

    for (uint8_t i = 0; i < 10; i++)
    {
        if (i > 6)
        {
            num_lines = 2;
        }
        else if (i < 5)
        {
            num_lines = 1;
        }
        new_offset_x = x0 + (ten_percentage_x / 2) * i;
        new_length = (lb - (ten_percentage_x)*i);
        new_offset_y = y0 - (ten_percentage_y / 2) * i;
        new_final_y = y_ort - (ten_percentage_y / 2) * i;
        diag_final_y = (y0 - lh) + (ten_percentage_diag / 2) * i;

        if (orient)
        {

            for (uint8_t j = 0; j < num_lines; j++)
            {
                et024006_DrawLine((new_length + new_offset_x) - j, new_offset_y, new_final_x - j, diag_final_y - j, BLACK); // Pendiente negativa
                et024006_DrawLine(new_offset_x + j, new_offset_y, new_final_x + j, diag_final_y + j, BLACK);                  // Pendiente positiva
                et024006_DrawLine(new_final_x, diag_final_y + j, new_length - j + new_offset_x - j, new_offset_y, BLACK);     // Pendiente negativa
                et024006_DrawLine(new_final_x, diag_final_y + j, new_offset_x + j, new_offset_y, BLACK);                      // Pendiente positiva
                if (j < 2)
                {
                    et024006_DrawHorizLine(new_offset_x + j, new_offset_y - j, new_length - j, BLACK);
                }
            }
        }
        else
        {
            et024006_DrawLine((new_length + new_offset_x), (240 - new_offset_y) , new_final_x , (240 - diag_final_y) , BLACK); // Pendiente negativa
            et024006_DrawLine(new_offset_x , 240 - new_offset_y, new_final_x, 240 - (diag_final_y) , BLACK);                      // Pendiente positiva
            et024006_DrawLine(new_final_x, 240 - (diag_final_y), new_length + new_offset_x, 240-(new_offset_y) , BLACK);       // Pendiente negativa
            et024006_DrawLine(new_final_x, 240-(diag_final_y), new_offset_x, 240-new_offset_y, BLACK);                          // Pendiente positiva
            et024006_DrawHorizLine(new_offset_x, 240-new_offset_y, new_length, BLACK);
        }
    }
	
	if (orient)
	{
			for (uint8_t k = ten_percentage_x; k > 0; k--)
				{
					et024006_DrawHorizLine(x0 + ((x_ort-ten_percentage_x/2)+k),y0 - y_ort -k + 6*ten_percentage_y-4, ten_percentage_x-2*k,BLACK);
				}
	}
}
