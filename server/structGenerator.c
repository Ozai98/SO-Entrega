// #include "atributes.c"
#include "hashTable.c"
#include <time.h>

//Declaración de las funciones
void txttoDat();
void getRandomName(void* p, char* selectedName);
int hashTable[HASH_TABLE_SIZE];

void printDataDogs(){
  FILE* fDataDogs = checkfopen(DATA_DOGS_PATH, "r");
  struct dogType* newDog = malloc(sizeof(struct dogType));
  int i=0;
  for(i=0; i<STRUCTURES_NUMBER; i++){
    fread(newDog, sizeof(struct dogType), 1, fDataDogs);
    // showDogType(newDog);
    showFullDogType(newDog);
  }
  free(newDog);
  checkfclose(fDataDogs,DATA_DOGS_PATH);
}

//Función que crea un .dat a partir del .txt de los nombres de mascotas
void txttoDat(){
  FILE* fNombresMascotas = checkfopen(NOMBRES_MASCOTAS_PATH, "r");
  FILE* fPetNames = checkfopen(PET_NAMES_PATH, "w");
  char name[NAME_SIZE];
  bzero(name, NAME_SIZE);
  while(fgets(name,NAME_SIZE,fNombresMascotas)){
    name[strlen(name)-1]=0;
    fwrite(name, NAME_SIZE, 1, fPetNames);
    bzero(name, NAME_SIZE);
  }
  checkfclose(fPetNames, PET_NAMES_PATH);
  checkfclose(fNombresMascotas, NOMBRES_MASCOTAS_PATH);
}

//Función que obtiene un nombre aleatorio de la lista proveída
void getRandomName(void* p, char* selectedName){
  FILE* fptr = p;
  // char* selectedName = (char*)malloc(sizeof(char)*NAME_SIZE);
  int number = (rand() % 1716) *32;
  fseek(fptr, number, SEEK_SET);
  fread(selectedName, 1, NAME_SIZE, fptr);
  // return selectedName;
}

int main(){
  txttoDat();
  htInit(hashTable);
  srand(time(NULL));
  int i = 0, j = 0, id = 1, typeIdx = 0, currPos, number;
  unsigned long currHash;
  // generar .dat
  // TODO: Crear mas tipos y razashashTable
  char* type[4]={"Perro","Gato","Roedor","Reptiles"};
  char* breed[4][3] = {
    {"Pastor Alemán","Labrador","Chihuahua"},
    {"Persa","Siamés","Bengalí"},
    {"Cuy","Hamster","Conejo"},
    {"Tortuga","Lagartija","Iguana"},
  };
  char gender[2] = {'H','M'};

  FILE* fDataDogs = checkfopen(DATA_DOGS_PATH, "w+");
  FILE* fPetNames = checkfopen(PET_NAMES_PATH,"r");
  struct dogType* newDog = malloc(sizeof(struct dogType));
  struct dogType* prevDog = malloc(sizeof(struct dogType));
  for(i=0; i<STRUCTURES_NUMBER; i++, id++){
    newDog->id = id;
    number = (i % 1716) *32;
    fseek(fPetNames, number, SEEK_SET);
    fread(newDog->name, 1, NAME_SIZE, fPetNames);
    typeIdx = rand()%4;
    bzero(newDog->type, TYPE_SIZE);
    strcpy(newDog->type, type[typeIdx]);
    bzero(newDog->breed, BREED_SIZE);
    strcpy(newDog->breed, breed[typeIdx][rand()%3]);
    newDog->age = rand()%20;
    newDog->height = rand()%100;
    newDog->weight = (rand()%5000)/100.0;
    newDog->gender = gender[rand()%2];
    newDog->next = 0;

    currPos = (int)ftell(fDataDogs);
    currHash = htHashFunction(newDog->name);
    if(hashTable[currHash] != -1){
      // the next atribute is the first in the struct
      fseek(fDataDogs, hashTable[currHash], SEEK_SET);
      fwrite(&currPos, sizeof(int) , 1, fDataDogs);
      fseek(fDataDogs, 0, SEEK_END);
    }

    hashTable[currHash] = currPos;
    fwrite(newDog, sizeof(struct dogType) , 1, fDataDogs);
  }
  checkfclose(fPetNames, PET_NAMES_PATH);
  free(newDog);
  free(prevDog);
  checkfclose(fDataDogs,DATA_DOGS_PATH);

  FILE* fCurrentId = checkfopen(CURRENT_ID_PATH, "w");
  fwrite(&id, sizeof(int), 1, fCurrentId);
  checkfclose(fCurrentId,CURRENT_ID_PATH);
  return 0;
}
