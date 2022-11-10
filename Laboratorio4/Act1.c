// BATTLESHIP BY ZORRIEMBEBIDOS

// *********************** Libraries **********************
#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "usart.h"
#include "string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ***********************   Macros  **********************

#define EXAMPLE_TARGET_PBACLK_FREQ_HZ FOSC0  // PBA clock target frequency, in Hz

//USART1

#define EXAMPLE_USART                 (&AVR32_USART0)
#define EXAMPLE_USART_RX_PIN          AVR32_USART0_RXD_0_0_PIN
#define EXAMPLE_USART_RX_FUNCTION     AVR32_USART0_RXD_0_0_FUNCTION
#define EXAMPLE_USART_TX_PIN          AVR32_USART0_TXD_0_0_PIN
#define EXAMPLE_USART_TX_FUNCTION     AVR32_USART0_TXD_0_0_FUNCTION
#define EXAMPLE_USART_CLOCK_MASK      AVR32_USART0_CLK_PBA
#define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA

#define RIGHT QT1081_TOUCH_SENSOR_RIGHT

// Manual Modex
#define ERR 1
#define OK 0

// Gameplay
#define limit 11

// Color codes for printing
/* #define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m" */

// ***********************  Variables  ********************

// Manual Mode
char board[11][11] = {0};
char size_board = sizeof(board) / sizeof(board[0]);
char ships[1] = {5};
//char ships[5] = {5, 4, 3, 3, 2};
char size_ships = sizeof(ships) / sizeof(ships[0]);
char ship_orientation;
char rep = 0;
char limit_err = 0, limit_errt = 0;
int x, y;
char err_h = 0, err_v = 0;
char err_ht = 0, err_vt = 0;
char rep_err = 0;

// Auto Mode
// time_t t;
// int Tableboard[11][11];

// Gameplay
int pl_one_agree = 0;
int pl_two_agree = 0;
int num_cells_pone = 0;
int num_cells_ptwo = 0;
int num_cells = 0;

// ******************** Prototype Functions ****************
//Strings
void usart_read_line(char *lineRead, size_t len);
char* convertIntegerToChar(int N);
void parseString(char *string_in, char *delimiter, float output_array[]);

// Manual Mode
int requirements_msg(char n_ship, char playerssBoard[11][11]);
int coordinates(char x, char y, char playerssBoard[11][11]);
int orientation(int n_ship, char playerssBoard[11][11]);
void printBoard(char playerssBoard[11][11]);
void setManual(char playerssBoard[11][11]);

// Gameplay
int getAgree(void);
int hitShip(char backend[11][11], char frontend[11][11], int x, int y, int counter);
int countCells(char backend[11][11]);
int minCells(int a, int b);

// ********************** Structures ***********************

// Auto Mode
struct tableBoard
{
    char board[11][11];
};
struct tableBoard createBoard(void)
{
    struct tableBoard playerBoard;

    for (char i = 0; i < 11; i++)
    {
        for (char j = 0; j < 11; j++)
        {
            if (i == 0 && j == 0)
            {
                playerBoard.board[0][j] = 48;
            }
            else if (i > 0 && j == 0)
            { // row numbers
                playerBoard.board[i][j] = i;
            }
            else if (i == 0 && j > 0)
            { // column numbers
                playerBoard.board[i][j] = j;
            }
            else
            {
                playerBoard.board[i][j] = 48;
            }
        }
    }

    return playerBoard;
};

