#include "board.h"
#include "pwm.h"
#include "gpio.h"
#include "spi.h"
#include "pdca.h"
#include "usart.h"
#include "et024006dhu.h"
#include "conf_sd_mmc_spi.h"
#include "sd_mmc_spi.h"
#include "power_clocks_lib.h"
#include "delay.h"
#include "pm.h"
#include "string.h"
#include "intc.h"
#include "flashc.h"

/****Se defienen los Macros*****/
#define FREQ        12000000
#define STARTUP     3

#define RIGHT       56
#define LEFT        57
#define ENTER       58

#define IRQ70       70
#define IRQ71       71
#define RISING      1

#define BUFFERSIZE                      64
#define AVR32_PDCA_CHANNEL_USED_RX      AVR32_PDCA_PID_SPI0_RX
#define AVR32_PDCA_CHANNEL_USED_TX      AVR32_PDCA_PID_SPI0_TX
#define AVR32_PDCA_CHANNEL_SPI_RX       0                //PDCA canal 0
#define AVR32_PDCA_CHANNEL_SPI_TX       1                //PDCA canal 1
#define EXAMPLE_USART                   (&AVR32_USART0)
#define AVR32_PDCA_USART_CHANNEL        2
#define EXAMPLE_USART_RX_PIN          AVR32_USART0_RXD_0_0_PIN
#define EXAMPLE_USART_RX_FUNCTION     AVR32_USART0_RXD_0_0_FUNCTION
#define EXAMPLE_USART_TX_PIN          AVR32_USART0_TXD_0_0_PIN
#define EXAMPLE_USART_TX_FUNCTION     AVR32_USART0_TXD_0_0_FUNCTION

/****Se definen las variables a usar****/
uint8_t flag_enter  = 0;
uint8_t flag_right  = 0;
uint8_t flag_left   = 0;
volatile char password[7];
volatile uint64_t sd_card_capacity;
uint32_t sd_mmc_sector = 20;
uint8_t save_status = 0;

const char dummy_data[] =   //Variable dummy para sincronizar datos
#include "dummy.h"
;
volatile avr32_pdca_channel_t* pdca_channelrx;
volatile avr32_pdca_channel_t* pdca_channeltx;
volatile bool end_of_transfer;

/***********Funciones********************/
char* convertIntegerToChar(int N)
{
	
	// Count digits in number N
	int m = N;
	int digit = 0;
	while (m) {
		
		// Increment number of digits
		digit++;
		m /= 10;
	}
	
	// Declare char array for result
	char* arr;
	// Declare duplicate char array
	char arr1[digit];
	// Memory allocation of array
	arr = (char*)malloc(digit);
	
	// Separating integer into digits and
	// accommodate it to character array
	int index = 0;
	while (N) {
		
		// Separate last digit from
		// the number and add ASCII
		// value of character '0' is 48
		arr1[++index] = N % 10 + '0';
		
		// Truncate the last
		// digit from the number
		N /= 10;
	}
	
	// Reverse the array for result
	int i;
	for (i = 0; i < index; i++) {
		arr[i] = arr1[index - i];
	}
	
	// Char array truncate by null
	arr[i] = '\0';
	
	// Return char array
	return (char*)arr;
}
void wait()
{
	volatile int i;
	for(i = 0 ; i < 5000; i++);
}

/****Funcion ISR para Teclas************/

__attribute__((__interrupt__))
static void spi_isr(void)
{
    Disable_global_interrupt();
    pdca_disable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);
    sd_mmc_spi_read_close_PDCA();
    wait();
    pdca_disable(AVR32_PDCA_CHANNEL_SPI_TX);
    pdca_disable(AVR32_PDCA_CHANNEL_SPI_RX);
    Enable_global_interrupt();
}

