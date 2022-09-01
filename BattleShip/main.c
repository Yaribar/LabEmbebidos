// BATTLESHIP BY ZORRIEMBEBIDOS

// *********************** Libraries **********************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ***********************   Macros  **********************

// Manual Modex
#define ERR 1
#define OK 0

// Gameplay
#define limit 11

// Color codes for printing
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

// ***********************  Variables  ********************

// Manual Mode
char board[11][11] = {0};
char size_board = sizeof(board) / sizeof(board[0]);
char ships[5] = {5, 4, 3, 3, 2};
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

// Manual Mode
int requirements_msg(char n_ship, char playerssBoard[11][11]);
int coordinates(char x, char y, char playerssBoard[11][11]);
int orientation(char n_ship, char playerssBoard[11][11]);
void printBoard(char playerssBoard[11][11]);
void setManual(char playerssBoard[11][11]);

// Gameplay
int getAgree();
int hitShip(char backend[11][11], char frontend[11][11], int x, int y,int counter);
int countCells(char backend[11][11]);
int minCells(int a, int b);

// ********************** Structures ***********************

// Auto Mode
struct tableBoard {
  char board[11][11];
};
struct tableBoard createBoard() {
  struct tableBoard playerBoard;

  for (char i = 0; i < 11; i++) {
    for (char j = 0; j < 11; j++) {
      if (i == 0 && j == 0) {
        playerBoard.board[0][j] = 0;
      } else if (i > 0 && j == 0) { // row numbers
        playerBoard.board[i][j] = i;
      } else if (i == 0 && j > 0) { // column numbers
        playerBoard.board[i][j] = j;
      } else {
        playerBoard.board[i][j] = 0;
      }
    }
  }

  return playerBoard;
};

void printPlayerBoard(char playersBoard[11][11]){
    
    for (char i = 0; i < 11; i++){
        for (char j = 0; j<11; j++){
            if (i + j == 0){
                printf("   ");
            }
            else if (playersBoard[i][j] < 10){
                if ((i > 0 && j == 0) || (i == 0 && j > 0)){
                    printf(" %i ",playersBoard[i][j]);
                }
                else if (i == 0 && j == 0){
                    printf(" %i ",playersBoard[i][j]);
                }
                else if (playersBoard[i][j] == 0){
                    printf(BLU " %i " RESET,playersBoard[i][j]);
                }
              else{
                    printf(YEL " %i " RESET,playersBoard[i][j]);
                }
            }
            else{
                if ((i > 0 && j == 0) || (i == 0 && j > 0)){
                    printf("%i ",playersBoard[i][j]);
                }
            }
        }
        printf("\n\n");
    }
}

void printPlayerBoardwColor(char playersBoard[11][11]){
    
    for (char i = 0; i < 11; i++){
        for (char j = 0; j < 11; j++){
        
            if (i + j == 0){
                printf("   ");
            }
            else if ((i == 0 && j > 0) || (i > 0 && j == 0)){  // first row and column
                if (playersBoard[i][j] < 10){
                    printf(" %i ",playersBoard[i][j]);
                }
                else{
                    printf("%i ",playersBoard[i][j]);
                }
            }
            else{
                if (playersBoard[i][j] == 0){
                    printf(BLU " * " RESET);
                }
                else if (playersBoard[i][j] == 1){
                    printf(BLU " * " RESET);
                }
                else if (playersBoard[i][j] == 2){
                    printf(RED " X " RESET);
                }
                else{
                    printf(YEL " * " RESET);
                }
            }
        }
        printf("\n\n");
    }
}

struct shipPos {
  char position_ship_two[3];
  char position_ship_three_one[3];
  char position_ship_three_two[3];
  char position_ship_four[3];
  char position_ship_five[3];
};

struct shipPos randomPosition(char upper, char lower) {
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

  if (col_rnd_1 % 2 == 0) {
    details.position_ship_two[2] = 1;
  } else {
    details.position_ship_two[2] = 0;
  }

  if (col_rnd_2 % 2 == 0) {
    details.position_ship_three_one[2] = 1;
  } else {
    details.position_ship_three_one[2] = 0;
  }

