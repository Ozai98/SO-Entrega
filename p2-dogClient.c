#include "atributes.c"
#include <unistd.h>
#include <ctype.h>
#include <termios.h>
#include <stdio_ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <arpa/inet.h>

#include <string.h>
#include <ctype.h>

#define PORT 3535


//Declaración de las funciones
void menu(int sd);
void exeMenu(int sd);
void addReg(int sd);
void seeReg(int sd);
void deleteReg(int sd);
void searchReg(int sd);

int main(int argc, char *argv[]){
  int sd = socket(AF_INET, SOCK_STREAM, 0);
  int optval;
  if(sd == -1){
    perror("Socket error\n");
    exit(-1);
  }
  struct sockaddr_in server;
  socklen_t serverSize;

  // Assigning a name to a socket
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = inet_addr(argv[1]);
  bzero(server.sin_zero, 8);

  serverSize = sizeof(struct sockaddr);
  optval = 1;
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

  int r = connect(sd, (struct sockaddr*)&server, serverSize);
  if(r == -1){
    perror("Connect error");
    exit(-1);
  }
  // int num = 99;
  // r = send(sd, num, sizeof(int), 0);
  // if(r == -1){
  //   perror("Send error\n");
  //   exit(-1);
  // }

	menu(sd);

  close(sd);
	return 0;
}

// Menú que despliega las opciones
void menu(int sd){
	char input[32];
	int num = 0, r;
	printf("--------------------------------------------------\n");
	printf("\t\tTienda de mascotas\n");
	printf("--------------------------------------------------\n");
	printf("\t1\tIngresar registro\n");
	printf("\t2\tVer registro\n");
	printf("\t3\tBorrar registro\n");
	printf("\t4\tBuscar registro\n");
	printf("\t5\tSalir\n");
	printf("--------------------------------------------------\n");

	/*En caso de no ingresar un numero dentro de las opciones expuestas,
	  se volverá a solicitar el ingreso de la opcion*/
	do{
		printf("Ingrese la opcion deseada: ");
		scanf("%32s", input);
		num = input[0]-48;
		printf("Opcion %s: ", input);
		if (strlen(input) != 1 || num < 1 || num > 5)
			printf("%s\n", "El valor ingresado no es valido, intente de nuevo");
	}while(strlen(input) != 1 || num < 1 || num > 5);

  r = send(sd, &num, sizeof(int), 0);
  if(r == -1){
    perror("Send error\n");
    exit(-1);
  }

	switch(num){
		case 1:
			printf("Ingresar registro\n");
			addReg(sd);
			break;
		case 2:
			printf("Ver registro\n");
			seeReg(sd);
			break;
		case 3:
			printf("Borrar registro\n");
			deleteReg(sd);
			break;
		case 4:
			printf("Buscar registro\n");
			searchReg(sd);
			break;
		case 5:
			printf("Salir\n");
			break;
		default:
			printf("default\n");
			break;
	}
}

//Función que reejecuta el menú
void exeMenu(int sd){
  static struct termios newt, oldt;
  printf("\nPresione una tecla para continuar...");
  tcgetattr( STDIN_FILENO, &oldt);  //Obteniendo la configuracion actual de la terminal
  newt = oldt;        //Copiando la configuracion
	//	Desactivando las banderas ECHO e ICANON
	newt.c_lflag &= ~(ICANON | ECHO);  //Desactivando la bandera ICANON para no esperar un caracter especial al final
  newt.c_cc[VMIN] = 1;
  tcsetattr( STDIN_FILENO, TCSANOW, &newt);//Configurando la terminal con los nuevos cambios
	__fpurge(stdin);      //Limpiando el buffer de entrada
  getchar();        //Esperando un caracter
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);//Configurando la terminal a su forma original
	printf("\n");
	menu(sd);
}

