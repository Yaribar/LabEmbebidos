// ****************************Librerías***********************
#include <avr32/io.h>
#include "pm.h"                 // Todos
#include "flashc.h"             // Todos
#include "gpio.h"               // GPIO
#include "intc.h"               // INTC
#include "delay.h"              // TODOS
#include "eic.h"                // EIC
#include "tc.h"                 // TIMERS
#include "spi.h"                // SPI
#include "usart.h"              // USART
#include "pdca.h"               // PDCA
#include "conf_sd_mmc_spi.h"    // SD/MMC
#include "sd_mmc_spi.h"         // SD/MMC
#include "delay.h"              // TODOS
#include "string.h"             // Trae méodos para String

// ***************************Macros***************************

//PM
#define FREQ_OSC0       12000000    // Frecuencia del oscilador
#define STARTUP         0           // Delay de arranque (Ver tabla 2)
#define MUL             10          // Factor de multiplicación (MUL) para PLL (Ver Sección PLL)
#define DIV             1           // Factor de división (DIV) para PLL (Ver Sección PLL)
#define OSC4PLL         1           // Oscilador a usar como fuente para PLL: 0 (OSC0), 1 (OSC1)
#define PLL             0           // PLL a habilitar: 0 (PLL0), 1 (PLL1)
#define PLL_RANGE       1           // Rango de 80MHz-180MHz

//GPIO
#define LED0	59
#define LED2	5
#define UP		54
#define RIGHT   56
#define ENTER   58

//INTC
#define IRQ70   70      // 54 (UP) y 55 (DOWN)
#define IRQ71   71      // 56 (RIGHT), 57 (LEFT) y 58 (ENTER)
#define A       0       // TECLAS
#define B       1
#define C       2

// SPI
#define SPI0_SCK             13
#define SPI0_MISO            11
#define SPI0_MOSI            12
#define SPI0_NPCS_0          10
#define SPI0_NPCS_1          8
#define SPI0_NPCS_2          9
               

#define CHIP_SELECT     2           // MACROS para estructura de SPI
#define BAUDRATE        150000
#define TRAMA           8
#define DLBYS           0
#define DLBYCT          0
#define CSAAT           0
#define SPI_MODE        3
#define FAULT           1

// USART
#define EXAMPLE_USART                 (&AVR32_USART1)                   //USARTX
#define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA

// PDCA 

#define BUFFERSIZE                      64
#define AVR32_PDCA_CHANNEL_USED_RX      AVR32_PDCA_PID_SPI0_RX
#define AVR32_PDCA_CHANNEL_USED_TX      AVR32_PDCA_PID_SPI0_TX
#define AVR32_PDCA_CHANNEL_SPI_RX       1                           // PDCA canal 0
#define AVR32_PDCA_CHANNEL_SPI_TX       0                           // PDCA canal 1

const char dummy_data[] =   //Variable dummy para sincronizar datos
#include "dummy.h"
;

volatile avr32_pdca_channel_t* pdca_channelrx;
volatile avr32_pdca_channel_t* pdca_channeltx;

volatile bool end_of_transfer;

volatile char ram_buffer[1000];

//*******************  Declaración de variables *******************

struct EEG
{
   char setup[3];   //•	Identificador (ID)
                    //•	Longitud de la trama
                    //•	Frecuencia de muestreo

   char datos[100]; //Trama
}EGG;

//------------------  Variables para INTC -------------------
uint8_t int_flag;
uint8_t flag_up = 0;
uint8_t flag_enter = 0;
uint8_t flag_right = 0;


__attribute__((__interrupt__))
static void spi_isr(void) //SPI SD
{
    Disable_global_interrupt();
    pdca_disable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);
    sd_mmc_spi_read_close_PDCA();
    wait();
    pdca_disable(AVR32_PDCA_CHANNEL_SPI_TX);
    pdca_disable(AVR32_PDCA_CHANNEL_SPI_RX);
    Enable_global_interrupt();

    end_of_transfer = true;
}


/*! \brief Initializes SD/MMC resources: GPIO, SPI and SD/MMC.
 */