__attribute__((__interrupt__))
void buttons_isr(void)
{
    if (gpio_get_pin_interrupt_flag(LEFT)){
		pdca_enable(AVR32_PDCA_USART_CHANNEL);
		et024006_DrawFilledRect(0,0,320,240, BLACK);
		et024006_PrintString("Waiting for password", (const unsigned char *)&FONT6x8,80,50,GREEN,-1);
		et024006_PrintString("******", (const unsigned char *)&FONT6x8,120,70,GREEN,-1);
		flag_left = 1;
        gpio_clear_pin_interrupt_flag(LEFT);
    }

    if (gpio_get_pin_interrupt_flag(RIGHT)){
        flag_right = 1;
        gpio_clear_pin_interrupt_flag(RIGHT);
    }

    if (gpio_get_pin_interrupt_flag(ENTER)){
        flag_enter = 1;
        gpio_clear_pin_interrupt_flag(ENTER);
    }
}

__attribute__((__interrupt__))
static void usart_isr(void)
{
    Disable_global_interrupt();
	usart_write_line(EXAMPLE_USART, "Transfer completed!!!!!!\n");
	et024006_DrawFilledRect(0,100,320,20, BLACK);
	et024006_PrintString(password, (const unsigned char *)&FONT6x8,80,120,GREEN,-1);
	pdca_disable_interrupt_transfer_complete(AVR32_PDCA_USART_CHANNEL);
	pdca_disable(AVR32_PDCA_USART_CHANNEL);
	Enable_global_interrupt();
}

/****Funcion ISR para fin de transferencia USART************/


/****Funcion para inicializar SD/MMC************/
static void sd_mmc_resources_init(void){
    
    //GPIO para comunicación con la SD/MMC
    static const gpio_map_t SD_MMC_SPI_GPIO_MAP =
    {
        {SD_MMC_SPI_SCK_PIN, SD_MMC_SPI_SCK_FUNCTION},      //Reloj para SPI
        {SD_MMC_SPI_MISO_PIN, SD_MMC_SPI_MISO_FUNCTION},    //MISO
        {SD_MMC_SPI_MOSI_PIN, SD_MMC_SPI_MOSI_FUNCTION},    //MOSI
        {SD_MMC_SPI_NPCS_PIN, SD_MMC_SPI_NPCS_FUNCTION}    //Chip select nPCS
    };
    
    //Configuración de SPI
    spi_options_t spiOptions =
    {
        .reg            = SD_MMC_SPI_NPCS,
        .baudrate       = SD_MMC_SPI_MASTER_SPEED,
        .bits           = SD_MMC_SPI_BITS,
        .spck_delay     = 0,
        .trans_delay    = 0,
        .stay_act       = 1,
        .spi_mode       = 0,
        .modfdis        = 1
    };

    //Habilitar pines de entrada/salida SPI
    gpio_enable_module(SD_MMC_SPI_GPIO_MAP, sizeof(SD_MMC_SPI_GPIO_MAP) / sizeof(SD_MMC_SPI_GPIO_MAP[0]));

    //Inicializar maestro
    spi_initMaster(SD_MMC_SPI, &spiOptions);

    spi_selectionMode(SD_MMC_SPI, 0, 0, 0);

    spi_enable(SD_MMC_SPI);

    sd_mmc_spi_init(spiOptions, FREQ);
}

/****Funcion para inicializar PDCA************/
void local_pdca_init(void){
    //Canal PDCA para recepcion de datos por SPI
    pdca_channel_options_t pdca_options_SPI_RX ={
        .addr               = password,                  //Tamaño de string a transmitir
        .size               = sizeof(password) / sizeof(password[0]),
        .r_addr             = NULL,                         //Siguiente memoria a cargar
        .r_size             = 0,
        .pid                = AVR32_PDCA_CHANNEL_USED_RX,   //ID del periferico (SPI1 RX)
        .transfer_size      = PDCA_TRANSFER_SIZE_BYTE       //Tamaño de la transferencia
    };

    pdca_channel_options_t pdca_options_SPI_TX ={
        .addr               = (void *)&dummy_data,          //Tamaño de string a transmitir
        .size               = 512,
        .r_addr             = NULL,                         //Siguiente memoria a cargar
        .r_size             = 0,
        .pid                = AVR32_PDCA_CHANNEL_USED_TX,   //ID del periferico (SPI1 TX)
        .transfer_size      = PDCA_TRANSFER_SIZE_BYTE       //Tamaño de la transferencia
    };

    pdca_init_channel(AVR32_PDCA_CHANNEL_SPI_TX, &pdca_options_SPI_TX);
    pdca_init_channel(AVR32_PDCA_CHANNEL_SPI_RX, &pdca_options_SPI_RX);
}

