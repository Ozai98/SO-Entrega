#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
// Constantes de tamaño
#define NAME_SIZE 32
#define TYPE_SIZE 32
#define BREED_SIZE 16
#define STRUCTURES_NUMBER 1e+7
#define HASH_TABLE_SIZE 2500
// Constantes de nombre de archivo
#define NOMBRES_MASCOTAS_PATH "nombresMascotas.txt"
#define PET_NAMES_PATH "petNames.dat"
#define DATA_DOGS_PATH "dataDogs.dat"
#define CURRENT_ID_PATH "currentId.dat"
#define TEMP_DATA_DOGS_PATH "tempDataDogs.dat"

//Declaración de la estructura dogType
struct dogType{
	int next; // Entero que contiene la referencia a la siguiente estructura con el mismo nombre dentro de dataDogs.dat
	int position; // Posición de la estructura en dataDogs.dat
	unsigned int id; // ID de la estructura
	char name[NAME_SIZE]; //Nombre de la estructura
	char type[TYPE_SIZE]; // Tipo de mascota de la estructura
	unsigned short age; // Edad de la mascota de la estructura
	char breed[BREED_SIZE]; // Raza de la mascota de la estructura
	unsigned short height; // Altura de la mascota de la estructura
	float weight; // Peso de la mascota de la estructura
	char gender; // Sexo de la mascota de la estructura
};

//Función que imprime una estructura mascota sin next
void showDogType(void *p){
	struct dogType *dog = p;
	printf("Id:\t%u\n",dog->id);
  printf("Posición:\t%u\n",dog->position);
  printf("Nombre:\t%s\n",dog->name);
  printf("Tipo:\t%s\n",dog->type);
  printf("Raza:\t%s\n",dog->breed);
	printf("Edad:\t%hu\n",dog->age);
	printf("Altura:\t%hu\n",dog->height);
  printf("Peso:\t%2.2f\n",dog->weight);
	printf("Genero:\t%c\n",dog->gender);
}

//Función que imprime la cabecera de la tabla que muestra los nombres que coinciden con el campo introducido en la función de busqueda de un registro
void showDogTypeTableHead(){
	printf("\tID\tPOSICIÓN\tEDAD\tALTURA\tPESO\tGENERO\tNOMBRE\t\t\t\tTIPO\t\t\t\tRAZA\tNEXT\n");
}

//Función que imprime la tabla que muestra los nombres que coinciden con el campo introducido en la función de busqueda de un registro
void showDogTypeTable(void* p){
	struct dogType* dog = p;
	char auxName[NAME_SIZE/8] = "";
	char auxType[TYPE_SIZE/8] = "";
	char auxBreed[BREED_SIZE/8] = "";
	int i=0;
	int cantTab = 0;
	char tab[1] = "\t";

	cantTab = NAME_SIZE/8 - strlen(dog->name)/8;
	for(i=0; i<cantTab; i++)
		strcat(auxName, tab);
	cantTab = TYPE_SIZE/8 - strlen(dog->type)/8;
	for(i=0; i<cantTab; i++)
		strcat(auxType, tab);
	cantTab = BREED_SIZE/8 - strlen(dog->breed)/8;
	for(i=0; i<cantTab; i++)
		strcat(auxBreed, tab);

	printf("%10i\t%10i\t%3hu\t%3hu\t%2.2f\t%c\t%s%s%s%s%s\t%i\n",dog->id, dog->position, dog->age, dog->height, dog->weight, dog->gender, dog->name, auxName, dog->type, auxType, dog->breed, dog->next);
}

//Funcion que abre un archivo y captura errores
FILE* checkfopen(const char *path, const char *mode){
  FILE* fd = fopen(path, mode);
  if(fd == NULL){
    char message[250] = "error fopen ";
    strcat(message,path);
    strcat(message," ");
    strcat(message,mode);
    perror(message);
    exit(-1);
  }
  return fd;
}
//Funcion que cierra un archivo y captura errores
int checkfclose(FILE* stream, char* path){
  int ret = fclose(stream);
  if(ret != 0){
    char message[250] = "error fclose ";
    strcat(message,path);
    perror(message);
    exit(-1);
  }
}
//Función que envía un dato y captura errores
ssize_t checkSend(int socketsd, const void *buf, size_t len, int flags, char* message){
	ssize_t remain_data = len, r;
	while((r = send(socketsd, buf+len-remain_data, len, 0)) != remain_data){
		if(r == -1){
			char messageRet[250];
      sprintf(messageRet, "send error %lu of %lu bytes sent of %s\n", len-remain_data, len, message);
			perror(messageRet);
			exit(-1);
		}
		remain_data -= r;
	}
	return remain_data;
}

//Función que recibe un dato y captura errores
ssize_t checkRecv(int sockfd, void *buf, size_t len, int flags, char* message){
	ssize_t remain_data = len, r;
	while((r = recv(sockfd, buf+len-remain_data, len, 0)) != remain_data){
		if(r == -1){
			char messageRet[250];
      sprintf(messageRet, "recv error %lu of %lu bytes received of %s\n", len-remain_data, len, message);
			perror(messageRet);
			exit(-1);
		}
		remain_data -= r;
	}
	return remain_data;
}
