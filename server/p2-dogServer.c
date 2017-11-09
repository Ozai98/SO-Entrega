#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <strings.h>
#include "hashTable.c"
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#define BACKLOG 2
#define PORT 3536
#define SERVER_LOG_PATH "ServerDogs.log"
#define NUM_THREADS 32

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

struct threadArgs{
	int clientsd;
	char ip[INET6_ADDRSTRLEN];
};


int hashTable[HASH_TABLE_SIZE];
void *menu(void *ptr);
void recvNewReg(void *ptr);
void showReg(void *ptr);
void recvDeleteReg(void *ptr);
void showSearch(void *ptr);
void openHistory();
int file_exist(char *filename);


int main(){
	struct threadArgs* args[NUM_THREADS];
	int i, r, serversd, serverSize, optval;
	socklen_t clientSize;
	pthread_t threads[NUM_THREADS];
	struct sockaddr_in server, client[32];
	htInit(hashTable);
	htLoad(hashTable);
	printf("%s\n","Servidor listo");
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
	optval = 1;

	if (setsockopt(serversd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

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
	for(i = 0; i < NUM_THREADS; i++){
	  // process or thread
		int clientsd = accept(serversd, (struct sockaddr*)&client[i], (socklen_t*)&clientSize);
		if(clientsd == -1){
			perror("Accept error\n");
			exit(-1);
		}
		args[i] = (struct threadArgs*)malloc(sizeof(struct threadArgs));
		args[i]->clientsd = clientsd;
		bzero(args[i]->ip,sizeof(args[i]->ip));
		inet_ntop(AF_INET, &client[i].sin_addr, args[i]->ip, sizeof args[i]->ip);
		r = pthread_create(&threads[i], NULL, menu, (void*)args[i]);
		if(r != 0){
      perror("pthread_create error");
      exit(-1);
    }
	}

	for(i = 0; i<NUM_THREADS; i++){
		r = pthread_join(threads[i], NULL);

		if(r != 0){
        perror("pthread_join error");
        exit(-1);
    }
		close(args[i]->clientsd);
		free(args[i]);
	}
  close(serversd);
}

void *menu(void* ptr){
	struct threadArgs* args = ptr;
	int clientsd = args->clientsd;
  int option;
  int r = recv(clientsd, &option, sizeof(int), MSG_WAITALL);
  if(r != sizeof(int)){
    perror("Recv error");
    exit(-1);
  }
  switch (option) {
		case 1:
			//printf("Ingresar registro\n");
			recvNewReg(args);
			break;
		case 2:
			//printf("Ver registro\n");
			showReg(args);
			break;
		case 3:
			//printf("Borrar registro\n");
			recvDeleteReg(args);
			break;
		case 4:
			//printf("Buscar registro\n");
      showSearch(args);
			break;
		case 5:
			//printf("Salir\n");
			free(args);
			break;
		default:
			//printf("default\n");
			break;
	}
}

char* timeAndDate(char* date){
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(date,"%d-%d-%d %d:%d:%d] ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	return date;
}

void generateLog(char* opType, char* ip, char* registry, char* searchedString){
	int index = 300;
	char serverLogString[300];
	char date[100];
	char* currentDate = timeAndDate(date);
	FILE* serverLog = checkfopen(SERVER_LOG_PATH, "a");
	snprintf(serverLogString, sizeof(serverLogString), "[Fecha: %s] [Cliente: %s] [Operacion: %s] [Registro: %s] [Cadena buscada: %s]\n", currentDate, ip, opType, registry, searchedString);
	//printf("%s\n", serverLogString);
	const char *ptr = strchr(serverLogString, '\n');
	if(ptr)
   index = ptr - serverLogString;
	fseek(serverLog,0, SEEK_END);
	fwrite(serverLogString, 1, index+1, serverLog);
	checkfclose(serverLog, SERVER_LOG_PATH);
}

void recvNewReg(void *ptr){
	struct threadArgs *args = ptr;
	int clientsd = args->clientsd;
  int j;
	char registry[10];
	bzero(registry, (int)sizeof(registry));
  struct dogType *newDog = (struct dogType*) malloc(sizeof(struct dogType));
  int r = recv(clientsd, newDog, sizeof(struct dogType), MSG_WAITALL);
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
  //showFullDogType(newDog);
  //Cambia el next de la estructura anterior del mismo nombre
	int code = htHashFunction(newDog->name);
	int next = hashTable[code];
  FILE* fptr = checkfopen(DATA_DOGS_PATH, "r+");
	if(next == -1){
		fseek(fptr, 0, SEEK_END);
		hashTable[code] = ftell(fptr);
	}
	struct dogType* currDog = malloc(sizeof(struct dogType));
	while(next > 0){
		fseek(fptr, next, SEEK_SET);
		fread(currDog, sizeof(struct dogType), 1, fptr);
		// showFullDogType(currDog);
		if(currDog->next == 0){
			fseek(fptr, 0, SEEK_END);
			currDog->next = (int)ftell(fptr);
			fseek(fptr, next, SEEK_SET);
			fwrite(&currDog->next, sizeof(int), 1, fptr);
			break;
		}
		next = currDog->next;
	}

	//Añade la estructura a la hashTable y a dataDogs.dat
	fseek(fptr, 0, SEEK_END);
	newDog->position = ftell(fptr)/sizeof(struct dogType)+1;
	fwrite(newDog, sizeof(struct dogType), 1, fptr);
	sprintf(registry, "%u", newDog->id);
  free(newDog);
  free(currDog);

	checkfclose(fptr, DATA_DOGS_PATH);
	//printf("Registro añadido exitosamente.\n");
	generateLog("Inserción", args->ip, registry, "Sin cadena de busqueda");
	menu(args);
}

void showReg(void *ptr){
	struct threadArgs *args = ptr;
	int clientsd = args->clientsd;
	FILE* fptr = checkfopen(DATA_DOGS_PATH, "r");
	char registryString [10];
	char searchedRegistry[10];
	bzero(registryString, (int)sizeof(registryString));
	bzero(searchedRegistry, (int)sizeof(searchedRegistry));
	fseek(fptr, 0, SEEK_END);
	int totalSize = ftell(fptr) / (sizeof(struct dogType));
  int r = send(clientsd, &totalSize, sizeof(int), 0);
  if(r == -1){
    perror("Send error\n");
    exit(-1);
  }

  int numReg;
  r = recv(clientsd, &numReg, sizeof(int), MSG_WAITALL);
  if(r != sizeof(int)){
    perror("Recv error");
    exit(-1);
  }
  struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
  fseek(fptr, numReg*sizeof(struct dogType), SEEK_SET);
  fread(newDog, sizeof(struct dogType), 1, fptr);
	// showFullDogType(newDog);
  r = send(clientsd, newDog, sizeof(struct dogType), 0);
  if(r == -1){
    perror("Send error\n");
    exit(-1);
  }
	sprintf(searchedRegistry, "%i", newDog->id);
	sprintf(registryString, "%d", numReg+1);
  checkfclose(fptr, DATA_DOGS_PATH);
  //printf("Consulta de registro exitosa\n");
	generateLog("Lectura", args->ip, registryString, searchedRegistry);
	openHistory(args, newDog->id);
	free(newDog);

	menu(args);
}

void recvDeleteReg(void *ptr){
	struct threadArgs *args = ptr;
	int r, i=0, numReg, filePointer, clientsd = args->clientsd, code;
	char consultedDog[10];
	char returnedDog[10];
	bzero(consultedDog, (int)sizeof(consultedDog));
	bzero(returnedDog, (int)sizeof(returnedDog));
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
	fseek(dataDogs, 0, SEEK_END);
	int totalSize = ftell(dataDogs) / (sizeof(struct dogType));
	rewind(dataDogs);

  r = send(clientsd, &totalSize, sizeof(int), 0);
  if(r == -1){
    perror("Send error\n");
    exit(-1);
  }

  r = recv(clientsd, &numReg, sizeof(int), MSG_WAITALL);
  if(r != sizeof(int)){
    perror("Recv error");
    exit(-1);
  }

	FILE* tempDataDogs = checkfopen(TEMP_DATA_DOGS_PATH, "w");
  struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType));
	for(i=0; i<totalSize; i++){
		//Copia todos los registros en el archivo nuevo
		filePointer = ftell(dataDogs);
		fread(currDog, sizeof(struct dogType), 1, dataDogs);
		if(filePointer == numReg*sizeof(struct dogType)){
			sprintf(returnedDog, "%d", currDog->id);
			r = send(clientsd, currDog, sizeof(struct dogType), 0);
			if(r == -1){
			  perror("Send error\n");
			  exit(-1);
			}
			continue;
		}
		if(filePointer > numReg*sizeof(struct dogType))
			currDog->position -= 1;

		if(currDog->next == 0){
			// en caso de que el registro que se quiera eliminar sea el primero (numReg = 0)
		}else if(currDog->next > numReg*sizeof(struct dogType)){
			currDog->next -= sizeof(struct dogType);
		}else if(currDog->next == numReg*sizeof(struct dogType)){
			fseek(dataDogs, currDog->next, SEEK_SET);
			fread(&currDog->next, sizeof(int), 1, dataDogs);
			currDog->next -= sizeof(struct dogType);
			fseek(dataDogs, filePointer+sizeof(struct dogType), SEEK_SET);
		}
		fwrite(currDog, sizeof(struct dogType), 1, tempDataDogs);
	}

  free(currDog);
	//printf("%s\n", "Eliminación exitosa... espere por favor");

	checkfclose(dataDogs, DATA_DOGS_PATH);
	checkfclose(tempDataDogs, TEMP_DATA_DOGS_PATH);
	remove(DATA_DOGS_PATH);
	rename(TEMP_DATA_DOGS_PATH, DATA_DOGS_PATH);

	htInit(hashTable);
	htLoad(hashTable);

  // printf("Borrado de registro exitoso\n");
	sprintf(consultedDog, "%d", numReg+1);
	generateLog("Eliminación", args->ip, returnedDog, consultedDog);

	// send delete confirmation to client
	int success = 1;
	r = send(clientsd, &success, sizeof(int), 0);
	if(r == -1){
		perror("Send error\n");
		exit(-1);
	}
	menu(args);

}

void showSearch(void *ptr){
	struct threadArgs *args = ptr;
	int clientsd = args->clientsd;
	char name[NAME_SIZE];
  int r = recv(clientsd, name, NAME_SIZE, MSG_WAITALL);
  if(r != NAME_SIZE){
    perror("Recv error");
    exit(-1);
  }
	//  printf("%s\n", name);

	int success = 0;
	int i = 0, j=0;


  int code = htHashFunction(name);

	int exists = hashTable[code] == -1 ? 0 : 1;
	r = send(clientsd, &exists, sizeof(int), 0);
	if(r == -1){
		perror("Send error\n");
		exit(-1);
	}
	//printf("%s\n", name);

  if(!exists){
		// printf("%s\n","Mascota no existe server");
		generateLog("Busqueda", args->ip, "No encontrado", name);
		menu(args);
		return;
	}
	// Siempre tendrá al menos un Valor
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
	struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
	//poner en minusculas name
	char dogNameAux[NAME_SIZE];
	char nameAux[NAME_SIZE];
	strcpy(nameAux, name);
	for(j = 0; j<strlen(nameAux); j++)
		nameAux[j] = tolower(nameAux[j]);

	int next = hashTable[code];
	int hasDog = 0;
	do{
		fseek(dataDogs, next, SEEK_SET);
		fread(newDog, sizeof(struct dogType), 1, dataDogs);
		strcpy(dogNameAux, newDog->name);

		for(i = 0; i<strlen(dogNameAux); i++){
			dogNameAux[i] = tolower(dogNameAux[i]);
		}
		if(!strcmp(dogNameAux, nameAux)){
			hasDog = 1;
			if(success==0){
				// showDogTypeTableHead();
				success = 1;
				hasDog = 2;
			}
			r = send(clientsd, &hasDog, sizeof(int), 0);
			if(r == -1){
				perror("Send error\n");
				exit(-1);
			}
			send(clientsd, newDog, sizeof(struct dogType), 0);
			// checkSend(clientsd, newDog, sizeof(struct dogType), 0, "newDog");
			// r = send(clientsd, newDog, sizeof(struct dogType), 0);
			// if(r == -1){
			// 	perror("Send error\n");
			// 	exit(-1);
			// }
		}
		// showDogTypeTable(newDog);
		next = newDog->next;
	}while(next != 0);
	hasDog = 0;
	r = send(clientsd, &hasDog, sizeof(int), 0);
	if(r == -1){
		perror("Send error\n");
		exit(-1);
	}
	// printf("Busqueda exitosa");
	generateLog("Busqueda", args->ip, "Múltiples", name);

	free(newDog);
	checkfclose(dataDogs, DATA_DOGS_PATH);
	menu(args);
}

void openHistory(void *ptr, int dogId){
	//printf("dogID: %i\n", dogId);
	struct threadArgs *args = ptr;
	int clientsd = args->clientsd;
	int r;
	char ans;
	r = recv(clientsd, &ans, sizeof(char), MSG_WAITALL);
	if(r != sizeof(char)){
		perror("Recv error");
		exit(-1);
	}

	if(ans == 's' || ans == 'S'){
		struct stat file_stat;
		int fileSize, resp;
		char file_name[12];
		char file_name_edit[12];
		sprintf(file_name, "%i.txt", dogId);
		sprintf(file_name_edit, "~%i.txt", dogId);
		//printf("file_name: %s\n", file_name);
		//printf("file_name_edit: %s\n", file_name_edit);
		if(file_exist(file_name_edit))
			resp = 2;
		else
			if(file_exist(file_name))
				resp = 1;
			else
				resp = 0;
		//printf("resp %i\n", resp);
		r = send(clientsd, &resp, sizeof(int), MSG_WAITALL);
		if(r == -1){
			perror("Recv error");
			exit(-1);
		}

		// file not found
		if(resp == 0){
			FILE* edit = checkfopen(file_name_edit, "w");
			checkfclose(edit, file_name_edit);

			r = recv(clientsd, &fileSize, sizeof(int), MSG_WAITALL);
			if(r != sizeof(int)){
				perror("Recv error");
				exit(-1);
			}
			int remain_data = fileSize;
			sprintf(file_name, "%i.txt", dogId);
			FILE* myFile = checkfopen(file_name, "w");
			int len;
			char buffer[BUFSIZ];
			while ((remain_data > 0) && ((len = recv(clientsd, buffer, BUFSIZ, 0)) > 0)){
				//printf("in while\n");
				fwrite(buffer, sizeof(char), len, myFile);
				remain_data -= len;
				//printf("Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
			}
			checkfclose(myFile, file_name);

			remove(file_name_edit);
		}
		//  file exists
		else if(resp == 1){
			FILE* edit = checkfopen(file_name_edit, "w");
			checkfclose(edit, file_name_edit);

			//printf("%s\n", file_name);
			int fd = open(file_name, O_RDONLY);
			if (fd == -1){
				perror("open error\n");
				exit(-1);
			}
			r = stat(file_name, &file_stat);
			if (r == -1){
				perror("fstat error\n");
				exit(-1);
			}
			//printf("File size %i\n", (int)file_stat.st_size);
			fileSize = (int)file_stat.st_size;
			// printf("File size %i\n", fileSize);
			r = send(clientsd, &fileSize, sizeof(int), 0);
			if(r == -1){
				perror("send error");
				exit(-1);
			}

			off_t offset = 0;
			int sent_bytes = 0;
			int remain_data = fileSize;
			/* Sending file data */
			// while ((remain_data > 0) && ((sent_bytes = sendfile(clientsd, fd, &offset, BUFSIZ)) > 0)){
			while (((sent_bytes = sendfile(clientsd, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0)){
				// printf("in while\n");
				// printf("1. Server sent %d bytes from file's data, offset is now : %li and remaining data = %d\n", sent_bytes, offset, remain_data);
				remain_data -= sent_bytes;
				// printf("2. Server sent %d bytes from file's data, offset is now : %li and remaining data = %d\n", sent_bytes, offset, remain_data);
			}
			// printf("Salió del while\n");
			close(fd);

			r = recv(clientsd, &fileSize, sizeof(int), MSG_WAITALL);
			if(r != sizeof(int)){
				perror("Recv error");
				exit(-1);
			}
			remain_data = fileSize;
			FILE* myFile = checkfopen(file_name, "w");
			int len;
      char buffer[BUFSIZ];
			while ((remain_data > 0) && ((len = recv(clientsd, buffer, BUFSIZ, 0)) > 0)){
			//	printf("in while\n");
				fwrite(buffer, sizeof(char), len, myFile);
				remain_data -= len;
			//	printf("Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
			}
			checkfclose(myFile, file_name);
			remove(file_name_edit);
		}
	}
}

int file_exist(char *filename){
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}