static void sd_mmc_resources_init(void)
{
  // GPIO pins used for SD/MMC interface
  static const gpio_map_t SD_MMC_SPI_GPIO_MAP =
  {
    {SD_MMC_SPI_SCK_PIN,  SD_MMC_SPI_SCK_FUNCTION },  // SPI Clock.
    {SD_MMC_SPI_MISO_PIN, SD_MMC_SPI_MISO_FUNCTION},  // MISO.
    {SD_MMC_SPI_MOSI_PIN, SD_MMC_SPI_MOSI_FUNCTION},  // MOSI.
    {SD_MMC_SPI_NPCS_PIN, SD_MMC_SPI_NPCS_FUNCTION}   // Chip Select NPCS.
    
  };

  // SPI options.
  spi_options_t spiOptions =
  {
    .reg          = SD_MMC_SPI_NPCS,
    .baudrate     = SD_MMC_SPI_MASTER_SPEED,  // Defined in conf_sd_mmc_spi.h.
    .bits         = SD_MMC_SPI_BITS,          // Defined in conf_sd_mmc_spi.h.
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 1,
    .spi_mode     = 0,
    .modfdis      = 1
  };

  // Assign I/Os to SPI.
  gpio_enable_module(SD_MMC_SPI_GPIO_MAP,
                     sizeof(SD_MMC_SPI_GPIO_MAP) / sizeof(SD_MMC_SPI_GPIO_MAP[0]));

  // Initialize as master.
  spi_initMaster(SD_MMC_SPI, &spiOptions);

  // Set SPI selection mode: variable_ps, pcs_decode, delay.
  spi_selectionMode(SD_MMC_SPI, 0, 0, 0);

  // Enable SPI module.
  spi_enable(SD_MMC_SPI);

  // Initialize SD/MMC driver with SPI clock (PBA).
  sd_mmc_spi_init(spiOptions, PBA_HZ);
}


static void dispositivo_medico_resources_init(void)
{
  // Configuracion SPI
    static const gpio_map_t SPI_GPIO_MAP =
        {
           {SPI0_NPCS_2,FUNCTION}                            // Chip Select para dispositivo medico
        };
        
    // SPI options: SI NO TE DA UN DELAY PONES CERO
    spi_options_t spiOptions =
        {
            .reg = CHIP_SELECT,         // [0, 1, 2, 3]   
            .baudrate = BAUDRATE,       // Directo te lo da el problema
            .bits = TRAMA,              // Número de bits a transmitir/recibir (8 a 16 bits)
            .spck_delay = DLBYS,        // Despejar fórmula #
            .trans_delay = DLBYCT,      // Despejar formula #
            .stay_act = CSAAT,          // Deselección de periféricos
            .spi_mode = CPOL Y NCPHA,   // Modo [0, 1, 2, 3]
            .modfdis = FAULT};          // 1 Inhabilitado y 0 Habilitado

    gpio_enable_module(SPI_GPIO_MAP, sizeof(SPI_GPIO_MAP) / sizeof(SPI_GPIO_MAP[0])); // Habilitamos GPIO
  
    spi_initMaster(&AVR32_SPI, &spiOptions); // Tantas estructuras tantos chip selects (periféricos) tengas

    spi_selectionMode(&AVR32_SPI, 0, 0, DLYBCS); // Set SPI selection mode: variable_ps, pcs_decode, delay.

    // Enable SPI module.
    spi_enable(&AVR32_SPI);

    spi_setupChipReg(&AVR32_SPI, &spiOptions, PBA_HZ); // Obtenemos la frecuencia del PBA con la fórmula de Baudrate

}

