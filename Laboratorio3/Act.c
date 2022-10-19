#include <stdint.h>
#include <stdbool.h>

#include "tc.h"
#include "sysclk.h"
#include "board.h"
#include "gpio.h"
#include "intc.h"
#include "eic.h"
#include "conf_example.h"
#include "power_clocks_lib.h"
#include "et024006dhu.h"
#include "delay.h"
#include "avr32_logo.h"
#include "conf_clock.h"
#include "math.h"
#include "pwm.h"

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

#define IRQ33 33
#define LEVEL 3
#define NLINE 1
#define LED0 LED0_GPIO
#define UP 54
#define DOWN 55
#define RIGHT 56
#define LEFT 57
#define ENTER 58

#define RELOJ 0
#define CHRONO 1
#define APPS 2

volatile ms10_tick=0, second_tick=0;
volatile static uint32_t seconds_tick = 0, minutes_tick = 0, hours_tick = 0;
uint32_t alarm_clock_seconds=0, alarm_clock_minutes=0, alarm_clock_hours=0;

uint16_t int_flag = 0;

uint8_t current_up_state, current_down_state;
uint8_t current_left_state, current_right_state;
uint8_t current_enter_state;
uint16_t k;

int selector;

uint8_t fu=0;
uint8_t fd=0;
uint8_t fe=0;
uint8_t fr=0;
uint8_t fl=0;

void intFlag(void);
void reloj(void);
void chronometer(void);
void apps(void);
void setAlarm(int hour, int minutes, int seconds);
int checkAlarm(int hour, int minutes, int seconds);
void newMask(et024006_color_t new_mask_color);
void background(void);
void resetClockCounter(void);

#if BOARD == EVK1105

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

__attribute__((__interrupt__)) 
static void tc_irq(void)
{
    
	ms10_tick++;
	
    // Clear the interrupt flag. This is a side effect of reading the TC SR.
    tc_read_sr(EXAMPLE_TC, EXAMPLE_TC_CHANNEL);

    // Toggle the GPIO line
	if(ms10_tick==100){
		gpio_tgl_gpio_pin(LED0);
        second_tick=1;
	}

}

static void tc_init(volatile avr32_tc_t *tc)
{

    // Options for waveform generation.
    static const tc_waveform_opt_t waveform_opt = {
        // Channel selection.
        .channel = EXAMPLE_TC_CHANNEL,
        // Software trigger effect on TIOB.
        .bswtrg = TC_EVT_EFFECT_NOOP,
        // External event effect on TIOB.
        .beevt = TC_EVT_EFFECT_NOOP,
        // RC compare effect on TIOB.
        .bcpc = TC_EVT_EFFECT_NOOP,
        // RB compare effect on TIOB.
        .bcpb = TC_EVT_EFFECT_NOOP,

        // Software trigger effect on TIOA.
        .aswtrg = TC_EVT_EFFECT_NOOP,
        // External event effect on TIOA.
        .aeevt = TC_EVT_EFFECT_NOOP,
        // RC compare effect on TIOA.
        .acpc = TC_EVT_EFFECT_NOOP,
        /* RA compare effect on TIOA.
         * (other possibilities are none, set and clear).
         */
        .acpa = TC_EVT_EFFECT_NOOP,
        /* Waveform selection: Up mode with automatic trigger(reset)
         * on RC compare.
         */

        .wavsel = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
        // External event trigger enable.

        .enetrg = false,
        // External event selection.
        .eevt = 0,
        // External event edge selection.
        .eevtedg = TC_SEL_NO_EDGE,
        // Counter disable when RC compare.

        .cpcdis = false,
        // Counter clock stopped with RC compare.
        .cpcstop = false,
        // Burst signal selection.

        .burst = false,
        // Clock inversion.
        .clki = false,
        // Internal source clock 3, connected to fPBA / 8.
        .tcclks = TC_CLOCK_SOURCE_TC3};

    // Options for enabling TC interrupts
    static const tc_interrupt_t tc_interrupt = {
        .etrgs = 0,
        .ldrbs = 0,
        .ldras = 0,
        .cpcs = 1, // Enable interrupt on RC compare alone
        .cpbs = 0,
        .cpas = 0,
        .lovrs = 0,
        .covfs = 0};
    // Initialize the timer/counter.
    tc_init_waveform(tc, &waveform_opt);

    /*
     * Set the compare triggers.
     * We configure it to count every 1 second.
     * We want: (1 / (fPBA / 8)) * RC = 10ms, hence RC = (fPBA / 8) / 1
     * to get an interrupt every 10 ms.
     */
    tc_write_rc(tc, EXAMPLE_TC_CHANNEL, (FOSC0 / 8)/100);
    // configure the timer interrupt
    tc_configure_interrupts(tc, EXAMPLE_TC_CHANNEL, &tc_interrupt);
    // Start the timer/counter.
    tc_start(tc, EXAMPLE_TC_CHANNEL);
}

__attribute__((__interrupt__)) void teclas_config_isr(void)
{
    int_flag = 1;
    eic_clear_interrupt_line(&AVR32_EIC, 1);
}

