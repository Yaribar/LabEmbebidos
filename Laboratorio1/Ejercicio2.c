/* Laboratorio 1 - Actividad2 
• Los LED0, LED1 y LED2, llevaran la cuenta en binario de un registro de 3 bits
• El registro se incrementa cada vez que se presiona la tecla (up)
• El registro se decrementa cada vez que se presiona la tecla (down)
• El valor no debe de sobrepasar los limites inferiores ni superiores
• El LED3 mostrara el último estado presionado (up or down)
*/
#include <asf.h>
#include "conf_gpio_peripheral_bus_example.h"

/*#define LED0	LED0_GPIO
#define LED1	LED1_GPIO
#define LED2	LED2_GPIO
#define LED3	LED3_GPIO
#define UP		QT1081_TOUCH_SENSOR_UP
#define DOWN	QT1081_TOUCH_SENSOR_DOWN*/

uint32_t state = 0;
uint32_t i, j;
uint8_t current_up_state, prev_up_state, current_down_state, prev_down_state;
uint8_t number = 0b000;
uint8_t third_bit = 0b000;
uint8_t second_bit = 0b000;
uint8_t first_bit = 0b000;

void bin2LED(uint8_t binary_number){
	first_bit = binary_number & 0b001;
	second_bit = binary_number & 0b010;
	third_bit = binary_number & 0b100;
	
	if (third_bit == 0b100){
		gpio_clr_gpio_pin(LED0);
		}else {
		gpio_set_gpio_pin(LED0);
	}
	
	if (second_bit == 0b010){
		gpio_clr_gpio_pin(LED1);
		}else {
		gpio_set_gpio_pin(LED1);
	}
	
	if (first_bit == 0b001){
		gpio_clr_gpio_pin(LED2);
		}else {
		gpio_set_gpio_pin(LED2);
	}
}

/** \brief Main function to alter the state of the pin \c GPIO_PIN_EXAMPLE.
 */
int main(void)
{

	while (1) {
		
		current_up_state = gpio_get_pin_value(UP);
		current_down_state = gpio_get_pin_value(DOWN);
		
		if (current_up_state != prev_up_state){
			for (int i = 0; i < 1000; i++){
				current_up_state = gpio_get_pin_value(UP);
			}
			
			if (number == 0b111){
				number = 0b111;
			} else {
				number++;
			}
			prev_up_state = current_up_state;
			gpio_clr_gpio_pin(LED3);
		}
		
		if (current_down_state != prev_down_state){
			for (int j = 0; j < 1000; j++){
				current_down_state = gpio_get_pin_value(DOWN);
			}
			
			if (number == 0b000){
				number = 0b000;
				} else {
				number--;
			}
			prev_down_state = current_down_state;
			gpio_set_gpio_pin(LED3);
		}
		
		bin2LED(number);
	}
}