  if (col_rnd_3 % 2 == 0) {
    details.position_ship_three_two[2] = 1;
  } else {
    details.position_ship_three_two[2] = 0;
  }

  if (col_rnd_4 % 2 == 0) {
    details.position_ship_four[2] = 1;
  } else {
    details.position_ship_four[2] = 0;
  }

  if (col_rnd_5 % 2 == 0) {
    details.position_ship_five[2] = 1;
  } else {
    details.position_ship_five[2] = 0;
  }

  return details;
};

void placeShip(char playersBoard[11][11],char shipSize, char rnd_pos[3], char identifier){
  char x_pos = rnd_pos[0];
  char y_pos = rnd_pos[1];
  char case_select = rnd_pos[2];
  char horizontal_free, vertical_free;
  int iteration = 0;

  switch(case_select){
    case 1:
        if (playersBoard[x_pos + (shipSize - 1)][y_pos] == 0 && x_pos + (shipSize) <= 11 && iteration == 0){  
            horizontal_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos + 1][y_pos] + playersBoard[x_pos + 2][y_pos] + playersBoard[x_pos + 3][y_pos] + playersBoard[x_pos + 4][y_pos];
            while (horizontal_free != 0 && iteration < 200){
                srand(time(NULL));
                x_pos = (rand() % 10)+1;
                y_pos = (rand() % 10)+1;
                horizontal_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos + 1][y_pos] + playersBoard[x_pos + 2][y_pos] + playersBoard[x_pos + 3][y_pos] + playersBoard[x_pos + 4][y_pos];
                iteration++;
            }
            if (iteration != 200){for (char i = 0; i< shipSize; i++){playersBoard[x_pos + i][y_pos] = identifier;} break;}
            else {iteration = 0;}
        }
        else {
            horizontal_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos - 1][y_pos] + playersBoard[x_pos - 2][y_pos] + playersBoard[x_pos - 3][y_pos] + playersBoard[x_pos - 4][y_pos];
            iteration = 0;
            while (horizontal_free != 0 && iteration < 200){
                srand(time(NULL));
                x_pos = (rand() % 10)+1;
                y_pos = (rand() % 10)+1;
                horizontal_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos - 1][y_pos] + playersBoard[x_pos - 2][y_pos] + playersBoard[x_pos - 3][y_pos] + playersBoard[x_pos - 4][y_pos];
                iteration++;
            }
            if (iteration != 100){{for (char i = 0; i< shipSize; i++){playersBoard[x_pos - i][y_pos] = identifier;} break;}}
            else{iteration = 0; case_select = 0;}
        }
    case 0:
        if (playersBoard[x_pos][y_pos + (shipSize - 1)] == 0 && y_pos + (shipSize) <= 11 && iteration == 0){ //first try going right
            vertical_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos][y_pos + 1] + playersBoard[x_pos][y_pos + 2] + playersBoard[x_pos][y_pos + 3] + playersBoard[x_pos][y_pos + 4];
            while (vertical_free != 0 && iteration < 200){
                srand(time(NULL));
                x_pos = (rand() % 10)+1;
                y_pos = (rand() % 10)+1;
                iteration++;
                vertical_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos][y_pos + 1] + playersBoard[x_pos][y_pos + 2] + playersBoard[x_pos][y_pos + 3] + playersBoard[x_pos][y_pos + 4];
            }
            if (iteration != 200){for (char j = 0; j< shipSize; j++){playersBoard[x_pos][y_pos + j] = identifier;} break;}
            else{ iteration = 0;}
        }
        else {
            iteration = 0;
            vertical_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos][y_pos - 1] + playersBoard[x_pos][y_pos - 2] + playersBoard[x_pos][y_pos - 3] + playersBoard[x_pos][y_pos - 4];
            while ( vertical_free!= 0 && iteration < 200){
                srand(time(NULL));
                x_pos = (rand() % 10)+1;
                y_pos = (rand() % 10)+1;
                vertical_free = playersBoard[x_pos][y_pos] + playersBoard[x_pos][y_pos - 1] + playersBoard[x_pos][y_pos - 2] + playersBoard[x_pos][y_pos - 3] + playersBoard[x_pos][y_pos - 4];
            }
            if (iteration != 200){for (char j = 0; j< shipSize; j++){playersBoard[x_pos][y_pos - j] = identifier;} break;}
            else {iteration = 0; case_select = 1;}
        }
}
    iteration = 0;
}

