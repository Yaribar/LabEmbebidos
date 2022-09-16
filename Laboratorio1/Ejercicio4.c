#include <asf.h>
#include "conf_gpio_peripheral_bus_example.h"

/*#define LED0	LED0_GPIO
#define LED1	LED1_GPIO
#define LED2	LED2_GPIO
#define LED3	LED3_GPIO
#define LED4	AVR32_PIN_PB30
#define LED5	AVR32_PIN_PB31
#define UP		QT1081_TOUCH_SENSOR_UP
#define DOWN	QT1081_TOUCH_SENSOR_DOWN*/

uint8_t LEFTSHIFT = 1;
uint8_t enter_state;
uint16_t shiftNumberLeft, shiftNumberRigth;
uint16_t number = 0b000001;
uint16_t sixth_bit = 0b000000;
uint16_t fifth_bit = 0b000000;
uint16_t fourth_bit = 0b000000; 
uint16_t third_bit = 0b000000;
uint16_t second_bit = 0b000000;
uint16_t first_bit = 0b000000;

uint16_t leftShift(uint16_t leftNumber){
	shiftNumberLeft = leftNumber<<1;
	return shiftNumberLeft;
}

uint16_t rightShift(uint16_t rightNumber){
	shiftNumberRigth = rightNumber>>1;
	return shiftNumberRigth;
}

void bin2LED(uint16_t binary_number){
	first_bit = binary_number & 0b000001;
	second_bit = binary_number & 0b000010;
	third_bit = binary_number & 0b000100;
	fourth_bit = binary_number & 0b001000;
	fifth_bit = binary_number & 0b010000;
	sixth_bit = binary_number & 0b100000;
	
	if (sixth_bit == 0b100000){
		gpio_clr_gpio_pin(LED5);
		}else {
		gpio_set_gpio_pin(LED5);
	}
	
	if (fifth_bit == 0b010000){
		gpio_clr_gpio_pin(LED4);
		}else {
		gpio_set_gpio_pin(LED4);
	}
	
	if (fourth_bit == 0b001000){
		gpio_clr_gpio_pin(LED3);
		}else {
		gpio_set_gpio_pin(LED3);
	}
	
	if (third_bit == 0b000100){
		gpio_clr_gpio_pin(LED2);
		}else {
		gpio_set_gpio_pin(LED2);
	}
	
	if (second_bit == 0b000010){
		gpio_clr_gpio_pin(LED1);
		}else {
		gpio_set_gpio_pin(LED1);
	}
	
	if (first_bit == 0b000001){
		gpio_clr_gpio_pin(LED0);
		}else {
		gpio_set_gpio_pin(LED0);
	}
}

/** \brief Main function to alter the state of the pin \c GPIO_PIN_EXAMPLE.
 */
int main(void)
{

	while (1) {
		
		if (number == 1){
			LEFTSHIFT = 1;
		} else if (number == 32){
			LEFTSHIFT = 0;
		}
		
		if (LEFTSHIFT){
			number = leftShift(number);
		} else {
			number = rightShift(number);
		}
		
		bin2LED(number);
		
		for (int i = 0; i < 500; i++){
			enter_state = gpio_get_pin_value(UP);
		}		
	}
}