/****Funcion para inicializar PDCA************/
void local_pdca_init(void){
    //Canal PDCA para recepcion de datos por SPI
    pdca_channel_options_t pdca_options_SPI_RX ={
        .addr               = EEG.datos,                  //Tamaño de string a transmitir
        .size               = sizeof(EEG.datos) / sizeof(EEG.datos[0]),
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



__attribute__ ((__interrupt__))
void INTC_HANDLER(void)
{
     if (gpio_get_pin_interrupt_flag(UP)){
		flag_up = 1;
        gpio_clear_pin_interrupt_flag(UP);
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

int main()
{   
    // Configuración de PM
    pm_switch_to_osc0(&AVR32_PM, FREQ, STARTUP) //Activa y cambia el oscilador al 0
    local_pdca_init();                          //Configuración PDCA 
    // Configuración de GPIO

    // ******************** USART BEGIN *****************

    	static const gpio_map_t USART_GPIO_MAP =
	{
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	};

	// USART options.
	static const usart_options_t USART_OPTIONS =
	{
		.baudrate     = 57600,                  //BAUDRATE
		.charlength   = 8,                      //CHAR LENGTH 8 OR 16 
		.paritytype   = USART_EVEN_PARITY,                
		.stopbits     = USART_1_STOPBIT,   
		.channelmode  = USART_NORMAL_CHMODE   //
	};

	// Assign GPIO to USART.
	gpio_enable_module(USART_GPIO_MAP, sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	// Initialize USART in RS232 mode.
	usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, FREQ); //Most used

    // ******************** USART END *******************

    Disable_global_interrupt();
    INTC_init_interrupts();
    INTC_register_interrupt(&spi_isr, AVR32_PDCA_IRQ_0, AVR32_INTC_INT1);       //Interrupción por transferencia 
    INTC_register_interrupt(&INTC_HANDLER, IRQ70, PRIORIDAD);  //Define el handler al cual va a ser llamado cuando suceda una interrupcion
    INTC_register_interrupt(&INTC_HANDLER, IRQ71, PRIORIDAD);  //Define el handler al cual va a ser llamado cuando suceda una interrupcion

    gpio_enable_gpio_pin(UP);
	gpio_enable_gpio_pin(RIGHT);
	gpio_enable_gpio_pin(ENTER);
    gpio_enable_pin_interrupt(RIGHT, RISING);
    gpio_enable_pin_interrupt(UP, RISING);
    gpio_enable_pin_interrupt(ENTER, RISING);

    Enable_global_interrupt();                  // Habilita interrupciones globales

    uint8_t sector = 0;
    uint8_t trama_counter=4;

    while(1){
        if(flag_up){
            if(sector>25){
                sector=5;
            }else if(sector<25){
                sector+=1;
            }
            flag_up = 0;
        }

        if (flag_right && trama_counter<=25){  //Recibir datos por SPI0 CHIP SELECT 2
            spi_selectChip(&AVR32_SPI0, SPI0_NPCS_2);
            spi_read(&AVR32_SPI0,  EEG.setup);

            pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_RX, EEG.datos, sizeof(EEG.setup[1]));
            pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_TX, (void *)&dummy_data, 512); //send dummy to activate the clock

            spi_write(&AVR32_SPI0,0xFF); // Write a first dummy data to synchronize transfer

            pdca_enable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);
            pdca_channelrx =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_RX); // get the correct PDCA channel pointer
            pdca_channeltx =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_TX); // get the correct PDCA channel pointer
            pdca_channelrx->cr = AVR32_PDCA_TEN_MASK; // Enable RX PDCA transfer first
            pdca_channeltx->cr = AVR32_PDCA_TEN_MASK; // and TX PDCA transfer

            while(!end_of_transfer);

            gpio_clr_gpio_pin(LED0); // PRENDE PIN
            trama_counter++;
            
            spi_selectChip(&AVR32_SPI0, SPI0_NPCS_1);

			while (!sd_mmc_spi_mem_check()); 
            sd_mmc_spi_write_open(1);
			sd_mmc_spi_write_sector_from_ram(EEG.setup);
			sd_mmc_spi_write_close();
			sd_mmc_spi_write_open(trama_counter);
			sd_mmc_spi_write_sector_from_ram(EEG.datos);
			sd_mmc_spi_write_close();
			memset(EEG.datos, 0, sizeof(EEG.datos)/ sizeof(EEG.datos[0]));				//Limpiar el array de caracteres de password para nueva entrada
			
			flag_right = 0;
            delay(300);
            gpio_set_gpio_pin(LED0); // PRENDE PIN
        }
        if (flag_enter){
            //Limpiar display para mostrar las claves
            
            spi_selectChip(&AVR32_SPI0, SPI0_NPCS_1);
            pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_RX, EEG.datos, sizeof(EEG.setup[1]));

            pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_TX, (void *)&dummy_data, 512); //send dummy to activate the clock
            
            sd_mmc_spi_read_open_PDCA(sector);
            
            spi_write(SD_MMC_SPI,0xFF); // Write a first dummy data to synchronize transfer
            pdca_enable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);
            pdca_channelrx =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_RX); // get the correct PDCA channel pointer
            pdca_channeltx =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_TX); // get the correct PDCA channel pointer
            pdca_channelrx->cr = AVR32_PDCA_TEN_MASK; // Enable RX PDCA transfer first
            pdca_channeltx->cr = AVR32_PDCA_TEN_MASK; // and TX PDCA transfer

            while(!end_of_transfer);

            if(strlen(EEG.datos) == 0){
                gpio_clr_gpio_pin(LED0); // PRENDE PIN
                delay(300);
            }
            else{
                usart_write_line(EXAMPLE_USART,EEG.datos);
                memset(trama, 0, sizeof(EEG.datos)/ sizeof(EEG.datos[0]));
            }
			flag_enter = 0;
            sector = 5;
        }
    }
}