void printPlayerBoard(char playersBoard[11][11])
{

    for (char i = 0; i < 11; i++)
    {
        for (char j = 0; j < 11; j++)
        {
            if (i + j == 0)
            {
                //printf("   ");
                usart_write_line(EXAMPLE_USART," 0 ");
            }
			else if(playersBoard[i][j] == 48){
				usart_write_line(EXAMPLE_USART," 0 ");
			}
            else if (playersBoard[i][j] < 10)
            {
				usart_write_line(EXAMPLE_USART," ");
				usart_write_line(EXAMPLE_USART,convertIntegerToChar((int)playersBoard[i][j]));
				usart_write_line(EXAMPLE_USART," ");
                
            }
            else
            {
                if ((i > 0 && j == 0) || (i == 0 && j > 0))
                {
                    //printf("%i ", playersBoard[i][j]);
                    usart_write_line(EXAMPLE_USART," ");
					usart_write_line(EXAMPLE_USART,convertIntegerToChar((int)playersBoard[i][j]));
                    usart_write_line(EXAMPLE_USART," ");
                }
            }
        }
        //printf("\n\n");
        usart_write_line(EXAMPLE_USART,"\n\n");
    }
}

void printPlayerBoardwColor(char playersBoard[11][11])
{

    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
			
            if (i + j == 0)
            {
                //printf("   ");
                usart_write_line(EXAMPLE_USART,"  ");
            }
            else if ((i == 0 && j > 0) || (i > 0 && j == 0))
            { // first row and column
                if (playersBoard[i][j] < 10)
                {
                    //printf(" %i ", playersBoard[i][j]);
                    usart_write_line(EXAMPLE_USART," ");
					usart_write_line(EXAMPLE_USART,convertIntegerToChar((int)playersBoard[i][j]));
                    usart_write_line(EXAMPLE_USART," ");
                }
                else
                {
                    //printf("%i ", playersBoard[i][j]);
                    usart_write_line(EXAMPLE_USART," ");
                    usart_write_line(EXAMPLE_USART,convertIntegerToChar((int)playersBoard[i][j]));
                    usart_write_line(EXAMPLE_USART," ");
                }
            }
            else
            {			
				
                if (playersBoard[i][j] == 48)
                {
                    //printf(BLU " * " RESET);
                    usart_write_line(EXAMPLE_USART," * ");
                }
                else if (playersBoard[i][j] == 1)
                {
                    //printf(BLU " * " RESET);
                    usart_write_line(EXAMPLE_USART," * ");
                }
                else if (playersBoard[i][j] == 2)
                {
                    //printf(RED " X " RESET);
                    usart_write_line(EXAMPLE_USART," X ");
                }
                else
                {
                    //printf(YEL " * " RESET);
                    usart_write_line(EXAMPLE_USART," + ");
                }
            }
        }
        //printf("\n\n");
        usart_write_line(EXAMPLE_USART,"\n\n");
    }
}

struct shipPos
{
    char position_ship_two[3];
    char position_ship_three_one[3];
    char position_ship_three_two[3];
    char position_ship_four[3];
    char position_ship_five[3];
};

struct shipPos randomPosition(char upper, char lower)
{
    struct shipPos details;
    char col_rnd_1 = 0, row_rnd_1 = 0, col_rnd_2 = 0, row_rnd_2 = 0,
         col_rnd_3 = 0, row_rnd_3 = 0, col_rnd_4 = 0, row_rnd_4 = 0,
         col_rnd_5 = 0, row_rnd_5 = 0, col_rnd_6 = 0, row_rnd_6 = 0,
         col_rnd_7 = 0, row_rnd_7 = 0, col_rnd_8 = 0, row_rnd_8 = 0,
         col_rnd_9 = 0, row_rnd_9 = 0, col_rnd_10 = 0, row_rnd_10 = 0;

