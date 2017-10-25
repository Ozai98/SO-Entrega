#include <stdio.h>
#include <termios.h>
#include <unistd.h>
// #include <unistd.h>
// #include "hashTable.c"
// #include <ctype.h>
// int keypress(unsigned char echo) {
// int keypress() {
//   struct termios savedState, newState;
//   if (-1 == tcgetattr(STDIN_FILENO, &savedState)){
//     return EOF;     /* error on tcgetattr */
//   }
//   newState = savedState;
//   /* disable canonical input and disable echo.  set minimal input to 1. */
//   newState.c_lflag &= ~ICANON;
//   newState.c_cc[VMIN] = 1;
//   if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &newState)){
//     return EOF;     /* error on tcsetattr */
//   }
//   int c;
//   printf("Ingrese cualquier tecla para continuar: ");
//   c = getchar();
//   printf("\nc: %c\n", c);
//   /* restore the saved state */
//   if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &savedState)){
//     return EOF;     /* error on tcsetattr */
//   }
//   return c;
// }
// void func(){
//   static struct termios newt, oldt;
//   printf("\nPresione una tecla para continuar...");
// 	tcgetattr( STDIN_FILENO, &oldt);	//Obteniendo la configuracion actual de la terminal
// 	newt = oldt;				//Copiando la configuracion
// 	newt.c_lflag &= ~ICANON;	//Desactivando la bandera ICANON para no esperar un caracter especial al final
//   newState.c_lflag &= ~ICANO;
//   tcsetattr( STDIN_FILENO, TCSANOW, &newt);//Configurando la terminal con los nuevos cambios
// 	getchar();				//Esperando un caracter
// 	__fpurge(stdin);			//Limpiando el buffer de entrada
// 	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);//Configurando la terminal a su forma original
// }
void func2();
void func3();
int hashTable[5];
int main(){
  // printf("Algo\n");
  func3();
  func2();
  func3();

  // int impar = 24 & 1;

  // char echo = 50;
  // keypress(echo);

  // FILE* file = checkfopen(DATA_DOGS_PATH, "r");
  // fseek(file, 0, SEEK_END);
  // printf("%lu", sizeof(struct dogType));
  // printf("%lu",ftell(file));
  // checkfclose(file, DATA_DOGS_PATH);
}
void func2(){
  int i=0;
  for(i=0; i<5; i++){
    hashTable[i] = 3;
  }
}
void func3(){
  printf("[");
  int i=0;
  for(i=0; i<4; i++){
    printf("%i,", hashTable[i]);
  }
  printf("%i", hashTable[i]);
  printf("]\n");
}
