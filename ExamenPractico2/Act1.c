#include "board.h"
#include "gpio.h"
#include "pm.h"
#include "delay.h"
#include "intc.h"
#include "flashc.h"

#define FREQ 12000000
#define STARTUP 1
#define UP      54
#define DOWN    55
#define RIGHT   56
#define LEFT    57
#define ENTER   58
#define LED0    59
#define LED1    60
#define LED2    5
#define LED3    6

#define IRQ70   70
#define IRQ71   71

#define RISING  1

uint8_t fu=0;
uint8_t fd=0;
uint8_t fe=0;
uint8_t fr=0;
uint8_t fl=0;

uint8_t number = 0b0110;
uint8_t first_bit = 0b0000;
uint8_t second_bit = 0b0000;
uint8_t third_bit = 0b0000;
uint8_t fourth_bit = 0b0000;
uint8_t binary_number1 = 1;
uint8_t binary_number2 = 2;


void bin2LED(uint8_t binary_number);
void outShift(uint8_t *num);
void inShift(uint8_t *num);

__attribute__ ((__interrupt__))

void isr(void)
{

  if(gpio_get_pin_interrupt_flag(UP)){
    fu=1;

    gpio_clear_pin_interrupt_flag(UP);
  }
  if(gpio_get_pin_interrupt_flag(DOWN)){
    fd=1;
	gpio_clr_gpio_pin(LED0);
    gpio_clear_pin_interrupt_flag(DOWN);
  }
  if(gpio_get_pin_interrupt_flag(RIGHT)){
    fr=1;
    gpio_clear_pin_interrupt_flag(RIGHT);
  }
  if(gpio_get_pin_interrupt_flag(LEFT)){
    fl=1;
    gpio_clear_pin_interrupt_flag(LEFT);
  }
  if(gpio_get_pin_interrupt_flag(ENTER)){
    fe=1;
    gpio_clear_pin_interrupt_flag(ENTER);
  }

}


int main(void)
{
    // Configuración de PM
    pm_switch_to_osc0(&AVR32_PM, FREQ, STARTUP);

    // Configuración de interrupciones
    Disable_global_interrupt();
    INTC_init_interrupts();
    INTC_register_interrupt(&isr, IRQ70, 3);
    INTC_register_interrupt(&isr, IRQ71, 3);

    gpio_enable_pin_interrupt(UP , RISING);
    gpio_enable_pin_interrupt(DOWN , RISING);
    gpio_enable_pin_interrupt(RIGHT , RISING);
    gpio_enable_pin_interrupt(LEFT , RISING);
    gpio_enable_pin_interrupt(ENTER, RISING);

    Enable_global_interrupt();  //Habilita interrupciones globales

    while(1){

      if(fu){
        bin2LED(number);
        if(number == 6){
          outShift(&number);
        }
        else if(number == 9){
          inShift(&number);
        }

      }
      if(fe){
        number=0;
        bin2LED(number);
        fe=0;
        fu=0;
      }
	  delay_ms(500);

    }

  return 0;
}

void outShift(uint8_t *num){
  binary_number1 = binary_number1 << 1;
  binary_number2 = binary_number2 >> 1;
  *num = binary_number1 <<2 | binary_number2;
}

void inShift(uint8_t *num){
  binary_number1 = binary_number1 >> 1;
  binary_number2 = binary_number2 << 1;
  *num = binary_number1 <<2 | binary_number2;
}

void bin2LED(uint8_t binary_number){
	first_bit = binary_number & 0b0001;
	second_bit = binary_number & 0b0010;
	third_bit = binary_number & 0b0100;
  fourth_bit = binary_number & 0b1000;

  if (first_bit == 0b0001){
		gpio_clr_gpio_pin(LED3);
		}else {
		gpio_set_gpio_pin(LED3);
	}

  if (second_bit == 0b0010){
		gpio_clr_gpio_pin(LED2);
		}else {
		gpio_set_gpio_pin(LED2);
	}

	if (third_bit == 0b0100){
		gpio_clr_gpio_pin(LED1);
		}else {
		gpio_set_gpio_pin(LED1);
	}

  if (fourth_bit == 0b1000){
		gpio_clr_gpio_pin(LED0);
		}else {
		gpio_set_gpio_pin(LED0);
	}

}