    srand(time(0));
    col_rnd_1 = (rand() % (upper - lower + 1)) + lower;
    row_rnd_1 = (rand() % (upper - lower + 1)) + lower;
    col_rnd_2 = (rand() % (upper - lower + 1)) + lower;
    row_rnd_2 = (rand() % (upper - lower + 1)) + lower;
    col_rnd_3 = (rand() % (upper - lower + 1)) + lower;
    row_rnd_3 = (rand() % (upper - lower + 1)) + lower;
    col_rnd_4 = (rand() % (upper - lower + 1)) + lower;
    row_rnd_4 = (rand() % (upper - lower + 1)) + lower;
    col_rnd_5 = (rand() % (upper - lower + 1)) + lower;
    row_rnd_5 = (rand() % (upper - lower + 1)) + lower;
    col_rnd_6 = (rand() % (upper - lower + 1)) + lower;
    row_rnd_6 = (rand() % (upper - lower + 1)) + lower;
    col_rnd_7 = (rand() % (upper - lower + 1)) + lower;
    row_rnd_7 = (rand() % (upper - lower + 1)) + lower;
    col_rnd_8 = (rand() % (upper - lower + 1)) + lower;
    row_rnd_8 = (rand() % (upper - lower + 1)) + lower;
    col_rnd_9 = (rand() % (upper - lower + 1)) + lower;
    row_rnd_9 = (rand() % (upper - lower + 1)) + lower;
    col_rnd_10 = (rand() % (upper - lower + 1)) + lower;
    row_rnd_10 = (rand() % (upper - lower + 1)) + lower;

    details.position_ship_two[0] = col_rnd_1;
    details.position_ship_two[1] = row_rnd_1;
    details.position_ship_three_one[0] = col_rnd_2;
    details.position_ship_three_one[1] = row_rnd_2;
    details.position_ship_three_two[0] = col_rnd_3;
    details.position_ship_three_two[1] = row_rnd_3;
    details.position_ship_four[0] = col_rnd_4;
    details.position_ship_four[1] = row_rnd_4;
    details.position_ship_five[0] = col_rnd_5;
    details.position_ship_five[1] = row_rnd_5;

    if (col_rnd_1 % 2 == 0)
    {
        details.position_ship_two[2] = 1;
    }
    else
    {
        details.position_ship_two[2] = 0;
    }

    if (col_rnd_2 % 2 == 0)
    {
        details.position_ship_three_one[2] = 1;
    }
    else
    {
        details.position_ship_three_one[2] = 0;
    }

    if (col_rnd_3 % 2 == 0)
    {
        details.position_ship_three_two[2] = 1;
    }
    else
    {
        details.position_ship_three_two[2] = 0;
    }

    if (col_rnd_4 % 2 == 0)
    {
        details.position_ship_four[2] = 1;
    }
    else
    {
        details.position_ship_four[2] = 0;
    }

    if (col_rnd_5 % 2 == 0)
    {
        details.position_ship_five[2] = 1;
    }
    else
    {
        details.position_ship_five[2] = 0;
    }

    return details;
};

