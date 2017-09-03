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
		if(str[i]==0)
			break;
		sum += (int)str[i]*(i+1);
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
		dllAddHead(hashTable[code], filePointer);
		printf("Code:%i Pointer: %ld Name: %s\n", code, (long)filePointer, currDog->name);
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
	int code;
	code = htHashFunction(name);
	if(hashTable[code] == NULL || dllIsEmpty(hashTable[code]))
		printf("%s\n","Mascota no existe");
	else{
		FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
		struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
		dllRewind(hashTable[code]);
		fseek(dataDogs, dllGetCurrData(hashTable[code]), SEEK_SET);
		fread(newDog, sizeof(struct dogType), 1, dataDogs);
		if(!strcmp(newDog->name, name))
			showDogType(newDog);
		else
			showDogType(newDog);
		while(dllHasNext(hashTable[code])){
			fseek(dataDogs, dllGetCurrData(hashTable[code]), SEEK_SET);
			fread(newDog, sizeof(struct dogType), 1, dataDogs);
			if(!strcmp(newDog->name, name)){
				showDogType(newDog);
			}
			dllNext(hashTable[code]);
		}
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