int main(void)
{
    volatile avr32_tc_t *tc = EXAMPLE_TC;
    uint32_t timer = 0;
    /**
     * \note the call to sysclk_init() will disable all non-vital
     * peripheral clocks, except for the peripheral clocks explicitly
     * enabled in conf_clock.h.
     */
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

    // Disable the interrupts
    cpu_irq_disable();
    Disable_global_interrupt();
    // Initialize interrupt vectors.
    INTC_init_interrupts();
    INTC_register_interrupt(&teclas_config_isr, IRQ33, LEVEL);
	
	//EIC
    eic_options_t eic_options;
    eic_options.eic_mode = EIC_MODE_EDGE_TRIGGERED;
    eic_options.eic_edge = EIC_EDGE_RISING_EDGE;
    eic_options.eic_async = EIC_SYNCH_MODE;
    eic_options.eic_line = NLINE;
	
    eic_init(&AVR32_EIC, &eic_options, NLINE);
    eic_enable_line(&AVR32_EIC, NLINE);
    eic_enable_interrupt_line(&AVR32_EIC,NLINE);
	
    // Register the RTC interrupt handler to the interrupt controller.
    INTC_register_interrupt(&tc_irq, EXAMPLE_TC_IRQ, EXAMPLE_TC_IRQ_PRIORITY);
    // Enable the interrupts
    Enable_global_interrupt();
    cpu_irq_enable();
    // Initialize the timer module
    tc_init(tc);

    et024006_Init( FOSC0, FOSC0 );

    /* PWM is fed by PBA bus clock which is by default the same
    * as the CPU speed. We set a 0 duty cycle and thus keep the
    * display black*/
    tft_bl_init();

    // Clear the display i.e. make it red
    et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, 0x52EB);
    background();
    newMask(RED);

    selector = -1;
    /////////////////////////////////////////////////////////////////////////////////////////
    //****************** LOOP  *************************************************************
    while (1)
    {
		intFlag();
        switch (selector)
        {
        case RELOJ:
            reloj();
            break;
        case CHRONO:
            chronometer();
            break;
        case APPS:
            apps();
        default:
            break;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    //******************* END LOOP *********************************************************
}

void intFlag(void){
    if (int_flag)
        {
            current_up_state    =   gpio_get_pin_value(UP);
            current_down_state  =   gpio_get_pin_value(DOWN);
            current_right_state =   gpio_get_pin_value(RIGHT);
            current_left_state  =   gpio_get_pin_value(LEFT);
            current_enter_state =   gpio_get_pin_value(ENTER);

            if (current_up_state != 0){         //BOTON ARRIBA
                fu = 1;
                delay_ms(300);

            }
            if (current_down_state != 0){       //BOTON ABAJO
                delay_ms(300);
                fd = 1;

            }
            if (current_right_state != 0){      //BOTON DERECHO
                if(selector < 2){
                    selector++;
                }
                delay_ms(300);
                
            }
            if (current_left_state != 0){       //BOTON IZQUIERDO
                if(selector > 0){
                    selector--;
                }
                delay_ms(300);

            }
            if (current_enter_state != 0){      //BOTON ENTER
                delay_ms(300);
                
            }
            int_flag = 0;
        }
}

void reloj(void){
	
}

void chronometer(void){
    //Boton start
    if(fu && start == 0 && reset == 1){
        start = 1;
    }
    else if(fu && start == 1){
        start = 0;
    }

    if(start && second){
        clockCounter();
    }
    //Boton stop
    if(fd){
        resetClockCounter();
    }
    //restart
}

void apps(void){
	
}

void setAlarm(int hour, int minutes, int seconds){
    alarm_clock_seconds = seconds;
    alarm_clock_minutes = minutes;
    alarm_clock_hours = hour;
}

int checkAlarm(int hour, int minutes, int seconds){
    if((alarm_clock_seconds == seconds) && (alarm_clock_minutes == minutes) && (alarm_clock_hours == hour)){
        return 1;
    }
    return 0;
}

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

void DrawImage(
et024006_color_t const * pixmap,
uint16_t map_width,
uint16_t x,
uint16_t y,
uint16_t width,
uint16_t height)
{
	for(uint16_t i = 0; i< height; i++)
	{
		for(uint16_t j = 0; j< width; j++)
		{	
			if(*pixmap!=0x0){
				et024006_DrawPixel( x+j, y+i, *pixmap++ );
			}
			else{
				 pixmap++;
			}
		}
	}
}

void clockCounter(void){
	// Increment the ms seconds counter
	seconds_tick++;
	if (seconds_tick == 60)
	{
		minutes_tick++;
		seconds_tick = 0;
	}
	if (minutes_tick == 60)
	{
		hours_tick++;
		seconds_tick = 0;
		minutes_tick = 0;
	}
	if (hours_tick == 24)
	{
		hours_tick = 0;
		seconds_tick = 0;
		minutes_tick = 0;
	}
}

void resetClockCounter(void){
    seconds_tick = 0;
    minutes_tick = 0;
    hours_tick = 0;
}