void placeShip(char playersBoard[11][11], char shipSize, char rnd_pos[3], char identifier)
{
    char x_pos = rnd_pos[0];
    char y_pos = rnd_pos[1];
    char case_select = rnd_pos[2];
    char horizontal_free, vertical_free;
    int iteration = 0;

    switch (case_select)
    {
    case 1:
        if (playersBoard[x_pos + (shipSize - 1)][y_pos] == 0 && x_pos + (shipSize) <= 11 && iteration == 0)
        {
            horizontal_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos + 1][y_pos] + playersBoard[x_pos + 2][y_pos] + playersBoard[x_pos + 3][y_pos] + playersBoard[x_pos + 4][y_pos];
            while (horizontal_free != 0 && iteration < 200)
            {
                srand(time(NULL));
                x_pos = (rand() % 10) + 1;
                y_pos = (rand() % 10) + 1;
                horizontal_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos + 1][y_pos] + playersBoard[x_pos + 2][y_pos] + playersBoard[x_pos + 3][y_pos] + playersBoard[x_pos + 4][y_pos];
                iteration++;
            }
            if (iteration != 200)
            {
                for (char i = 0; i < shipSize; i++)
                {
                    playersBoard[x_pos + i][y_pos] = identifier;
                }
                break;
            }
            else
            {
                iteration = 0;
            }
        }
        else
        {
            horizontal_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos - 1][y_pos] + playersBoard[x_pos - 2][y_pos] + playersBoard[x_pos - 3][y_pos] + playersBoard[x_pos - 4][y_pos];
            iteration = 0;
            while (horizontal_free != 0 && iteration < 200)
            {
                srand(time(NULL));
                x_pos = (rand() % 10) + 1;
                y_pos = (rand() % 10) + 1;
                horizontal_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos - 1][y_pos] + playersBoard[x_pos - 2][y_pos] + playersBoard[x_pos - 3][y_pos] + playersBoard[x_pos - 4][y_pos];
                iteration++;
            }
            if (iteration != 100)
            {
                {
                    for (char i = 0; i < shipSize; i++)
                    {
                        playersBoard[x_pos - i][y_pos] = identifier;
                    }
                    break;
                }
            }
            else
            {
                iteration = 0;
                case_select = 0;
            }
        }
    case 0:
        if (playersBoard[x_pos][y_pos + (shipSize - 1)] == 0 && y_pos + (shipSize) <= 11 && iteration == 0)
        { // first try going right
            vertical_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos][y_pos + 1] + playersBoard[x_pos][y_pos + 2] + playersBoard[x_pos][y_pos + 3] + playersBoard[x_pos][y_pos + 4];
            while (vertical_free != 0 && iteration < 200)
            {
                srand(time(NULL));
                x_pos = (rand() % 10) + 1;
                y_pos = (rand() % 10) + 1;
                iteration++;
                vertical_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos][y_pos + 1] + playersBoard[x_pos][y_pos + 2] + playersBoard[x_pos][y_pos + 3] + playersBoard[x_pos][y_pos + 4];
            }
            if (iteration != 200)
            {
                for (char j = 0; j < shipSize; j++)
                {
                    playersBoard[x_pos][y_pos + j] = identifier;
                }
                break;
            }
            else
            {
                iteration = 0;
            }
        }
        else
        {
            iteration = 0;
            vertical_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos][y_pos - 1] + playersBoard[x_pos][y_pos - 2] + playersBoard[x_pos][y_pos - 3] + playersBoard[x_pos][y_pos - 4];
            while (vertical_free != 0 && iteration < 200)
            {
                srand(time(NULL));
                x_pos = (rand() % 10) + 1;
                y_pos = (rand() % 10) + 1;
                vertical_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos][y_pos - 1] + playersBoard[x_pos][y_pos - 2] + playersBoard[x_pos][y_pos - 3] + playersBoard[x_pos][y_pos - 4];
            }
            if (iteration != 200)
            {
                for (char j = 0; j < shipSize; j++)
                {
                    playersBoard[x_pos][y_pos - j] = identifier;
                }
                break;
            }
            else
            {
                iteration = 0;
                case_select = 1;
            }
        }
    }
    iteration = 0;
}

void randomAllocator(char playerssBoard[11][11], char two_slot_ship[3],
                     char three_one_slot_ship[3], char three_two_slot_ship[3],
                     char four_slot_ship[3], char five_slot_ship[3])
{
    placeShip(playerssBoard, 2, two_slot_ship, 1);
    placeShip(playerssBoard, 3, three_one_slot_ship, 1);
    placeShip(playerssBoard, 3, three_two_slot_ship, 1);
    placeShip(playerssBoard, 4, four_slot_ship, 1);
    placeShip(playerssBoard, 5, five_slot_ship, 1);
}

// Gameplay
struct gamer
{
    char name[50];
    int turn;
    int modality;
    int finish;
    struct tableBoard playerBoard;
    struct tableBoard frontendBoard;
};

struct gamer player_one;
struct gamer player_two;

struct gamer getNames(int i);
void getModality(struct gamer players, int j);
void playRoutine(struct gamer fisrt_p, struct gamer second_p, int minn_cells);

// *************************** Main ************************

