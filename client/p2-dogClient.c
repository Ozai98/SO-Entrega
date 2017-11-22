#include "atributes.c"
#include <unistd.h>
#include <ctype.h>
#include <termios.h>
#include <stdio_ext.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#define PORT 3535

// Declaración de las funciones
void menu(int sd);
void exeMenu(int sd);
void addReg(int sd);
void seeReg(int sd);
void deleteReg(int sd);
void searchReg(int sd);

int main(int argc, char *argv[]){
  int sd = socket(AF_INET, SOCK_STREAM, 0); // Crea una socket
  int optval;
  if(sd == -1){
    perror("Socket error\n");
    exit(-1);
  }
  struct sockaddr_in server;
  socklen_t serverSize;

  // Asigna el nombre a la socket
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = inet_addr(argv[1]);
  bzero(server.sin_zero, 8);

  serverSize = sizeof(struct sockaddr);
  optval = 1; // Configura la reutilización del puerto de comunicación
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");

  int r = connect(sd, (struct sockaddr*)&server, serverSize); // Se conecta con el servidor
  if(r == -1){
    perror("Connect error");
    exit(-1);
  }

	menu(sd); // Ejecuta el menu
  close(sd); // Cierra la socket
	return 0;
}

void menu(int sd){ // Menú que despliega las opciones
	char input[32];
	int num = 0, r;
	printf("*--------------------------------------------------*\n");
	printf("*\t\tTienda de mascotas                            *\n");
	printf("*--------------------------------------------------*\n");
	printf("*\t1\tIngresar registro                            *\n");
	printf("*\t2\tVer registro                                 *\n");
	printf("*\t3\tBorrar registro                              *\n");
	printf("*\t4\tBuscar registro                              *\n");
	printf("*\t5\tSalir                                        *\n");
	printf("*--------------------------------------------------*\n");

	/*En caso de no ingresar un numero dentro de las opciones expuestas,
	  se volverá a solicitar el ingreso de la opcion*/
	do{
		printf("Ingrese la opcion deseada: ");
		scanf("%32s", input);
    printf("Opcion %s: ", input);
		num = input[0]-48;

		if (strlen(input) != 1 || num < 1 || num > 5)
			printf("%s\n", "El valor ingresado no es valido, intente de nuevo");

	}while(strlen(input) != 1 || num < 1 || num > 5);

  checkSend(sd, &num, sizeof(int), 0, "num"); // Envía la opción seleccionada al servidor
  switch(num){ //Ejecuta la opción deseada
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
			searchReg(sd);system("clear");
			break;
		case 5:
			printf("Salir\n");
			break;
		default:
			printf("default\n");
			break;
	}
}

void exeMenu(int sd){ // Función que reejecuta el menú
  static struct termios newt, oldt;
  printf("\t\nPresione una tecla para continuar...");
  tcgetattr( STDIN_FILENO, &oldt);  // Obteniendo la configuracion actual de la terminal
  newt = oldt;        // Copiando la configuracion
	//	Desactivando las banderas ECHO e ICANON
	newt.c_lflag &= ~(ICANON | ECHO);  // Desactivando la bandera ICANON para no esperar un caracter especial al final
  newt.c_cc[VMIN] = 1;
  tcsetattr( STDIN_FILENO, TCSANOW, &newt);// Configurando la terminal con los nuevos cambios
	__fpurge(stdin);      // Limpiando el buffer de entrada
  getchar();        // Esperando un caracter
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);// Configurando la terminal a su forma original
	printf("\n");
  system("clear");
	menu(sd);
}

