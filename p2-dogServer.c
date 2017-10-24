#include "hashTable.c"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#define BACKLOG 2
#define PORT 3547
#define MESSAGESIZE 22

// struct sockadd_in{
//   short sin_family; //  AF_INET
//   unsigned short sin_port;
//   struct in_addr sin_addr;
//   char sin_zero[8];
// };
// struct in_addr{
//   unsigned long s_addr;
// }

// Endianismo
//  big endian
//  little endian
// htons(3535);

void menu(int clientsd);
void recvNewReg(int clientsd);
void showReg(int clientsd);
void showSearch(int clientsd);

int hashTable[HASH_TABLE_SIZE];
int main(){
	htInit(hashTable);
	htLoad(hashTable);

  int serversd, clientsd[32];
  struct sockaddr_in server, client[32];
  socklen_t clientSize;
  int serverSize, r;

  serversd = socket(AF_INET, SOCK_STREAM, 0);
  if(serversd == -1){
    perror("Socket error\n");
    exit(-1);
  }

  // Assigning a name to a socket
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = INADDR_ANY;
  bzero(server.sin_zero, 8);

  serverSize = sizeof(struct sockaddr);

  r = bind(serversd, (struct sockaddr*)&server, serverSize);
  if(r == -1){
    perror("Bind error\n");
    exit(-1);
  }

  r = listen(serversd, BACKLOG);
  if(r == -1){
    perror("Listen error\n");
    exit(-1);
  }

  // process or thread
  clientsd[0] = accept(serversd, (struct sockaddr*)&client[0], (socklen_t*)&clientSize);
  if(clientsd[0] == -1){
    perror("Accept error\n");
    exit(-1);
  }

	menu(clientsd[0]);

  close(clientsd[0]);
  close(serversd);
}
void menu(int clientsd){
  int option;
  int r = recv(clientsd, &option, sizeof(int), 0);
  if(r != sizeof(int)){
    perror("Recv error");
    exit(-1);
  }
  printf("option: %i\n", option);
  switch (option) {
		case 1:
			printf("Ingresar registro\n");
			recvNewReg(clientsd);
			break;
		case 2:
			printf("Ver registro\n");
			showReg(clientsd);
			break;
		case 3:
			printf("Borrar registro\n");
			// deleteReg();
			break;
		case 4:
			printf("Buscar registro\n");
      showSearch(clientsd);
			break;
		case 5:
			printf("Salir\n");
			break;
		default:
			printf("default\n");
			break;
	}
}

void recvNewReg(int clientsd){
  int j;
  struct dogType *newDog = (struct dogType*) malloc(sizeof(struct dogType));
  int r = recv(clientsd, newDog, sizeof(struct dogType), 0);
  if(r != sizeof(struct dogType)){
    perror("Recv error");
    exit(-1);
  }

	FILE* fptc = checkfopen(CURRENT_ID_PATH, "r+");
	unsigned int id;
	fread(&id, sizeof(int), 1, fptc);
	newDog->id = id;
	id += 1;
	rewind(fptc);
	fwrite(&id, sizeof(int), 1, fptc);
	checkfclose(fptc, CURRENT_ID_PATH);

  showFullDogType(newDog);
  //Cambia el next de la estructura anterior del mismo nombre
  //poner en minusculas name
  char nameAux[NAME_SIZE];
  strcpy(nameAux, newDog->name);
  for(j = 0; j<strlen(nameAux); j++)
    nameAux[j] = tolower(nameAux[j]);
	int code = htHashFunction(nameAux);

	int next = hashTable[code];
  FILE* fptr = checkfopen(DATA_DOGS_PATH, "r+");
	struct dogType* currDog = malloc(sizeof(struct dogType));
	if(next == -1){
		fseek(fptr, 0, SEEK_END);
		hashTable[code] = ftell(fptr);
	}
	while(next > 0){
		fseek(fptr, next, SEEK_SET);
		fread(currDog, sizeof(struct dogType), 1, fptr);
		showFullDogType(currDog);
		if(currDog->next == 0){
			fseek(fptr, 0, SEEK_END);
			currDog->next = (int)ftell(fptr);
			fseek(fptr, next, SEEK_SET);
			fwrite(currDog, sizeof(struct dogType), 1, fptr);
			break;
		}
		next = currDog->next;
	}

	//Añade la estructura a la hashTable y a dataDogs.dat
	fseek(fptr, 0, SEEK_END);
	fwrite(newDog, sizeof(struct dogType), 1, fptr);

  free(newDog);
  free(currDog);

	checkfclose(fptr, DATA_DOGS_PATH);
	printf("Registro añadido exitosamente.\n");
	menu(clientsd);
}

void showReg(int clientsd){
	FILE* fptr = checkfopen(DATA_DOGS_PATH, "r");
	fseek(fptr, 0, SEEK_END);
	int totalSize = ftell(fptr) / (sizeof(struct dogType));
  int r = send(clientsd, &totalSize, sizeof(int), 0);
  if(r == -1){
    perror("Send error\n");
    exit(-1);
  }

  int numReg;
  r = recv(clientsd, &numReg, sizeof(int), 0);
  if(r != sizeof(int)){
    perror("Recv error");
    exit(-1);
  }
  struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
  fseek(fptr, numReg*sizeof(struct dogType), SEEK_SET);
  fread(newDog, sizeof(struct dogType), 1, fptr);
  r = send(clientsd, newDog, sizeof(struct dogType), 0);
  if(r == -1){
    perror("Send error\n");
    exit(-1);
  }
  showFullDogType(newDog);
  free(newDog);
  checkfclose(fptr, DATA_DOGS_PATH);
  printf("Consulta de registro exitosa\n");
	menu(clientsd);
}

void showSearch(int clientsd){
	char name[NAME_SIZE];
  int r = recv(clientsd, name, NAME_SIZE, 0);
  if(r != NAME_SIZE){
    perror("Recv error");
    exit(-1);
  }
  printf("%s\n", name);

	int success = 0;
	int i = 0, j=0;

  //poner en minusculas name
  char nameAux[NAME_SIZE];
  strcpy(nameAux, name);
  for(j = 0; j<strlen(nameAux); j++)
    nameAux[j] = tolower(nameAux[j]);
  int code = htHashFunction(nameAux);
  int next = 0;


	r = send(clientsd, &hashTable[code], sizeof(int), 0);
	if(r == -1){
		perror("Send error\n");
		exit(-1);
	}
	printf("%s\n", name);

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
			printf("Entró al do\n");
			fseek(dataDogs, next, SEEK_SET);
			fread(newDog, sizeof(struct dogType), 1, dataDogs);
			strcpy(dogNameAux, newDog->name);

			for(i = 0; i<strlen(dogNameAux); i++){
				dogNameAux[i] = tolower(dogNameAux[i]);
			}
			if(!strcmp(dogNameAux, nameAux)){
				r = send(clientsd, newDog, sizeof(int), 0);
				if(r == -1){
					perror("Send error\n");
					exit(-1);
				}
				if(success==0){
					showDogTypeTableHead();
					success = 1;
				}
			}
			showDogTypeTable(newDog);
			next = newDog->next;
		}while(next != 0);
		newDog = NULL;
		r = send(clientsd, newDog, sizeof(int), 0);
		if(r == -1){
			perror("Send error\n");
			exit(-1);
		}

		free(newDog);
		checkfclose(dataDogs, DATA_DOGS_PATH);
	}
	menu(clientsd);
}