void usart_communication_init(void){
    const gpio_map_t usart_gpio_map = {
        {EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
        {EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION},
    };

    const usart_options_t usart_options = {
        .baudrate = 57600,
        .charlength = 8,
        .paritytype = USART_NO_PARITY,
        .stopbits = USART_1_STOPBIT,
        .channelmode = USART_NORMAL_CHMODE,
    };

    const pdca_channel_options_t PDCA_OPTIONS = {
        .pid            = AVR32_PDCA_PID_USART0_RX,
        .transfer_size  = PDCA_TRANSFER_SIZE_BYTE,
        .addr           = (void *)password,
        .size           = sizeof(password) / sizeof(password[0]),
        .r_addr         = NULL,
        .r_size          = 0
    };

    gpio_enable_module(usart_gpio_map, sizeof(usart_gpio_map) / sizeof(usart_gpio_map[0]));
    usart_init_rs232(EXAMPLE_USART, &usart_options, FREQ);
    pdca_init_channel(AVR32_PDCA_USART_CHANNEL, &PDCA_OPTIONS);
}

/****Declaraciones para pantalla LCD************/
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

/****Funciones para interfaz de usuario************/

void greetingSequence(){
    et024006_DrawFilledRect(0,0,ET024006_WIDTH,ET024006_HEIGHT, BLACK);                     //Limpiar la pantalla y ponerla en negro
    et024006_PrintString("Welcome!!!!!", (const unsigned char *)&FONT6x8,80,10,GREEN,-1);   //Mensaje de bienvenida

    if (!sd_mmc_spi_mem_check()){
        et024006_PrintString("Please insert SD/MMC to continue", (const unsigned char *)&FONT6x8,80,30,GREEN,-1);
        while (!sd_mmc_spi_mem_check());                                                      //Esperar a que se inserte la tarjeta        
    } else {
        /*et024006_DrawFilledRect(80,30,10,20, BLACK);*/ 
        et024006_PrintString("SD/MMC detected, ready to go!!!!!", (const unsigned char *)&FONT6x8,80,30,GREEN,-1);
    }
}

uint8_t saveSequence(uint8_t sector_number){
	et024006_DrawFilledRect(0,0,ET024006_WIDTH,ET024006_HEIGHT, BLACK);
	if (!sd_mmc_spi_mem_check()){
		et024006_PrintString("Please insert SD/MMC to continue", (const unsigned char *)&FONT6x8,80,30,GREEN,-1);
		while (!sd_mmc_spi_mem_check());                                                      //Esperar a que se inserte la tarjeta
		}
	if (sector_number >= 30){
		et024006_DrawFilledRect(0,0,320,240, BLACK);
		et024006_PrintString("Error, maximum number of password", (const unsigned char *)&FONT6x8,80,30,GREEN,-1);
		return 0;
	}
	if (sd_mmc_spi_mem_check() && strlen(password) == 0){
		et024006_PrintString("Error, no password given", (const unsigned char *)&FONT6x8,80,30,GREEN,-1);
		return 0;
		} else {
				et024006_DrawFilledRect(0,0,320,240, BLACK);
				et024006_PrintString("Saving the password", (const unsigned char *)&FONT6x8,80,30,GREEN,-1);
				et024006_PrintString("Writing in sector: ", (const unsigned char *)&FONT6x8,80,60,GREEN,-1);
				et024006_PrintString(convertIntegerToChar(sector_number), (const unsigned char *)&FONT6x8,200,60,GREEN,-1);
				et024006_PrintString("Capacity: ", (const unsigned char *)&FONT6x8,80,90,GREEN,-1);
				sd_mmc_spi_get_capacity();
				et024006_PrintString(convertIntegerToChar(capacity >> 20), (const unsigned char *)&FONT6x8,180,90,GREEN,-1);
		}
	return 1;
}

int main(void){
    pcl_switch_to_osc(PCL_OSC0, FREQ, STARTUP);
    et024006_Init( FOSC0, FOSC0 );
	tft_bl_init();

    local_pdca_init();                          //Configuración PDCA para SD/MMC
    usart_communication_init();                  //Configuración PDCA para comunicación USART

    sd_mmc_resources_init();
    greetingSequence();

    // Configuración de interrupciones
    Disable_global_interrupt();
    INTC_init_interrupts();
    INTC_register_interrupt(&buttons_isr, IRQ70, AVR32_INTC_INT3);
    INTC_register_interrupt(&buttons_isr, IRQ71, AVR32_INTC_INT3);              //Interrupción teclas 
    INTC_register_interrupt(&spi_isr, AVR32_PDCA_IRQ_0, AVR32_INTC_INT1);       //Interrupción por transferencia SD/MMC
    INTC_register_interrupt(&usart_isr, AVR32_PDCA_IRQ_2, AVR32_INTC_INT3);     //Interrupción por transferencia USART
	pdca_enable_interrupt_transfer_complete(AVR32_PDCA_USART_CHANNEL);
	gpio_enable_gpio_pin(LEFT);
	gpio_enable_gpio_pin(RIGHT);
	gpio_enable_gpio_pin(ENTER);
    gpio_enable_pin_interrupt(RIGHT, RISING);
    gpio_enable_pin_interrupt(LEFT, RISING);
    gpio_enable_pin_interrupt(ENTER, RISING);
    Enable_global_interrupt();                  // Habilita interrupciones globales
    
    
    et024006_DrawFilledRect(0,30,320,20, BLACK);
    et024006_PrintString("Please enter a new password", (const unsigned char *)&FONT6x8,50,30,GREEN,-1);
	et024006_PrintString("to be stored in the SD/MMC", (const unsigned char *)&FONT6x8,50,45,GREEN,-1);    
	
    while(1){
        if (flag_left){ //Recibir clave via USART con DMA
			pdca_enable(AVR32_PDCA_USART_CHANNEL);
			pdca_load_channel(AVR32_PDCA_USART_CHANNEL, (void *)password, sizeof(password) / sizeof(password[0]));
			pdca_enable_interrupt_transfer_complete(AVR32_PDCA_USART_CHANNEL);
			
			flag_left = 0;
        }
        if (flag_right){  //Transmitir clave a SDCARD por SPI
			if(!saveSequence(sd_mmc_sector)){
				flag_right = 0;
				continue;
			}
			sd_mmc_spi_write_open(sd_mmc_sector);
			sd_mmc_spi_write_sector_from_ram(password);
			sd_mmc_spi_write_close();
			memset(password, 0, sizeof(password)/ sizeof(password[0]));				//Limpiar el array de caracteres de password para nueva entrada
			sd_mmc_sector++;														//Incrementar el sector de memoria para guardar en el siguiente sector la proxima iteracion
			
			flag_right = 0;
        }
        if (flag_enter){
            //Limpiar display para mostrar las claves
			et024006_DrawFilledRect(0,0,320,240, BLACK);
			for (uint16_t i = 20; i < 30; i++){
				pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_RX, password, sizeof(password)/ sizeof(password[0]));

				pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_TX, (void *)&dummy_data, 512); //send dummy to activate the clock
				
				sd_mmc_spi_read_open_PDCA(i);
				
				spi_write(SD_MMC_SPI,0xFF); // Write a first dummy data to synchronize transfer
				pdca_enable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);
				pdca_channelrx =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_RX); // get the correct PDCA channel pointer
				pdca_channeltx =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_TX); // get the correct PDCA channel pointer
				pdca_channelrx->cr = AVR32_PDCA_TEN_MASK; // Enable RX PDCA transfer first
				pdca_channeltx->cr = AVR32_PDCA_TEN_MASK; // and TX PDCA transfer
				et024006_PrintString("Password: ", (const unsigned char *)&FONT6x8, 50, (i - 20)*10,GREEN,-1);
				et024006_PrintString(password, (const unsigned char *)&FONT6x8, 110, (i - 20)*10,GREEN,-1);
				memset(password, 0, sizeof(password)/ sizeof(password[0]));
			}
			flag_enter = 0;
        }
// 		usart_write_line(EXAMPLE_USART, password);
// 		usart_write_line(EXAMPLE_USART, "\n");
    }
}