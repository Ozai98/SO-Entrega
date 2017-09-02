#include "linkedList.c"
#include "atributes.c"

// struct HashTable{
// 	struct List* hashTable[HASH_TABLE_SIZE];
// 	unsigned long size;
// };
//
// // void htInit(void** p){
// // 	struct List** hashTable = (struct List**)p;
// void htInit(void* p){
// 	struct HashTable* hashTable = p;
// 	hashTable->size = 0;
// 	hashTable->hashTable = (struct List**)malloc(sizeof(struct List*)*HASH_TABLE_SIZE);
// 	printf("capa %i\n", HASH_TABLE_SIZE);
// 	printf("size %i\n", hashTable->size);
// 	int i=0;
// 	for(i=0; i<HASH_TABLE_SIZE; i++){
// 		hashTable[i] = NULL;
// 	}
// }

// void htInit(void** p){
// 	struct List** hashTable = (struct List**)p;
void htInit(struct List* hashTable[HASH_TABLE_SIZE]){
	printf("htinit puntero hashTable: %lu\n", (long)hashTable);

	int i=0;
	for(i=0; i<HASH_TABLE_SIZE; i++){
		hashTable[i] = NULL;
		// printf("init i%i\t%ld\n", i, (long)hashTable[i]);
	}
}

//	hash function char int idx
int htHashFunction(char* str){
	int sum = 0;
	int i=0;
	for(i=0; i<NAME_SIZE; i++){
		// printf("%c\n", str[i]);
		if(str[i]=='\n')
			break;
		sum += (int)str[i]*(i+1);
		// sum %= HASH_TABLE_SIZE;
	}
	return sum%HASH_TABLE_SIZE;
}


void htLoad(struct List* hashTable[HASH_TABLE_SIZE]){
	struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType));
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
	fseek(dataDogs, 0, SEEK_END);
	long numberOfStructures = ftell(dataDogs)/sizeof(struct dogType);
	printf("number of structures: %ld\n", numberOfStructures);
	rewind(dataDogs);

	long filePointer=0;
	int code=0;
	int i=0;
	for(i=0; i<numberOfStructures; i++){
		filePointer = ftell(dataDogs);
		fread(currDog, sizeof(struct dogType), 1, dataDogs);
		// showDogType(currDog);
		code = htHashFunction(currDog->name);
		// printf("nombre y su hash\t\t%s\t%i\n", currDog->name, code);
		//
		// printf("puntero de hashtable en code\t%lu\n", (long)hashTable[code]);


	  hashTable[code] = (struct List*)malloc(sizeof(struct List));
		if(hashTable[code] == NULL){
			dllInit(hashTable[code]);
			// dllAddHead(hashTable[code], filePointer);
		}
		// printf("puntero de hashtable en code\t%lu\n", (long)hashTable[code]);

		// printf("\t%i\n", (int)filePointer);
		// dllPrintAll(hashTable[code]);
		// while(dllHasNext(hashTable)){
		// }
		// printf("\t\t%ld\n", dllGetCurrData(hashTable[code]));
	// 	// printf("%ld\n", dll);
	}
	checkfclose(dataDogs, DATA_DOGS_PATH);
	free(currDog);
}

void htFree(struct List* list[HASH_TABLE_SIZE]){
  int i=0;
  for(i=0; i<HASH_TABLE_SIZE; i++){
    dllFree(list[i]);
  }
}

// htAdd (nombre, filePointer) ---> dllAddHead(filePointer)
// htSearch/show
// htDelete
// htupdate = htLoad (furrFilePointer)

int main(){
	// void* p = (void*)malloc(sizeof(struct List*)*HASH_TABLE_SIZE);
	// printf("HASH_TABLE_SIZE: %i\n", HASH_TABLE_SIZE);
	struct List** hashTable = (struct List**)malloc(sizeof(struct List*)*HASH_TABLE_SIZE);
	// printf("puntero hashTable 1: %lu\n", (long)hashTable);
	if(hashTable == NULL){
		perror("error en malloc de la hash table");
		exit(-1);
	}
	// int i;
	// for(i=0; i<HASH_TABLE_SIZE; i++){
	// 	printf("init i%i\t%ld\n", i, (long)hashTable[i]);
	// }
	// printf("puntero hashTable 2: %lu\n", (long)hashTable);
	htInit(hashTable);
	// printf("puntero hashTable 3: %lu\n", (long)hashTable);
	// for(i=0; i<HASH_TABLE_SIZE; i++){
	// 	printf("init i%i\t%ld\n", i, (long)hashTable[i]);
	// }


	htLoad(hashTable);
	// confirmar con htSearch
	return 0;
}