int main()
{
    // Introduction to the game
    //printf("Hi!\nAre you guys ready to have some fun? Sink your enemies' ships "
    //       "to be known as the king of the sea.\n\n");
    pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

	static const gpio_map_t USART_GPIO_MAP =
	{
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	};

	// USART options.
	static const usart_options_t USART_OPTIONS =
	{
		.baudrate     = 38400,
		.charlength   = 8,
		.paritytype   = USART_EVEN_PARITY,
		.stopbits     = USART_2_STOPBITS,
		.channelmode  = USART_NORMAL_CHMODE
	};

	// Assign GPIO to USART.
	gpio_enable_module(USART_GPIO_MAP, sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	// Initialize USART in RS232 mode.
	usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, EXAMPLE_TARGET_PBACLK_FREQ_HZ);

    usart_write_line(EXAMPLE_USART,"Hi!\nAre you guys ready to have some fun? Sink your enemies' ships to be known as the king of the sea.\n\n");

    // Get the players' names
    player_one = getNames(0);
    player_two = getNames(1);

    // Player 1 setup: Mode selection and backend board creation
    do
    {
        getModality(player_one, 0);
        player_one.playerBoard = createBoard();

        switch (player_one.modality)
        {
        case 1: // Manual
            setManual(player_one.playerBoard.board);
            printPlayerBoard(player_one.playerBoard.board);
            break;
        case 2: // Auto
            srand(time(NULL));
            struct shipPos ships = randomPosition(10, 1);

            randomAllocator(player_one.playerBoard.board, ships.position_ship_two,
                            ships.position_ship_three_one,
                            ships.position_ship_three_two, ships.position_ship_four,
                            ships.position_ship_five);

            printPlayerBoard(player_one.playerBoard.board);
            break;
        }
        pl_one_agree = getAgree();

    } while (pl_one_agree == 0);

    // Player 2 setup: Mode selection and  backend board creation
    do
    {
        getModality(player_two, 1);
        player_two.playerBoard = createBoard();

        switch (player_two.modality)
        {
        case 1: // Manual
            setManual(player_two.playerBoard.board);
            printPlayerBoard(player_two.playerBoard.board);
            break;
        case 2: // Auto
            srand(time(NULL));
            struct shipPos ships = randomPosition(10, 1);

            randomAllocator(player_two.playerBoard.board, ships.position_ship_two,
                            ships.position_ship_three_one,
                            ships.position_ship_three_two, ships.position_ship_four,
                            ships.position_ship_five);

            printPlayerBoard(player_two.playerBoard.board);
            break;
        }
        pl_two_agree = getAgree();
    } while (pl_two_agree == 0);

    // Frontend board creation for both players
    player_one.frontendBoard = createBoard();
    player_two.frontendBoard = createBoard();

    // We count the cells of both boards to see if both have the same amount of
    // ships. In case they dont then we set it down.
    num_cells_pone = countCells(player_one.playerBoard.board);
    num_cells_ptwo = countCells(player_two.playerBoard.board);
    num_cells = minCells(num_cells_pone, num_cells_ptwo);
    //  printf("%i", num_cells);
    //  printf("%i", num_cells_pone);
    //  printf("%i", num_cells_ptwo);
	usart_write_line(EXAMPLE_USART,convertIntegerToChar(num_cells_pone));
	usart_write_line(EXAMPLE_USART,convertIntegerToChar(num_cells_ptwo));
	usart_write_line(EXAMPLE_USART,convertIntegerToChar(num_cells));

    // Finally, we do the game routine
    playRoutine(player_one, player_two, num_cells);

    return 0;
}

// *************************** Functions *******************

// Gameplay
struct gamer getNames(int i)
{
    struct gamer players;
    if (i == 0)
    {
        //printf("Please, enter first player's name:\n");
        usart_write_line(EXAMPLE_USART,"Please, enter first player's name:\n");
        //scanf("%s", players.name);
        usart_read_line(players.name,50);
    }
    else
    {
        //printf("Please, enter second player's name:\n");
        usart_write_line(EXAMPLE_USART,"Please, enter second player's name:\n");
        //scanf("%s", players.name);
        usart_read_line(players.name,50);
    }
    return players;
}

