#include "hashTable.c"
#include <time.h>

// Declaración de las funciones
void txttoDat();
void getRandomName(void* p, char* selectedName);

int hashTable[HASH_TABLE_SIZE];

// Función que crea un .dat a partir del .txt de los nombres de mascotas
void txttoDat(){
  FILE* fNombresMascotas = checkfopen(NOMBRES_MASCOTAS_PATH, "r"); // Abre el archivo que contiene los nombres de las mascotas en txt
  FILE* fPetNames = checkfopen(PET_NAMES_PATH, "w"); // Abre el archivo que contendrá las mascota en .dat
  char name[NAME_SIZE];
  bzero(name, NAME_SIZE);
  while(fgets(name,NAME_SIZE,fNombresMascotas)){ // Mientras haya nombres en el txt los copia al .dat
    name[strlen(name)-1]=0;
    fwrite(name, NAME_SIZE, 1, fPetNames);
    bzero(name, NAME_SIZE);
  }
  checkfclose(fPetNames, PET_NAMES_PATH); // Cierra el txt y el .dat
  checkfclose(fNombresMascotas, NOMBRES_MASCOTAS_PATH);
}

int main(){
  txttoDat();
  htInit(hashTable); // Inicializa la hashTable
  srand(time(NULL)); // Configura la semilla de la función de aleatoreidad
  int i = 0, j = 0, id = 1, typeIdx = 0, currPos, number;
  unsigned long currHash;
  char* type[4]={"Perro","Gato","Roedor","Reptiles"}; //Crea los tipos y razas de las mascotas
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
  for(i=0; i<STRUCTURES_NUMBER; i++, id++){ // Crea 10000000 de estructuras
    newDog->id = id; // Asigna un ID a la nueva estructura
    newDog->position = id; // Asigna una posición a la nueva estructura
    number = (i % 1716) *32; // Selecciona un nombre aleatorio
    fseek(fPetNames, number, SEEK_SET);
    fread(newDog->name, 1, NAME_SIZE, fPetNames); // Lo asigna a la nueva estructura
    typeIdx = rand()%4; // Selecciona un tipo aleatorio
    bzero(newDog->type, TYPE_SIZE);
    strcpy(newDog->type, type[typeIdx]); // Lo asigna a la nueva estructura
    bzero(newDog->breed, BREED_SIZE);
    strcpy(newDog->breed, breed[typeIdx][rand()%3]); // Selecciona una raza aleatoria y la asigna a la nueva estructura
    newDog->age = rand()%20; // Asigna edad altura peso y genero aleatorios a la nueva estructura
    newDog->height = rand()%100;
    newDog->weight = (rand()%5000)/100.0;
    newDog->gender = gender[rand()%2];
    newDog->next = 0;

    currPos = (int)ftell(fDataDogs);
    currHash = (int)htHashFunction(newDog->name);
    if(hashTable[currHash] != -1){
      // (El atributo siguiente es el primero de la estructura)
      fseek(fDataDogs, hashTable[currHash], SEEK_SET);
      fwrite(&currPos, sizeof(int) , 1, fDataDogs);
      fseek(fDataDogs, 0, SEEK_END);
    }

    hashTable[currHash] = currPos;
    fwrite(newDog, sizeof(struct dogType) , 1, fDataDogs); // Escribe la estructura en dataDogs
  }
  checkfclose(fPetNames, PET_NAMES_PATH);
  free(newDog);
  free(prevDog);
  checkfclose(fDataDogs,DATA_DOGS_PATH);

  FILE* fCurrentId = checkfopen(CURRENT_ID_PATH, "w");
  fwrite(&id, sizeof(int), 1, fCurrentId); //Escribe el ID en el archivo de ids
  checkfclose(fCurrentId,CURRENT_ID_PATH);
  return 0;
}
