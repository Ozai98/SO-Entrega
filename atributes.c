#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<ctype.h>
// Size constants
#define NAME_SIZE 32
#define TYPE_SIZE 32
#define BREED_SIZE 16
#define STRUCTURES_NUMBER 1e+7
// #define STRUCTURES_NUMBER 200
#define HASH_TABLE_SIZE 2500

#define NOMBRES_MASCOTAS_PATH "nombresMascotas.txt"
#define PET_NAMES_PATH "petNames.dat"
#define DATA_DOGS_PATH "dataDogs.dat"
#define CURRENT_ID_PATH "currentId.dat"
#define TEMP_DATA_DOGS_PATH "tempDataDogs.dat"

// Struct declaration
struct dogType{
	unsigned int id;
	char name[NAME_SIZE];
	char type[TYPE_SIZE];
	unsigned short age;
	char breed[BREED_SIZE];
	unsigned short height;
	float weight;
	char gender;
};

void showDogType(void *p){
	struct dogType *dog = p;
  printf("Id:\t%u\n",dog->id);
  printf("Nombre:\t%s\n",dog->name);
  printf("Tipo:\t%s\n",dog->type);
  printf("Raza:\t%s\n",dog->breed);
	printf("Edad:\t%hu\n",dog->age);
	printf("Altura:\t%hu\n",dog->height);
  printf("Peso:\t%2.2f\n",dog->weight);
	printf("Genero:\t%c\n",dog->gender);
}

void showDogTypeTableHead(){
	printf("\tID\tEDAD\tALTURA\tPESO\tGENERO\tNOMBRE\t\t\t\tTIPO\t\t\t\tRAZA\n");
}
void showDogTypeTable(void* p){
	struct dogType* dog = p;
	char auxName[NAME_SIZE/8] = "";
	char auxType[TYPE_SIZE/8] = "";
	char auxBreed[BREED_SIZE/8] = "";
	int i=0;
	int cantTab = 0;
	char tab[1] = "\t";

	char name[NAME_SIZE];
	char type[TYPE_SIZE];
	char breed[BREED_SIZE];
	strcpy(name, dog->name);
	strcpy(type, dog->type);
	strcpy(breed, dog->breed);
	for(i=strlen(name); i<NAME_SIZE; i++)
		name[i]=0;
	for(i=strlen(type); i<NAME_SIZE; i++)
		type[i]=0;
	for(i=strlen(breed); i<NAME_SIZE; i++)
		breed[i]=0;


	cantTab = NAME_SIZE/8 - strlen(name)/8;
	for(i=0; i<cantTab; i++)
		strcat(auxName, tab);
	cantTab = TYPE_SIZE/8 - strlen(type)/8;
	for(i=0; i<cantTab; i++)
		strcat(auxType, tab);

	cantTab = BREED_SIZE/8 - strlen(breed)/8;
	for(i=0; i<cantTab; i++)
		strcat(auxBreed, tab);


	printf("%10i\t%3hu\t%3hu\t%2.2f\t%c\t%s%s%s%s%s\n",dog->id, dog->age, dog->height, dog->weight, dog->gender, dog->name, auxName, dog->type, auxType, dog->breed);
}

//Funciones adicionalesstruct List* hashTable[HASH_TABLE_SIZE]
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
// TODO: Create checkfwrite, checkfread, checkmalloc, checkfree
int checkfclose(FILE* stream, char* path){
  int ret = fclose(stream);
  if(ret != 0){
    char message[250] = "error fclose ";
    strcat(message,path);
    perror(message);
    exit(-1);
  }
}