// Función que añade un registro a al archivo
void addReg(int sd){
	char term;
	char petName[NAME_SIZE];
	char type[TYPE_SIZE];
	char breed[BREED_SIZE];
	char sex;
	int age;
	int height;
	float weight;

	int rightValue = 0;
	int next;
	int code = 0;
	int i;

	//Captura de los datos de la mascota
	printf("%s", "Ingrese el nombre de la mascota, este no debe superar los 32 caracteres,\nsi lo hace solo se guardaran los primeros 32: ");
	getchar();
	fgets(petName, NAME_SIZE, stdin);
	petName[strlen(petName)-1]=0;
	printf("Nombre ingresado: %s\n", petName);

	printf("%s", "Ingrese el tipo de la mascota, este no debe superar los 32 caracteres \nsi lo hace solo se guardaran los primeros 32: ");
	fgets(type, NAME_SIZE, stdin);
	type[strlen(type)-1]=0;
	printf("Tipo ingresado: %s\n", type);

	printf("%s","Ingrese la raza de la mascota, este no debe superar los 32 caracteres, \nsi lo hace solo se guardaran los primeros 32: ");
	fgets(breed, NAME_SIZE, stdin);
	breed[strlen(breed)-1]=0;
	printf("Raza ingresada: %s\n", breed);
	printf("%s", "Ingrese el sexo 'H' para Hembra o 'M' para Macho: ");
	fflush(stdin);
	scanf(" %c", &sex);
	while(sex != 'H' && sex != 'M' && sex != 'm' && sex != 'h'){
		printf("%s\n", "Ingrese el sexo 'H' o 'M'");
		scanf(" %c", &sex);
	}
	printf("Sexo ingresado: %c\n", sex);

	printf("Ingrese la edad de la mascota: ");
	do{
		getchar();
		if(scanf("%d%c", &age, &term) != 2 || term != '\n')
		    printf("Valor no valido, intente de nuevo: ");
		else
			rightValue = 1;
	}while(rightValue == 0);
	printf("La edad introducida es: %d\n", age);

	printf("Ingrese la altura de la mascota (En cm): ");
	rightValue = 0;
	do{
		if(scanf(" %d%c", &height, &term) != 2 || term != '\n')
				printf("Valor no valido, intente de nuevo. ");
		else
			rightValue = 1;
	}while(rightValue == 0);
	printf("La altura introducida es: %d\n", height);

	printf("Ingrese el peso de la mascota (En kg): ");
	rightValue = 0;
	do{
		if(scanf(" %f%c", &weight, &term) != 2 || term != '\n')
		    printf("Valor no valido, intente de nuevo. ");
		else
			rightValue = 1;
	}while(rightValue == 0);
	printf("El peso introducido es: %f\n", weight);

	//Creación de la estructura
	struct dogType* newDog = malloc(sizeof(struct dogType));
	strcpy(newDog->name, petName);
	strcpy(newDog->type, type);
	strcpy(newDog->breed, breed);
	newDog->age = age;
	newDog->height = height;
	newDog->weight = weight;
	newDog->gender = sex;
	newDog->next = 0;
	//Muestra la estructura creada
	showDogType(newDog);

  int r = send(sd, newDog, sizeof(struct dogType), 0);
  if(r == -1){
    perror("Send error\n");
    exit(-1);
  }
	//Libera el espacio en memoria de la estructura cierra el archivo dataDogs.dat y reejecuta el menú
	free(newDog);
	// printf("Registro añadido exitosamente.\n");
	exeMenu(sd);
}
//	Función que permite ver un registro de dataDogs.dat
void seeReg(int sd){
	int numberReg;
	int value = 0;
	int totalSize;
	char ans;


  int r = recv( sd, &totalSize, sizeof(int), 0);
  if(r != sizeof(int)){
    perror("Recv error");
    exit(-1);
  }
	printf("El número de registros es de: %i\n",totalSize);
  // El ciclo no es reactivo
	do{
		printf("%s\n", "Ingrese el número del registro a consultar");
		scanf("%d", &numberReg);
		//Si el numero ingresado se sale del rango de registros el registro solicitado no existe
		if(numberReg < 1 || numberReg > totalSize)
			printf("%s\n", "Este registro no existe");
		else{
			numberReg -= 1;
      r = send(sd, &numberReg, sizeof(int), 0);
      if(r == -1){
        perror("Send error\n");
        exit(-1);
      }
			//Imprime el registro solicitado
			value = 1;
			struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
      r = recv(sd, newDog, sizeof(struct dogType), 0);
      if(r != sizeof(struct dogType)){
        perror("Recv error");
        exit(-1);
      }
      showDogType(newDog);
      free(newDog);
      printf("Consulta de registro exitosa\n");
			exeMenu(sd);
		}
	}while (value == 0);
}
// //	Función que elimina un registro de datadogs.dat
void deleteReg(int sd){
  int totalSize;
  int value = 0;

	int i = 0;
	int filePointer = 0;
	int code = 0;
	int delReg = 0;
	int success = 0;

  int r = recv( sd, &totalSize, sizeof(int), 0);
  if(r != sizeof(int)){
    perror("Recv error");
    exit(-1);
  }
  printf("El número de registros es de: %i\n",totalSize);

  do{
    printf("%s\n", "Ingrese la posición del registro a eliminar");
  	scanf("%i", &delReg);
  	if(delReg < 1 || delReg > totalSize){
  		printf("%s\n", "El registro que desea eliminar no existe");
  	}else{
      value = 1;
  		delReg-=1;
      r = send(sd, &delReg, sizeof(int), 0);
      if(r == -1){
        perror("Send error\n");
        exit(-1);
      }

      struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType));
      r = recv(sd, currDog, sizeof(struct dogType), 0);
      if(r != sizeof(struct dogType)){
        perror("Recv error");
        exit(-1);
      }
      showDogType(currDog);
      free(currDog);
  		printf("%s\n", "Eliminación exitosa... espere por favor");
  	}
  }while(value == 0);
	exeMenu(sd);
}
//	Función que busca en dataDogs.dat las mascotas con el mismo nombre
void searchReg(int sd){
	char petName[NAME_SIZE];
  printf("Ingrese el nombre de la mascota, este no debe superar los 32 caracteres, si lo hace solo se guardaran los primeros 32: ");
	getchar();
	fgets(petName, NAME_SIZE, stdin);
	petName[strlen(petName)-1]=0;

  int r = send(sd, petName, NAME_SIZE, 0);
  if(r == -1){
    perror("Send error\n");
    exit(-1);
  }

  int exists = 0;
  r = recv(sd, &exists, sizeof(int), 0);
  if(r != sizeof(int)){
    perror("Recv error");
    exit(-1);
  }
  printf("exists: %i\n", exists);
  if(!exists){
		printf("%s\n", "Este registro no existe");
    exeMenu(sd);
    return;
  }

  int hasDog = 0;
  struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
  do{
    r = recv(sd, &hasDog, sizeof(int), 0);
    if(r != sizeof(int)){
      perror("Recv error");
      exit(-1);
    }
    if (hasDog){
      exists = 1;
      if (hasDog == 2)
        showDogTypeTableHead();

      r = recv(sd, newDog, sizeof(struct dogType), 0);
      if(r != sizeof(struct dogType)){
        perror("Recv error");
        exit(-1);
      }
      showDogTypeTable(newDog);
    }
  }while(hasDog);

  if(!exists){
		printf("%s\n", "Este registro no existe");
  }

  printf("%s\n", "Busqueda exitosa");
  free(newDog);
	exeMenu(sd);
}