void getModality(struct gamer players, int j)
{
    /* printf("\n%s, please select the number of the option that describes how you "
           "would like to accomodate your ships:\n1.- Manual\n2.- Automatic\n",
           players.name); */

    usart_putchar(EXAMPLE_USART,10);
    usart_write_line(EXAMPLE_USART,players.name);
    usart_write_line(EXAMPLE_USART,", please select the number of the option that describes how you would like to accomodate your ships:\n1.- Manual\n2.- Automatic\n");
    
    if (j == 0)
    {
        //scanf("%i", &player_one.modality);
		char modality[2];
		usart_read_line(modality,2);
        player_one.modality=atoi(modality);
		usart_putchar(EXAMPLE_USART,48+player_one.modality);
    }
    else
    {
        //scanf("%i", &player_two.modality);
		char modality[2];
		usart_read_line(modality,2);
        player_two.modality=atoi(modality);
    }
}

void setManual(char playerssBoard[11][11])
{
    for (int n_ship = 0; n_ship < size_ships; n_ship++)
    {

        do
        {
            rep_err = 0;

            do
            {
                rep = requirements_msg(n_ship, playerssBoard);
            } while (rep != 0);
            limit_err = 0;
            limit_errt = 0;

            do
            {
                err_h = 0;
                err_v = 0;
                //printf("Horizontal: H\tVertical: V\n");
                usart_write_line(EXAMPLE_USART,"Horizontal: H\tVertical: V\n");
                //scanf("%c", &ship_orientation);
				char porientation[2];
				usart_read_line(porientation,2);
                ship_orientation = porientation[0];

                if (ship_orientation == 72 || ship_orientation == 104) //H or h
                {
                    if ((y + ships[n_ship]) > size_board)
                    {
                        err_h = 1;
                        //printf("This orientation exceed the board limits\n");
                        usart_write_line(EXAMPLE_USART,"This orientation exceed the board limits\n");
                        //getchar();
                    }
                }
                else if (ship_orientation == 86 || ship_orientation == 118) //86 118
                {
                    if ((x + ships[n_ship]) > size_board)
                    {
                        err_v = 1;
                        //printf("This orientation exceed the board limits\n");
                        usart_write_line(EXAMPLE_USART,"This orientation exceed the board limits\n");
                        //getchar();
                    }
                }
                limit_err = limit_err + err_h + err_v;

                if (limit_err == 0)
                {

                    err_ht = 0;
                    err_vt = 0;

                    if (ship_orientation == 72 || ship_orientation == 104)
                    {
                        if (orientation(n_ship, playerssBoard))
                        {
                            err_ht = 1;
                            //printf("The boat overlaps another boat\n");
                            usart_write_line(EXAMPLE_USART,"The boat overlaps another boat\n");
                            //getchar();
                        }
                    }
                    else if (ship_orientation == 86 || ship_orientation == 118)
                    {
                        if (orientation(n_ship, playerssBoard))
                        {
                            err_vt = 1;
                            //printf("The boat overlaps another boat\n");
                            usart_write_line(EXAMPLE_USART,"The boat overlaps another boat\n");
                            //getchar();
                        }
                    }
                    limit_errt = limit_errt + err_ht + err_vt;
                }

            } while (((err_h == 1 || err_v == 1) && limit_err != 2) ||
                     ((err_ht == 1 || err_vt == 1) && limit_err != 2));

            if (limit_err == 2 || limit_errt == 2)
            {
                //printf("Choose another coordinate\n");
                usart_write_line(EXAMPLE_USART,"Choose another coordinate\n");
                rep_err = 1;
            }
        } while (rep_err == 1);

        playerssBoard[x][y] = 1;
    }
}

