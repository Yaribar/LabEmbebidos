#include <stdio.h>

#define ERR 1
#define OK 0

char board[11][11] = {0};
char size_board = sizeof(board) / sizeof(board[0]);
char barcos[2] = {5, 4};
char size_barcos = sizeof(barcos) / sizeof(barcos[0]);
char orientacion;
char rep=0;
char limit_err=0,limit_errt=0;
char traslape=0;

int x, y;
char err_h = 0,err_v = 0;
char err_ht = 0,err_vt = 0;
char rep_err=0;

int msg_requerimientos(char n_barco);
int coordenadas(char x,char y);
int orientation(char n_barco);
void printBoard();

int main(void) {

  for (int n_barco = 0; n_barco < size_barcos; n_barco++) {
    
    do{
      rep_err=0;
    
      do{
        rep=msg_requerimientos(n_barco);
      }while(rep!=0);
      limit_err=0;
      
      do{
        err_h=0;
        err_v=0;
        printf("Horizontal: H\tVertical: V\n");
        scanf("%c", &orientacion);
        
        if (orientacion == 'H') {
          if((y+barcos[n_barco])>size_board){
            err_h=1;
            printf("Excede los limites del tablero esta orientacion\n");
            getchar();
          }
        }else if (orientacion == 'V') {
          if((x+barcos[n_barco])>size_board){
            err_v=1;
            printf("Excede los limites del tablero esta orientacion\n");
            getchar();
          }
        }
        limit_err=err_h+err_v;
  
        if(limit_err==0){
  
          err_ht=0;
          err_vt=0;
    
          if (orientacion == 'H') {
            if(orientation(n_barco)){
              err_ht=1;
              printf("El barco se traslapa con otro barco\n");
              getchar();
            }
          }else if (orientacion == 'V') {
            if(orientation(n_barco)){
              err_vt=1;
              printf("El barco se traslapa con otro barco\n");
              getchar();
            }
          }
          limit_errt=err_ht+err_vt;
  
        }
        
      }while(((err_h==1 || err_v==1) && limit_err!=2) || ((err_ht==1 || err_vt==1) && limit_err!=2));
  
      if(limit_err==2 || limit_errt==2){
        printf("Escoge otra coordenada\n");  
        rep=1;
      }
    }while(rep_err==1);
    
    board[x][y]=1;

  }

  printBoard();
  return 0;
}

int msg_requerimientos(char n_barco){

      printf("Barco No. %d de tamaño %i. Escribe la coordenada en el "
             "formato:x,y\n",
             n_barco + 1, barcos[n_barco]);
      scanf("%d,%d", &x, &y);
      getchar();
  
      if(coordenadas(x,y)){
        return ERR;
      }
      return OK;
}

int coordenadas(char x, char y){
  if (board[x][y] == 1) {
    printf("El barco no puede ser colocado en esa coordenada\n");
    return ERR;
  }
  return OK;
}

int orientation(char n_barco){
  if (orientacion == 'H') {
    for (int i = 1; i < barcos[n_barco]; i++) {
      if(board[x][y + i] == 1)
        return ERR;
    }
    for (int i = 1; i < barcos[n_barco]; i++) {
      board[x][y + i] = 1;
    }
  } else if (orientacion == 'V') {
    for (int i = 1; i < barcos[n_barco]; i++) {
      if(board[x + i][y] == 1){
        return ERR;
      }
    }
    for (int i = 1; i < barcos[n_barco]; i++){
      board[x + i][y] = 1;
    }
  }
  return OK;
}

void printBoard(){
  for (int i = 0; i < 11; i++) {
    for (int j = 0; j < 11; j++) {
      printf("%d ", board[i][j]);
    }
    printf("\n");
  }
}


