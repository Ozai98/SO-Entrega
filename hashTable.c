#include "linkedList.c"
#include "atributes.c"

void htInit(struct List* hashTable[HASH_TABLE_SIZE]){
	int i=0;
	for(i; i<HASH_TABLE_SIZE; i++){
		hashTable[i] = NULL;
	}
}

int htHashFunction(char* str){
	int sum = 0;
	int i=0;
	for(i=0; i<NAME_SIZE; i++){
		// printf("%c\n", str[i]);
		if(str[i] == 0)
			break;
		sum += tolower(str[i])*(i+1);
		// sum %= HASH_TABLE_SIZE;
	}
	return sum%HASH_TABLE_SIZE;
}


void htLoad(struct List* hashTable[HASH_TABLE_SIZE]){
	long filePointer = 0;
	int code = 0;
	int i = 0;
	struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType));
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
	fseek(dataDogs, 0, SEEK_END);
	long numberOfStructures = ftell(dataDogs)/sizeof(struct dogType);
	printf("number of structures: %ld\n", numberOfStructures);
	rewind(dataDogs);

	for(i; i<numberOfStructures; i++){
		filePointer = ftell(dataDogs);
		fread(currDog, sizeof(struct dogType), 1, dataDogs);
		code = htHashFunction(currDog->name);
		if(hashTable[code] == NULL){
			hashTable[code] = (struct List*)malloc(sizeof(struct List));
			dllInit(hashTable[code]);
		}
		printf("Code: %i id: %i Pointer: %ld Pos: %lu Name: %s \n", code, currDog->id, (long)filePointer, filePointer/sizeof(struct dogType), currDog->name);
		dllAddHead(hashTable[code], filePointer);
	}
	checkfclose(dataDogs, DATA_DOGS_PATH);
	free(currDog);
}

void htPrintAll(struct List* hashTable[HASH_TABLE_SIZE]){
	int i = 0;
	for(i; i < HASH_TABLE_SIZE; i++){
		if(hashTable[i] != NULL)
			dllPrintAll(hashTable[i]);
	}
}

void htFree(struct List* hashTable[HASH_TABLE_SIZE]){
  int i;
  for(i=0; i<HASH_TABLE_SIZE; i++){
    dllFree(hashTable[i]);
  }
}

void htSearch(struct List* hashTable[HASH_TABLE_SIZE], char* name){
	int i = 0;
	int code;
	int quit = 0;
	code = htHashFunction(name);
	// printf("%lu\n", (long)hashTable[code]);
	// printf("%i\n", dllIsEmpty(hashTable[code]));


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
			if(!strcmp(dogNameAux, nameAux))
				showDogType(newDog);
			dllNext(hashTable[code]);
		}
		fseek(dataDogs, dllGetCurrData(hashTable[code]), SEEK_SET);
		fread(newDog, sizeof(struct dogType), 1, dataDogs);
		strcpy(dogNameAux, newDog->name);
		for(i = 0; dogNameAux[i]!=0; i++){
			dogNameAux[i] = tolower(dogNameAux[i]);
		}
		if(!strcmp(dogNameAux, nameAux))
			showDogType(newDog);
		free(newDog);
		checkfclose(dataDogs, DATA_DOGS_PATH);
	}
}

void htAdd(struct List* hashTable[HASH_TABLE_SIZE], char* name, long pos){
	int code;
	code = htHashFunction(name);
	if(hashTable[code] == NULL){
		hashTable[code] = (struct List*)malloc(sizeof(struct List));
		dllInit(hashTable[code]);
	}
	dllAddHead(hashTable[code], pos);
}

// int main(){
// 	struct List** hashTable = (struct List**)malloc(sizeof(struct List*)*HASH_TABLE_SIZE);
// 	htInit(hashTable);
// 	htPrintAll(hashTable);
// 	htLoad(hashTable);
// 	htPrintAll(hashTable);
// 	htAdd(hashTable,"Tysson",120);
// 	htPrintAll(hashTable);
// 	htSearch(hashTable, "Tysson");
// 	//htSearch(hashTable,"Banana");
// 	//htFree(hashTable);
// 	// confirmar con htSearch
// 	return 0;
// }
