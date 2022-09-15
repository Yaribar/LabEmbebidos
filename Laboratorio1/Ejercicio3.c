/*
Laboratorio 1- Actividad 3

• Los LED0, LED1 y LED2, llevaran la cuenta en binario de un registro de 3 bits
• El LED3 representa el dato de entrada (1,0)
• La tecla (up)    inserta en el dato de entrada un [1]
• La tecla (down)  inserta en el dato de entrada un [0]
• La tecla (left)  recorre el registro a la izquierda una posición e inserta a la derecha el dato de entrada
• La tecla (right) recorre el registro a la derecha una posición e inserta a la izquierda el dato de entrada
• La tecla (enter) resetea el registro a cero
• Cada vez que se realice un corrimiento, se debe de introducir un nuevo dato
de entrada, de lo contrario se deshabilitan las teclas (left) y (right)
*/

#include <asf.h>

#define LED0		LED0_GPIO
#define LED1		LED1_GPIO
#define LED2		LED2_GPIO
#define LED3		LED3_GPIO
#define S_ENTER		QT1081_TOUCH_SENSOR_ENTER
#define S_LEFT      QT1081_TOUCH_SENSOR_LEFT
#define S_RIGHT     QT1081_TOUCH_SENSOR_RIGHT
#define S_UP        QT1081_TOUCH_SENSOR_UP
#define S_DOWN      QT1081_TOUCH_SENSOR_DOWN

uint8_t counter = 0;
uint8_t input_data = 0;
uint8_t bit0 = 0, bit1 = 0, bit2 = 0;

uint8_t prev_right = 0, current_right = 0, prev_left = 0, current_left =0;
uint8_t input_right = 0;

uint32_t i;
uint32_t j;

/** \brief Main function to alter the state of the pin \c GPIO_PIN_EXAMPLE.
 */

void shiftBits(void);
int main(void)
{

	shiftBits();
	while(1){
		//La tecla (up)    inserta en el dato de entrada un [1]
		if (gpio_get_pin_value(S_UP) == true) {
			input_data = 1;
		}
		//La tecla (down)  inserta en el dato de entrada un [0]
		if(gpio_get_pin_value(S_DOWN) == true){
			input_data = 0;
		}
		//El LED3 representa el dato de entrada (1,0)
		if(input_data == 1){
			gpio_clr_gpio_pin(LED3);
		}else if(input_data == 0){
			gpio_set_gpio_pin(LED3);
		}

		//La tecla (enter) resetea el registro a cero
		if(gpio_get_pin_value(S_ENTER) == true){
			counter = 0;
			shiftBits();
		}
		//Cada vez que se realice un corrimiento, se debe de introducir un nuevo dato
		//de entrada, de lo contrario se deshabilitan las teclas (left) y (right)
		if(input_data == 1){

			//La tecla (left)  recorre el registro a la izquierda una posición e inserta a la derecha el dato de entrada
			current_left = (uint8_t)gpio_get_pin_value(S_LEFT);

			if(prev_left != current_left){
				if(prev_left == 0 && current_left == 1){
					for (i = 0; i < 1000; i ++) {
						current_left = (uint8_t)gpio_get_pin_value(S_LEFT);
						j=i;
					}
					current_left = 0;
					prev_left = 0;
					counter = counter << 1;
					counter = input_data | counter;
					shiftBits();
				}
			}

			//La tecla (right) recorre el registro a la derecha una posición e inserta a la izquierda el dato de entrada
			current_right = (uint8_t)gpio_get_pin_value(S_RIGHT);

			if(prev_right != current_right){
				if(prev_right == 0 && current_right == 1){
					for (i = 0; i < 1000; i ++) {
						current_left = (uint8_t)gpio_get_pin_value(S_RIGHT);
						j=i;
					}
					current_right = prev_right;
					counter = counter >> 1;
					input_right = input_data << 2;
					counter = input_right | counter;
					shiftBits();
				}
			}
		}

		if(bit0 == 1){
			gpio_clr_gpio_pin(LED2);
		}else if(bit0 == 0){
			gpio_set_gpio_pin(LED2);
		}

		if(bit1 == 1){
			gpio_clr_gpio_pin(LED1);
			}else if(bit1 == 0){
			gpio_set_gpio_pin(LED1);
		}

		if(bit2 == 1){
			gpio_clr_gpio_pin(LED0);
			}else if(bit2 == 0){
			gpio_set_gpio_pin(LED0);
		}

	}

}

void shiftBits(void){
	bit0 = counter & 1;
	bit1 = counter & 2;
	bit1 = bit1 >> 1;
	bit2 = counter & 4;
	bit2 = bit2 >> 2;
}