void randomAllocator(char playerssBoard[11][11], char two_slot_ship[3],
                     char three_one_slot_ship[3], char three_two_slot_ship[3],
                     char four_slot_ship[3], char five_slot_ship[3]) {
  placeShip(playerssBoard, 2, two_slot_ship, 1);
  placeShip(playerssBoard, 3, three_one_slot_ship, 1);
  placeShip(playerssBoard, 3, three_two_slot_ship, 1);
  placeShip(playerssBoard, 4, four_slot_ship, 1);
  placeShip(playerssBoard, 5, five_slot_ship, 1);
}

// Gameplay
struct gamer {
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

int main() {
  // Introduction to the game
  printf("Hi!\nAre you guys ready to have some fun? Sink your enemies' ships "
         "to be known as the king of the sea.\n\n");

  // Get the players' names
  player_one = getNames(0);
  player_two = getNames(1);

  // Player 1 setup: Mode selection and backend board creation
  do {
    getModality(player_one, 0);
    player_one.playerBoard = createBoard();

    switch (player_one.modality) {
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
  do {
    getModality(player_two, 1);
    player_two.playerBoard = createBoard();

    switch (player_two.modality) {
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

  // Finally, we do the game routine
  playRoutine(player_one, player_two, num_cells);

  return 0;
}

// *************************** Functions *******************

// Gameplay
struct gamer getNames(int i) {
  struct gamer players;
  if (i == 0) {
    printf("Please, enter first player's name:\n");
    scanf("%s", players.name);
  } else {
    printf("Please, enter second player's name:\n");
    scanf("%s", players.name);
  }
  return players;
}

void getModality(struct gamer players, int j) {
  printf("\n%s, please select the number of the option that describes how you "
         "would like to accomodate your ships:\n1.- Manual\n2.- Automatic\n",
         players.name);
  if (j == 0) {
    scanf("%i", &player_one.modality);
  } else {
    scanf("%i", &player_two.modality);
  }
}

void setManual(char playerssBoard[11][11]) {
  for (int n_ship = 0; n_ship < size_ships; n_ship++) {

    do {
      rep_err = 0;

      do {
        rep = requirements_msg(n_ship, playerssBoard);
      } while (rep != 0);
      limit_err = 0;
      limit_errt = 0;

      do {
        err_h = 0;
        err_v = 0;
        printf("Horizontal: H\tVertical: V\n");
        scanf("%c", &ship_orientation);

        if (ship_orientation == 'H') {
          if ((y + ships[n_ship]) > size_board) {
            err_h = 1;
            printf("This orientation exceed the board limits\n");
            getchar();
          }
        } else if (ship_orientation == 'V') {
          if ((x + ships[n_ship]) > size_board) {
            err_v = 1;
            printf("This orientation exceed the board limits\n");
            getchar();
          }
        }
        limit_err = limit_err + err_h + err_v;

        if (limit_err == 0) {

          err_ht = 0;
          err_vt = 0;

          if (ship_orientation == 'H') {
            if (orientation(n_ship, playerssBoard)) {
              err_ht = 1;
              printf("The boat overlaps another boat\n");
              getchar();
            }
          } else if (ship_orientation == 'V') {
            if (orientation(n_ship, playerssBoard)) {
              err_vt = 1;
              printf("The boat overlaps another boat\n");
              getchar();
            }
          }
          limit_errt = limit_errt + err_ht + err_vt;
        }

      } while (((err_h == 1 || err_v == 1) && limit_err != 2) ||
               ((err_ht == 1 || err_vt == 1) && limit_err != 2));

      if (limit_err == 2 || limit_errt == 2) {
        printf("Choose another coordinate\n");
        rep_err = 1;
      }
    } while (rep_err == 1);

    playerssBoard[x][y] = 1;
  }
}

int getAgree() {
  char temp_agree;
  int temp_flag;
  printf("Above you can see the resulting board. If you like it then please "
         "type Y, or if you want to change it please tipe N:\n");
  scanf("%s", &temp_agree);
  if (temp_agree == 'N') {
    temp_flag = 0;
  } else {
    temp_flag = 1;
    system("clear");
  }
  return temp_flag;
}

int hitShip(char backend[limit][limit], char frontend[limit][limit], int x,
            int y, int counter) {
  int val = backend[x][y];

  if (val) {
    frontend[x][y] = 2;
    printf("\n*************Ouch \n");
    counter += 1;
  } else if (val == 0) {
    frontend[x][y] = 3;
    printf("\n*************You missed!!!! \n");
  }
  return counter;
}

int countCells(char backend[11][11]) {
  int sum = 0;
  for (int i = 1; i < 11; i++) {
    for (int j = 1; j < 11; j++) {
      if (backend[i][j] > 1) {
        sum += 1;
      } else {
        sum += backend[i][j];
      }
    }
  }
  return sum;
}

int minCells(int a, int b) {
  int c;
  if (a > b) {
    a = b;
  } else {
    b = a;
  }
  c = a;
  return c;
}

void playRoutine(struct gamer first_p, struct gamer second_p, int minn_cells) {
  first_p.turn = 1;   // Player one starts
  num_cells_pone = 0; // Restart counters
  num_cells_ptwo = 0;
  int x_axis;
  int y_axis;
  printf("MIN CELLS %i\n", minn_cells);
  do {
    do {
      printf(
          "%s, please choose a cell by entering it in the following format: ",
          first_p.name);
      scanf("%i, %i", &x_axis, &y_axis);
      num_cells_pone =
          hitShip(second_p.playerBoard.board, second_p.frontendBoard.board,
                  x_axis, y_axis, num_cells_pone);
      printPlayerBoardwColor(second_p.frontendBoard.board);
      first_p.turn = 0;
      second_p.turn = 1;
    } while (first_p.turn == 1);
    if (num_cells_pone < minn_cells) {
      do {
        printf(
            "%s, please choose a cell by entering it in the following format: ",
            second_p.name);
        scanf("%i, %i", &x_axis, &y_axis);
        num_cells_ptwo =
            hitShip(first_p.playerBoard.board, first_p.frontendBoard.board,
                    x_axis, y_axis, num_cells_ptwo);
        printPlayerBoardwColor(first_p.frontendBoard.board);
        first_p.turn = 1;
        second_p.turn = 0;
      } while (second_p.turn == 1);
    }

  } while ((num_cells_pone < minn_cells) && (num_cells_ptwo < minn_cells));
  printf("GAME OVER. ");
  if (num_cells_pone >= minn_cells) {
    printf("%s, WINS", first_p.name);
  } else {
    printf("%s, WINS", second_p.name);
  }
}

// Manual

int requirements_msg(char n_ship, char playerssBoard[11][11]) {

  printf("Ship No. %d of size %i. Write the coordinate in the Format x,y:\n",n_ship + 1, ships[n_ship]);
  scanf("%d,%d", &x, &y);
  getchar();

  if (coordinates(x, y, playerssBoard)) {
    return ERR;
  }
  return OK;
}

int coordinates(char x, char y, char playerssBoard[11][11]) {
  if (playerssBoard[x][y] == 1) {
    printf("The ship cannot be placed at that coordinate\n");
    return ERR;
  }
  return OK;
}

int orientation(char n_ship, char playerssBoard[11][11]) {
  if (ship_orientation == 'H') {
    for (int i = 1; i < ships[n_ship]; i++) {
      if (board[x][y + i] == 1)
        return ERR;
    }
    for (int i = 1; i < ships[n_ship]; i++) {
      playerssBoard[x][y + i] = 1;
    }
  } else if (ship_orientation == 'V') {
    for (int i = 1; i < ships[n_ship]; i++) {
      if (playerssBoard[x + i][y] == 1) {
        return ERR;
      }
    }
    for (int i = 1; i < ships[n_ship]; i++) {
      playerssBoard[x + i][y] = 1;
    }
  }
  return OK;
}