#include "linkedList.c"
#include "atributes.c"
//Inicializa la hashTable
void htInit(struct List* hashTable[HASH_TABLE_SIZE]){
	int i=0;
	for(i; i<HASH_TABLE_SIZE; i++){
		hashTable[i] = NULL;
	}
}
//Le calcula la función hash a str
int htHashFunction(char* str){
	int sum = 0;
	int i=0;
	for(i=0; i<NAME_SIZE; i++){
		if(str[i] == 0)
			break;
		sum += tolower(str[i])*(i+1);
	}
	return sum%HASH_TABLE_SIZE;
}
//Carga la hashTable con los registros de dataDogs.dat
void htLoad(struct List* hashTable[HASH_TABLE_SIZE]){
	long filePointer = 0;
	int code = 0;
	int i = 0;
	struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType));
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
	fseek(dataDogs, 0, SEEK_END);
	long numberOfStructures = ftell(dataDogs)/sizeof(struct dogType);
	rewind(dataDogs);

	for(i; i<numberOfStructures; i++){
		filePointer = ftell(dataDogs);
		fread(currDog, sizeof(struct dogType), 1, dataDogs);
		code = htHashFunction(currDog->name);
		if(hashTable[code] == NULL){
			hashTable[code] = (struct List*)malloc(sizeof(struct List));
			dllInit(hashTable[code]);
		}
		dllAddHead(hashTable[code], filePointer);
	}
	checkfclose(dataDogs, DATA_DOGS_PATH);
	free(currDog);
}
//Imprime todos los valores de la hashTable
void htPrintAll(struct List* hashTable[HASH_TABLE_SIZE]){
	int i = 0;
	for(i; i < HASH_TABLE_SIZE; i++){
		if(hashTable[i] != NULL)
			dllPrintAll(hashTable[i]);
	}
}
//Libera el espacio de memoria ocupado por la hashTable
void htFree(struct List* hashTable[HASH_TABLE_SIZE]){
  int i;
  for(i=0; i<HASH_TABLE_SIZE; i++){
		if(hashTable[i] = NULL)
    	dllFree(hashTable[i]);
  }
	free(hashTable);
}
//Busca un valor en la hashTable
int htSearch(struct List* hashTable[HASH_TABLE_SIZE], char* name){
	int success = 0;
	int i = 0;
	int quit = 0;
	int code;
	code = htHashFunction(name);

	if(hashTable[code] == NULL)
		printf("%s\n","Mascota no existe");
	else{
		// Siempre tendrá al menos un Valor
		FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
		struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
		dllRewind(hashTable[code]);
		char dogNameAux[NAME_SIZE];
		char nameAux[NAME_SIZE];
		strcpy(nameAux, name);

		for(i = 0; nameAux[i]!=0; i++){
			nameAux[i] = tolower(nameAux[i]);
		}
		while(dllHasNext(hashTable[code])){
			fseek(dataDogs, dllGetCurrData(hashTable[code]), SEEK_SET);
			fread(newDog, sizeof(struct dogType), 1, dataDogs);
			strcpy(dogNameAux, newDog->name);

			for(i = 0; dogNameAux[i]!=0; i++){
				dogNameAux[i] = tolower(dogNameAux[i]);
			}
			if(!strcmp(dogNameAux, nameAux)){
				if(success==0)
					showDogTypeTableHead();
				showDogTypeTable(newDog);
				success = 1;
			}
			dllNext(hashTable[code]);
		}
		fseek(dataDogs, dllGetCurrData(hashTable[code]), SEEK_SET);
		fread(newDog, sizeof(struct dogType), 1, dataDogs);
		strcpy(dogNameAux, newDog->name);

		for(i = 0; dogNameAux[i]!=0; i++){
			dogNameAux[i] = tolower(dogNameAux[i]);
		}
		if(!strcmp(dogNameAux, nameAux)){
			if(success==0)
				showDogTypeTableHead();
			showDogTypeTable(newDog);
			success = 1;
		}
		free(newDog);
		checkfclose(dataDogs, DATA_DOGS_PATH);
	}
	return success;
}
//Añade un valor a la hashTable
void htAdd(struct List* hashTable[HASH_TABLE_SIZE], char* name, long pos){
	int code;
	code = htHashFunction(name);
	if(hashTable[code] == NULL){
		hashTable[code] = (struct List*)malloc(sizeof(struct List));
		dllInit(hashTable[code]);
	}
	dllAddHead(hashTable[code], pos);
}
