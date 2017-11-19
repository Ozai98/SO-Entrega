#include "atributes.c"
#include <unistd.h>

void htInit(int* hashTable);
unsigned long htHashFunction(char* str);
void htLoad(int* hashTable);
int htSearch(int* hashTable, char* name);


void htInit(int* hashTable){// Función que inicializa la hashTable
	int i = 0;
	for(i = 0; i < HASH_TABLE_SIZE; i++)
		hashTable[i] = -1;
}

unsigned long htHashFunction(char *str){
  unsigned long c, hash = 5381;
	int i;
  for(i=0; i<strlen(str); i++){
		c = tolower(*str++);
    hash = (hash << 5) + hash + c; /* hash * 33 + c */
	}
  return hash%HASH_TABLE_SIZE;
}

void htLoad(int* hashTable){// Función que carga la hashTable con los registros de dataDogs.dat
	int i = 0, j = 0, code = 0, filePointer = 0, count = 0;  // Inicializar variables a usar
	char nameAux[NAME_SIZE];
	struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType)); // Reservar espacio para la estructura que cargará las mascotas
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r"); // Abrir dataDogs
	fseek(dataDogs, 0, SEEK_END); // Ubicar el descriptor al final del archivo
	int nStructures = ftell(dataDogs)/sizeof(struct dogType); // Calcular el número de estructuras en datadogs.dat
	rewind(dataDogs); // Reestablecer la posición del descriptor
	htInit(hashTable); // Inicializar la hasTable

	for(i = 0; i < nStructures; i++){ // Carga cada una de las estructuras
		filePointer = ftell(dataDogs); // Guarda el descriptor donde inicia la estrucrura
		fread(currDog, sizeof(struct dogType), 1, dataDogs); // Lee la estructura actual
    strcpy(nameAux, currDog->name);
		code = htHashFunction(nameAux); // Calcula la hash de la estructura actual

		if(hashTable[code] == -1){ // Si la hash de la estructura es la primera ocurrencia, añade este hash a la hashTable
			count++;
			hashTable[code] = filePointer;
		}
	}

	free(currDog); // Libera el espacio de la estructura de carga
	checkfclose(dataDogs, DATA_DOGS_PATH); // Cierra dataDogs
}



int htSearch(int* hashTable, char* name){// Función que busca un valor en la hashTable
	int success = 0, i = 0, j = 0; // Inicializar variables a usar
  char nameAux[NAME_SIZE];
  strcpy(nameAux, name);
	int code = htHashFunction(nameAux);
	int next = 0;

	if(hashTable[code] == -1)
		printf("%s\n","Mascota no existe");
	else{ // Siempre habrá al menos una mascota con ese hashTable
		FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r"); // Abrir dataDogs
		struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType)); // Reservar espacio para la estructura auxiliar
		char dogNameAux[NAME_SIZE]; // Declarar cadenas auxiliares
		char nameAux[NAME_SIZE];
		strcpy(nameAux, name);
		next = hashTable[htHashFunction(name)];
		do{
			fseek(dataDogs, next, SEEK_SET); // Coloca el descriptor al inicio de dataDogs
			fread(newDog, sizeof(struct dogType), 1, dataDogs); // Lee la estructura
			strcpy(dogNameAux, newDog->name);

			for(i = 0; i < strlen(dogNameAux); i++){
				dogNameAux[i] = tolower(dogNameAux[i]);
			}

			if(!strcmp(dogNameAux, nameAux)){ // Compara el nombre de la mascota que tenga el hash de la mascota buscada con el nombre la mascota buscada,
				if(success == 0){								// ya que hay varios nombres con un mismo hash y solo queremos imprimir los del mismo nombre
					showDogTypeTableHead();
					success = 1;
				}
			}
			showDogTypeTable(newDog); // Imprime la mascota encontrada que coincide con el nombre buscado
			next = newDog->next;
		}while(next != 0); // Mientras hayan mascotas en la hashtable

		free(newDog); // Libera la estructura auxiliar
		checkfclose(dataDogs, DATA_DOGS_PATH); // Cierra datadogs
	}
	return success; // Retorna success para indicar que la mascota buscada si existe
}