int getAgree()
{
    char temp_agree;
    int temp_flag;
    //printf("Above you can see the resulting board. If you like it then please "
    //       "type Y, or if you want to change it please tipe N:\n");
    usart_write_line(EXAMPLE_USART,"Above you can see the resulting board. If you like it then please type Y, or if you want to change it please tipe N:\n");       
    //scanf("%s", &temp_agree);
    temp_agree = usart_getchar(EXAMPLE_USART);
	usart_getchar(EXAMPLE_USART);
    if (temp_agree == 78 || temp_agree == 110) //N or n
    {
        temp_flag = 0;
    }
    else
    {
        temp_flag = 1;
        //system("clear");
        //usart_putchar(EXAMPLE_USART, 12); //12 or 13
    }
    return temp_flag;
}

int hitShip(char backend[limit][limit], char frontend[limit][limit], int x,
            int y, int counter)
{
    int val = (int)backend[x][y];

    if (val==1)
    {
        frontend[x][y] = 2;
        //printf("\n*************Ouch \n");
        usart_write_line(EXAMPLE_USART,"\n*************Ouch \n");
		usart_putchar(EXAMPLE_USART,frontend[x][y]+48);
		usart_write_line(EXAMPLE_USART,"\n");
		
        counter += 1;
    }
    else if (val == 48)
    {
        frontend[x][y] = 3;
        //printf("\n*************You missed!!!! \n");
        usart_write_line(EXAMPLE_USART,"\n*************You missed!!!! \n");
    }
    return counter;
}

int countCells(char backend[11][11])
{
    int sum = 0;
    for (int i = 1; i < 11; i++)
    {
        for (int j = 1; j < 11; j++)
        {
            if (backend[i][j] > 1 && backend[i][j] != 48 )
            {
                sum += 1;
            }
            else if(backend[i][j] == 48 )
			{
				sum += 0;
			}
			else
			{
                sum += backend[i][j];
            }
        }
    }
    return sum;
}

int minCells(int a, int b)
{
    int c;
    if (a > b)
    {
        a = b;
    }
    else
    {
        b = a;
    }
    c = a;
    return c;
}

void playRoutine(struct gamer first_p, struct gamer second_p, int minn_cells)
{
    first_p.turn = 1;   // Player one starts
    num_cells_pone = 0; // Restart counters
    num_cells_ptwo = 0;
    int x_axis;
    int y_axis;
    //printf("MIN CELLS %i\n", minn_cells);
    usart_write_line(EXAMPLE_USART,"MIN CELLS ");
    usart_write_line(EXAMPLE_USART,convertIntegerToChar(minn_cells));
    usart_write_line(EXAMPLE_USART,"\n");
    do
    {
        do
        {
            //printf(
            //    "%s, please choose a cell by entering it in the following format x,y: ",
            //    first_p.name);
            usart_write_line(EXAMPLE_USART,first_p.name);
            usart_write_line(EXAMPLE_USART,", please choose a cell by entering it in the following format x,y: ");
            
            //scanf("%i, %i", &x_axis, &y_axis);
            char input[5];
            float output[2];
            usart_read_line(input,5);
            parseString(input,",",output);
            x_axis = (int)output[0];
            y_axis = (int)output[1];
            num_cells_pone =
                hitShip(second_p.playerBoard.board, second_p.frontendBoard.board,
                        x_axis, y_axis, num_cells_pone);
						
            printPlayerBoardwColor(second_p.frontendBoard.board);
            first_p.turn = 0;
            second_p.turn = 1;
        } while (first_p.turn == 1);
        if (num_cells_pone < minn_cells)
        {
            do
            {
                //printf(
                //    "%s, please choose a cell by entering it in the following format x,y: ",
                //    second_p.name);
                usart_write_line(EXAMPLE_USART,second_p.name);
                usart_write_line(EXAMPLE_USART,", please choose a cell by entering it in the following format x,y: ");
                //scanf("%i, %i", &x_axis, &y_axis);
                char input[5];
                float output[2];
                usart_read_line(input,5);
                parseString(input,",",output);
                x_axis = (int)output[0];
                y_axis = (int)output[1];

                num_cells_ptwo =
                    hitShip(first_p.playerBoard.board, first_p.frontendBoard.board,
                            x_axis, y_axis, num_cells_ptwo);
                printPlayerBoardwColor(first_p.frontendBoard.board);
                first_p.turn = 1;
                second_p.turn = 0;
            } while (second_p.turn == 1);
        }

    } while ((num_cells_pone < minn_cells) && (num_cells_ptwo < minn_cells));
    //printf("GAME OVER. ");
    usart_write_line(EXAMPLE_USART,"GAME OVER. ");
    if (num_cells_pone >= minn_cells)
    {
        //printf("%s, WINS", first_p.name);
        usart_write_line(EXAMPLE_USART,first_p.name);
        usart_write_line(EXAMPLE_USART,", WINS");
    }
    else
    {
        //printf("%s, WINS", second_p.name);
        usart_write_line(EXAMPLE_USART,second_p.name);
        usart_write_line(EXAMPLE_USART,", WINS");
    }
}

