#include "linkedList.c"



// TODO: change atribute to void* p
void initHashTable(struct List* list[HASH_TABLE_SIZE]){
	// struct List* list[HASH_TABLE_SIZE] = p;
	int i=0;
	for(i=0; i<HASH_TABLE_SIZE; i++){
		list[i] = NULL;
	}
}
//	hash function char int idx
int hashFunction(char* str){
	int sum = 0;
	int i=0;
	for(i=0; i<NAME_SIZE; i++){
		// printf("%c\n", str[i]);
		if(str[i]=='\n')
			break;
		sum+=(int)str[i]*i*i;
		sum%=HASH_TABLE_SIZE;
	}
	return sum%HASH_TABLE_SIZE;
}
void loadHashTable(struct List* list[HASH_TABLE_SIZE]){
	// struct List* list[HASH_TABLE_SIZE] = p;
	struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType));
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
	fseek(dataDogs, 0, SEEK_END);
	long nStructures = ftell(dataDogs)/sizeof(struct dogType);
	rewind(dataDogs);
	long i=0;
	long id=0;
	int code=0;
	for(i=0; i<nStructures; i++){
		id = ftell(dataDogs);
		fread(currDog, sizeof(struct dogType), 1, dataDogs);
		code = hashFunction(currDog->name);
		// printf("%s%i\n", currDog->name, code);
		// printf("%i\n", (int)list[code]);
		// printf("%ld\n", dll);
		if(list[code] == NULL)
			dllInit(list[code]);
		dllAddBack(list[code], id);
	}
	checkfclose(dataDogs, DATA_DOGS_PATH);
	free(currDog);
}

void freeHashTable(struct List* list[HASH_TABLE_SIZE]){
  int i=0;
  for(i=0; i<HASH_TABLE_SIZE; i++){
    dllFree(list[i]);
  }
}