void addReg(int sd){ // Función que añade un registro a al archivo
	char term; // Inicializando variables de adición
	char petName[NAME_SIZE];
	char type[TYPE_SIZE];
	char breed[BREED_SIZE];
	char sex;
	int age;
	int height;
	float weight;

	int rightValue = 0, next, code = 0, i;

	// Captura de los datos de la mascota
	printf("\t%s", "Ingrese el nombre de la mascota, este no debe superar los 32 caracteres,\nsi lo hace solo se guardaran los primeros 32: "); // Captura del nombre
	getchar();
	fgets(petName, NAME_SIZE, stdin);
	petName[strlen(petName)-1]=0;
	printf("\tNombre ingresado: %s\n", petName);

	printf("\t%s", "Ingrese el tipo de la mascota, este no debe superar los 32 caracteres \nsi lo hace solo se guardaran los primeros 32: "); // Captura del tipo
	fgets(type, NAME_SIZE, stdin);
	type[strlen(type)-1]=0;
	printf("\tTipo ingresado: %s\n", type);

	printf("\t%s","Ingrese la raza de la mascota, este no debe superar los 32 caracteres, \nsi lo hace solo se guardaran los primeros 32: "); // Captura de la raza
	fgets(breed, NAME_SIZE, stdin);
	breed[strlen(breed)-1]=0;
	printf("\tRaza ingresada: %s\n", breed);

	printf("\t%s", "Ingrese el sexo 'H' para Hembra o 'M' para Macho: "); // Captura del sexo
	fflush(stdin);
	scanf(" %c", &sex);
	while(sex != 'H' && sex != 'M' && sex != 'm' && sex != 'h'){
		printf("\t%s\n", "Ingrese el sexo 'H' o 'M'");
		scanf(" %c", &sex);
	}
	printf("\tSexo ingresado: %c\n", sex);

	printf("\tIngrese la edad de la mascota: "); // Captura de la edad
	do{
		getchar();
		if(scanf("%d%c", &age, &term) != 2 || term != '\n')
		    printf("\tValor no valido, intente de nuevo: ");
		else
			rightValue = 1;
	}while(rightValue == 0);
	printf("\tLa edad introducida es: %d\n", age);

	printf("\tIngrese la altura de la mascota (En cm): "); // Captura de la altura
	rightValue = 0;
	do{
		if(scanf(" %d%c", &height, &term) != 2 || term != '\n')
				printf("\tValor no valido, intente de nuevo. ");
		else
			rightValue = 1;
	}while(rightValue == 0);
	printf("\tLa altura introducida es: %d\n", height);

	printf("\tIngrese el peso de la mascota (En kg): "); // Captura del peso
	rightValue = 0;
	do{
		if(scanf(" %f%c", &weight, &term) != 2 || term != '\n')
		    printf("\tValor no valido, intente de nuevo. ");
		else
			rightValue = 1;
	}while(rightValue == 0);
	printf("\tEl peso introducido es: %f\n", weight);

	// Creación de la estructura
	struct dogType* newDog = malloc(sizeof(struct dogType));
	strcpy(newDog->name, petName);
	strcpy(newDog->type, type);
	strcpy(newDog->breed, breed);
	newDog->age = age;
	newDog->height = height;
	newDog->weight = weight;
	newDog->gender = sex;
	newDog->next = 0;
  newDog->id = 0;
  newDog->position = 0;
	showDogType(newDog); // Muestra la estructura creada
  checkSend(sd, newDog, sizeof(struct dogType), 0, "newDog"); // Envia la estructura creada al servidor
	free(newDog); // Libera el espacio en memoria de la estructura y reejecuta el menú
	exeMenu(sd);
}

