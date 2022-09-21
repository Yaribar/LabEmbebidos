/*
Laboratorio 1 - Actividad 1
Utilizando las teclas (left), (right), y (enter), introducir cuatro números de
cuatro bits
    • Al presionar la tecla (up), realizar AND lógico
    • Al presionar la tecla (down), realizar OR lógico
    • Las operaciones tiene que realizarse en los cuatro números
    • Los datos de entrada y salida serán representados con los LEDs
*/

#include <asf.h>

#define LED0 LED0_GPIO
#define LED1 LED1_GPIO
#define LED2 LED2_GPIO
#define LED3 LED3_GPIO
#define ENTER QT1081_TOUCH_SENSOR_ENTER
#define LEFT QT1081_TOUCH_SENSOR_LEFT
#define RIGHT QT1081_TOUCH_SENSOR_RIGHT
#define UP QT1081_TOUCH_SENSOR_UP
#define DOWN QT1081_TOUCH_SENSOR_DOWN

uint8_t change = 0, ntimes = 0;                            // Flag to see if something changed and # of times the enter button has been pressed
uint8_t f_left = 0, f_right = 0, f_enter = 0, f_logic = 0; // Flags to see if any button was pressed
uint16_t counter = 0;                                      // Counter to be increased or decreased
uint8_t current_up_state, current_down_state, current_left_state, current_right_state, current_enter_state;
uint8_t prev_up_state, prev_down_state, prev_left_state, prev_right_state, prev_enter_state;

struct num
{
    uint8_t bits[4];
};

struct num number_one, number_two, result; // First number

uint16_t getCounter(uint8_t sum, uint8_t sub, uint16_t number);
void dec2bcd(uint16_t number, uint8_t bites[4]);
struct num getResult(uint8_t bits_one[4], uint8_t bits_two[4], uint8_t operation);

/** \brief Main function to alter the state of the pin \c GPIO_PIN_EXAMPLE.
 */
int main(void)
{
    while (1)
    {
        // We read the values of the sensors (polling)
		current_down_state = gpio_get_pin_value(DOWN);
		current_up_state = gpio_get_pin_value(UP);
		current_right_state = gpio_get_pin_value(RIGHT);
		current_left_state = gpio_get_pin_value(LEFT);
		current_enter_state = gpio_get_pin_value(ENTER);
		
		if (current_up_state != prev_up_state){
			for (int i = 0; i < 1000; i++){
				current_up_state = gpio_get_pin_value(UP);
			}
			f_logic = 1;
		}

		if (current_down_state != prev_down_state){
			for (int i = 0; i < 1000; i++){
				current_down_state = gpio_get_pin_value(DOWN);
			}
			f_logic = 2;
		}
		
		if (current_right_state != prev_right_state){
			for (int i = 0; i < 1000; i++){
				current_right_state = gpio_get_pin_value(RIGHT);
			}
			f_right = 1;
			change = 1;
		}
		
		if (current_left_state != prev_left_state){
			for (int i = 0; i < 1000; i++){
				current_left_state = gpio_get_pin_value(LEFT);
			}
			f_left = 1;
			change =1;
		}

		if (current_enter_state != prev_enter_state){
			for (int i = 0; i < 1000; i++){
				current_enter_state = gpio_get_pin_value(ENTER);
			}
			f_enter = 1;
			ntimes += 1;
		}
        // We increase/decrease the counter
        counter = getCounter(f_right, f_left, counter);
		f_right = 0;
		f_left = 0;

        // We check if an arrow has been pressed
        if (change && ntimes == 0)
        {
            dec2bcd(counter, number_one.bits);
            result = number_one;
        }
		
        else if (change && ntimes == 1)
        {
            dec2bcd(counter, number_two.bits);
            result = number_two;
        }


        // We do the logic operations (in case they are demanded)
        if (ntimes == 2)
        {
			if (f_logic !=0)
			{
				result = getResult(number_one.bits, number_two.bits, f_logic);
				change = 1;
			}
/*
			gpio_clr_gpio_pin(LED0);
			if (number_one.bits[3])
			{
				gpio_clr_gpio_pin(LED3);
			}
*/
           
        }

        // We display the number in the leds

        if (change)
        {
			if (result.bits[0])
			{
				gpio_clr_gpio_pin(LED0);
			} else if (result.bits[0] == 0)
			{
				gpio_set_gpio_pin(LED0);
			}
			if (result.bits[1])
			{
				gpio_clr_gpio_pin(LED1);
			}else if (result.bits[1] == 0)
			{
				gpio_set_gpio_pin(LED1);
			}
			
			if (result.bits[2])
			{
				gpio_clr_gpio_pin(LED2);
			} else if (result.bits[2] == 0)
			{
				gpio_set_gpio_pin(LED2);
			}
			
			if (result.bits[3])
			{
				gpio_clr_gpio_pin(LED3);
			} else if (result.bits[3] == 0)
			{
				gpio_set_gpio_pin(LED3);
			}

			change = 0;
        }

        // Pokayokes
        if (ntimes >= 2)
        {
            ntimes = 2;
        }
		
		if (f_enter && ntimes <3)
		{
			f_enter =0;
			counter = 0;
			for (int i=0; i<4;i++)
			{
				result.bits[i] = 0;
			}
			gpio_set_gpio_pin(LED0);
			gpio_set_gpio_pin(LED1);
			gpio_set_gpio_pin(LED2);
			gpio_set_gpio_pin(LED3);
		}else if (f_enter && ntimes >=3)
		{
			f_enter =0;
			counter = 0;
			for (int i=0; i<4;i++)
			{
				result.bits[i] = 0;
				number_one.bits[i] = 0;
				number_two.bits[i] = 0;
			}
			gpio_set_gpio_pin(LED0);
			gpio_set_gpio_pin(LED1);
			gpio_set_gpio_pin(LED2);
			gpio_set_gpio_pin(LED3);
		}
		
		

    }
}

uint16_t getCounter(uint8_t sum, uint8_t sub, uint16_t number)
{
    if (sum && number < 15)
    {
        number += 1;
    }
    else if (sub && number > 0)
    {
        number -= 1;
    }
	if (number >= 15)
	{
		number = 15;
	}else if (number <=0)
	{
		number =0;
	}
    return number;
}

void dec2bcd(uint16_t number, uint8_t bites[4])
{
    int finish = 1, quotient = 1, remainder, j = 4, k =0;
    while (j != 0)
    {
        quotient = number / 2;
        remainder = number % 2;
		bites[j-1] = remainder;
		if (k)
		{
			bites [j-1] = 0;
		}
		if (quotient == 0)
		{
			k=1;
		}
        j -= 1;
		
        number = quotient;
    }
}

struct num getResult(uint8_t bits_one[4], uint8_t bits_two[4], uint8_t operation)
{
    struct num aux;
    if (operation == 1)
    {
        for (int i = 0; i < 4; i++)
        {
            aux.bits[i] = bits_one[i] & bits_two[i];
        }
		f_logic = 0;
		ntimes = 0;
    }
    else if (operation == 2)
    {
        for (int i = 0; i < 4; i++)
        {
            aux.bits[i] = bits_one[i] | bits_two[i];
        }
		f_logic = 0;
		ntimes = 0;
    }
    return aux;
}
