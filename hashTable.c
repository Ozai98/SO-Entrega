// #include "linkedList.c"
#include "atributes.c"
//Inicializa la hashTable
void htInit(int* hashTable){
	int i=0;
	for(i=0; i<HASH_TABLE_SIZE; i++)
		hashTable[i] = -1;
}
//Le calcula la función hash a str
int htHashFunction(char* str){
	int sum = 0;
	int i=0;
	for(i=0; i<strlen(str); i++){
		sum += tolower(str[i])*(i+1);
		sum%=HASH_TABLE_SIZE;
	}
	return sum;
}
//Carga la hashTable con los registros de dataDogs.dat
void htLoad(int* hashTable){
	int filePointer = 0;
	int code = 0;
	int i = 0;

	struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType));
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
	fseek(dataDogs, 0, SEEK_END);
	int nStructures = ftell(dataDogs)/sizeof(struct dogType);
	rewind(dataDogs);
	int count=0;
	for(i=0; i<nStructures; i++){
		filePointer = ftell(dataDogs);
		fread(currDog, sizeof(struct dogType), 1, dataDogs);
		code = htHashFunction(currDog->name);
		if(hashTable[code] == -1){
			count++;
			hashTable[code] = filePointer;
		}
	}
	printf("%i\n", count);
	checkfclose(dataDogs, DATA_DOGS_PATH);
	free(currDog);
}
//Imprime todos los valores de la hashTable
void htPrintAll(int* hashTable){
	int i = 0;
	for(i; i < HASH_TABLE_SIZE; i++){
		printf("%i\n", hashTable[i]);
	}
}
//Busca un valor en la hashTable
int htSearch(int* hashTable, char* name){
	int success = 0;
	int i = 0;
	int code = htHashFunction(name);
	int next = 0;

	if(hashTable[code] == -1)
		printf("%s\n","Mascota no existe");
	else{
		// Siempre tendrá al menos un Valor
		FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
		struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
		char dogNameAux[NAME_SIZE];
		char nameAux[NAME_SIZE];
		strcpy(nameAux, name);
		for(i = 0; i<strlen(nameAux); i++){
			nameAux[i] = tolower(nameAux[i]);
		}
		next = hashTable[htHashFunction(name)];
		do{
			fseek(dataDogs, next, SEEK_SET);
			fread(newDog, sizeof(struct dogType), 1, dataDogs);
			strcpy(dogNameAux, newDog->name);

			for(i = 0; i<strlen(dogNameAux); i++){
				dogNameAux[i] = tolower(dogNameAux[i]);
			}
			if(!strcmp(dogNameAux, nameAux)){
				if(success==0){
					showDogTypeTableHead();
					success = 1;
				}
				showDogTypeTable(newDog);
			}
			next = newDog->next;
		}while(next != 0);

		free(newDog);
		checkfclose(dataDogs, DATA_DOGS_PATH);
	}
	return success;
}

// //Añade un valor a la hashTable
// void htAdd(struct List** hashTable, char* name, int pos){
// 	int code;
// 	code = htHashFunction(name);
// 	if(hashTable[code] == NULL){
// 		hashTable[code] = (struct List*)malloc(sizeof(struct List));
// 		dllInit(hashTable[code]);
// 	}
// 	dllAddHead(hashTable[code], pos);
// }