void seeReg(int sd){ //	Función que permite ver un registro de dataDogs.dat
  //Inicializa variables de visualiazación
	int numberReg;
	int value = 0;
	int totalSize;
	char ans;
  int resp;
  char ch;

  checkRecv( sd, &totalSize, sizeof(int), MSG_WAITALL, "totalSize"); // Recibe el tamaño del archivo dataDogs desde el servidor
  printf("\tEl número de registros es de: %i\n",totalSize);
	do{
		printf("\t%s\n", "Ingrese el número del registro a consultar");
		scanf("%d", &numberReg);
		if(numberReg < 1 || numberReg > totalSize){ // Si el numero ingresado se sale del rango de registros el registro solicitado no existe
			printf("\t%s\n", "Este registro no existe");
      scanf(" %c", &ch);
		}else{
			numberReg -= 1;
      checkSend(sd, &numberReg, sizeof(int), 0, "numberReg"); // Envía al servidor el número del registro a mostrar
			value = 1;
			struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType)); // Reserva espacio para la estructura temporal
      checkRecv(sd, newDog, sizeof(struct dogType), MSG_WAITALL, "newDog"); //Recibe el registro a mostrar desde el servidor
      showDogType(newDog); // Imprime el registro solicitado
      free(newDog); // Libera la estructura temporal
      printf("\tConsulta de registro exitosa\n");

      printf("\t%s\n", "Desea abrir la historia clínica del registro seleccionado Escriba S o N");
			scanf(" %c", &ans);

      checkSend(sd, &ans, sizeof(char), 0, "ans"); // Envía la respuesta al servidor
			if(ans == 's' || ans == 'S'){ // Si la respuesta es afirmativa, abre la historia clinima
        checkRecv(sd, &resp, sizeof(int), MSG_WAITALL, "resp"); // Recibe del servidor si la historia clínica solicitada, existe y  esta abierta por otro usuario,
                                                        // si existe y no esta abierta, o si no existe

        if(resp == 0){ // Si la historia clínica no existe en el servidor
          int fileSize, len;
          char buffer[BUFSIZ];
          char file_name[12];
          sprintf(file_name, "%d.txt", (numberReg+1));
          FILE* myFile = checkfopen(file_name, "w"); // Crea la historia clínica en el cliente para que la edite
          checkfclose(myFile, file_name);
          sprintf(file_name, "gedit %d.txt", (numberReg+1));
          system(file_name); // Abre la historia clínica para su edición

          sprintf(file_name, "%d.txt", (numberReg+1));
          int fd = open(file_name, O_RDONLY);

	        struct stat file_stat; // Obtiene el tamaño de la nueva historia clinica
          if (fstat(fd, &file_stat) < 0){
    				perror("fstat error\n");
    				exit(-1);
    			}
          fileSize = file_stat.st_size;

          checkSend(sd, &fileSize, sizeof(int), 0, "fileSize"); // Envía el tamaño de la historia clínica al servidor

          off_t offset = 0;
          int sent_bytes = 0;
          int remain_data = fileSize;
          while (((sent_bytes = sendfile(sd, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0)){ // Envía la historia clínica al servidor
            remain_data -= sent_bytes;
          }
			    close(fd);
          remove(file_name); // Elimina la historia clínica del cliente
        }

        else if(resp == 1){ //  Si la historia clínica existe en el servidor
          int fileSize, len;
          char buffer[BUFSIZ];
          char file_name[12];
          sprintf(file_name, "%d.txt", (numberReg+1));
          FILE* myFile = checkfopen(file_name, "w");

          checkRecv(sd, &fileSize, sizeof(int), MSG_WAITALL, "fileSize"); // Recibe del servidor el tamaño de la historia clínica

          int remain_data = fileSize;
          while ((remain_data > 0) && ((len = recv(sd, buffer, BUFSIZ, 0)) > 0)){ // Recibe la historia clínica desde el servidor
            fwrite(buffer, sizeof(char), len, myFile);
            remain_data -= len;
          }
          checkfclose(myFile, file_name);

          sprintf(file_name, "gedit %d.txt", (numberReg+1));
          system(file_name); // Abre la historia para su edición
          sprintf(file_name, "%d.txt", (numberReg+1));
          int fd = open(file_name, O_RDONLY);

	        struct stat file_stat; // Calcula el tamaño de la historia clínica editada
          if (fstat(fd, &file_stat) < 0){
    				perror("fstat error\n");
    				exit(-1);
    			}

          fileSize = file_stat.st_size;

          checkSend(sd, &fileSize, sizeof(int), 0, "fileSize"); // Envía el nuevo tamaño al servidor
      		fileSize = (int)file_stat.st_size;
          off_t offset = 0;
          int sent_bytes = 0;
          remain_data = fileSize;
          while (((sent_bytes = sendfile(sd, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0)){ // Envía la historia clínica editada al servidor
            remain_data -= sent_bytes;
          }
			    close(fd);
          remove(file_name); // Elimina la historia clínica del cliente
        }

        else if(resp == 2){ //  Si el archivo esta abierto por otro usuario
          printf("\tArchivo en uso, intente más tarde\n");
        }
			}
			exeMenu(sd);
		}
	}while (value == 0);
}

void deleteReg(int sd){ // Función que elimina un registro de datadogs.dat
  int totalSize, value = 0, i = 0, filePointer = 0, code = 0, delReg = 0, success = 0; // Variables de eliminación
  checkRecv( sd, &totalSize, sizeof(int), MSG_WAITALL, "totalSize"); // Recibe el tamaño de dataDogs desde el servidor
  printf("\tEl número de registros es de: %i\n",totalSize);

  do{
    printf("\t%s\n", "Ingrese la posición del registro a eliminar");
  	scanf("%i", &delReg);
  	if(delReg < 1 || delReg > totalSize){ // Si el número ingresado se sale del rango de dataDogs el registro no existe
  		printf("\t%s\n", "El registro que desea eliminar no existe");
  	}else{
      value = 1;
  		delReg-=1;
      checkSend(sd, &delReg, sizeof(int), 0, "delReg"); // Envía el número del registro a eliminar al servidor
      struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType)); // Reserva el espacio para la estructura auxiliar
      checkRecv(sd, currDog, sizeof(struct dogType), MSG_WAITALL, "currDog"); // Recibe la estructura a eliminar
      showDogType(currDog); // Muestra la estructura a eliminar
      free(currDog);
      printf("\t%s\n","Espere por favor");
      checkRecv(sd, &success, sizeof(int), MSG_WAITALL, "success"); // Recibe la confirmación de que el proceso de eliminación en el servidor ha terminado
      printf("\t%s\n", "Eliminación exitosa");
  	}
  }while(value == 0);
	exeMenu(sd);
}