// Manual

int requirements_msg(char n_ship, char playerssBoard[11][11])
{

    //printf("Ship No. %d of size %i. Write the coordinate in the Format x,y:\n", n_ship + 1, ships[n_ship]);
    usart_write_line(EXAMPLE_USART,"Ship No. ");
    usart_putchar(EXAMPLE_USART,(48+ n_ship + 1));
    usart_write_line(EXAMPLE_USART," of size ");
    usart_putchar(EXAMPLE_USART,(48 + ships[n_ship]));
	usart_write_line(EXAMPLE_USART," Write the coordinate in the Format x,y:\n");
    //scanf("%d,%d", &x, &y);
    char input[5];
    float output[2];
    usart_read_line(input,5);
    parseString(input,",",output);
    x=output[0];
    y=output[1];
    //getchar();


    if (coordinates(x, y, playerssBoard))
    {
        return ERR;
    }
    return OK;
}

int coordinates(char x, char y, char playerssBoard[11][11])
{
    if (playerssBoard[x][y] == 1)
    {
        printf("The ship cannot be placed at that coordinate\n");
        return ERR;
    }
    return OK;
}

int orientation(int n_ship, char playerssBoard[11][11])
{

    if (ship_orientation == 72 || ship_orientation == 104)
    {
        for (int i = 1; i < (int)ships[n_ship]; i++)
        {
            if (board[x][y + i] == 1)
                return ERR;
        }
        for (int i = 1; i < (int)ships[n_ship]; i++)
        {
            playerssBoard[x][y + i] = 1;
        }
    }
    else if (ship_orientation == 86 || ship_orientation == 118)
    {
        for (int i = 1; i < (int)ships[n_ship]; i++)
        {
            if (playerssBoard[x + i][y] == 1)
            {
                return ERR;
            }
        }
        for (int i = 1; i < (int)ships[n_ship]; i++)
        {
            playerssBoard[x + i][y] = 1;
        }
    }
    return OK;
}

void usart_read_line(char *lineRead, size_t len){
	char line[len];
	int  i = 0;
	memset(lineRead, 0, len); 
	
	while(1) {
		line[i] = usart_getchar(EXAMPLE_USART); //Si se llega a utilizar el buffer "line" hay que limpiarlo antes o cualquier otro buffer
		if (line[i] == '\n') break;
		i++;
	}
	line[i] = 0;
	
	strncpy(lineRead, line, len);
}

void parseString(char *string_in, char *delimiter, float output_array[])
{
	char* pch; //create buffer
	pch = strtok(string_in, delimiter); //begin parsing the values
	uint8_t i = 0;
	while (pch != NULL)
	{
		output_array[i] = atof(pch);
		pch = strtok(NULL, delimiter);
		i++;
	}
}

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