void searchReg(int sd){ // Función que busca en dataDogs.dat las mascotas con el mismo nombre
	char petName[NAME_SIZE];
  printf("\tIngrese el nombre de la mascota, este no debe superar los 32 caracteres, si lo hace solo se guardaran los primeros 32: "); // Recibe el nombre del registro a buscar
	getchar();
	fgets(petName, NAME_SIZE, stdin);
	petName[strlen(petName)-1]=0;
  checkSend(sd, petName, NAME_SIZE, 0, "petName"); // Envía al servidor el nombre del registro a buscar
  int exists = 0;
  checkRecv(sd, &exists, sizeof(int), MSG_WAITALL, "exists"); // Recibe del servidor si el registro existe o no en dataDogs

  if(!exists){
		printf("\t%s\n", "Este registro no existe");
    exeMenu(sd);
    return;
  }

  int hasDog = 0;
  struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
  int something;
  int r2;
  do{
    checkRecv(sd, &hasDog, sizeof(int), MSG_WAITALL, "hasDog"); //Recibe
    if (hasDog){
      exists = 1;
      if (hasDog == 2)
        showDogTypeTableHead();
      something = sizeof(struct dogType);
      checkRecv(sd, newDog, sizeof(struct dogType), MSG_WAITALL, "newDog"); //Recibe la estructura que tiene el nombre buscadi
      showDogTypeTable(newDog);
    }
  }while(hasDog); // Mientras hayan coincidencias en el nombre

  if(!exists){
		printf("\t%s\n", "Este registro no existe");
  }

  printf("\t%s\n", "Busqueda exitosa");
  free(newDog);
	exeMenu(sd);
}
