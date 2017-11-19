#include <arpa/inet.h>
#include <pthread.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include "hashTable.c"

// Constantes de operación
#define BACKLOG 2
#define PORT 3535
#define SERVER_LOG_PATH "ServerDogs.log"
#define NUM_THREADS 32

struct threadArgs{// Estructura que contiene el descriptor y la IP de un cliente entrante
	int clientsd;
	char ip[INET6_ADDRSTRLEN];
};


int hashTable[HASH_TABLE_SIZE];
void* menu(void *ptr);
char* timeAndDate(char* date);
void generateLog(char* opType, char* ip, char* registry, char* searchedString);
void recvNewReg(void *ptr);
void showReg(void *ptr);
void recvDeleteReg(void *ptr);
void showSearch(void *ptr);
void openHistory();
int file_exist(char *filename);


int main(){
	struct threadArgs* args[NUM_THREADS]; //Inicializa variables del servidor
	int i, r, serversd, serverSize, optval;
	socklen_t clientSize;
	pthread_t threads[NUM_THREADS];
	struct sockaddr_in server, client[32];
	htInit(hashTable); //Inicializa la hashTable
	htLoad(hashTable); //Carga la hashTable
	printf("%s\n","Servidor listo");
	serversd = socket(AF_INET, SOCK_STREAM, 0); //Crea una socket

  if(serversd == -1){
    perror("Socket error\n");
    exit(-1);
  }

  //Configura la socket
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = INADDR_ANY;
  bzero(server.sin_zero, 8);
	serverSize = sizeof(struct sockaddr);
	//Configura la reutilización del puerto de comunicación
	optval = 1;
	if (setsockopt(serversd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");
	//Nombra la socket
  r = bind(serversd, (struct sockaddr*)&server, serverSize);

	if(r == -1){
    perror("Bind error\n");
    exit(-1);
  }
	//Configura el servidor para que escuche solicitudes entrantes
  r = listen(serversd, BACKLOG);

	if(r == -1){
    perror("Listen error\n");
    exit(-1);
  }
	for(i = 0; i < NUM_THREADS; i++){
	  // Acepta la conexión entrante
		int clientsd = accept(serversd, (struct sockaddr*)&client[i], (socklen_t*)&clientSize);
		if(clientsd == -1){
			perror("Accept error\n");
			exit(-1);
		}
		//Coloca la conexión entrante en un hilo
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
 	//Cierra los hilos cuando han finalizado las tareas
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
	//Inicializa las variables de operación
	struct threadArgs* args = ptr;
	int clientsd = args->clientsd;
  int option;
  int r = recv(clientsd, &option, sizeof(int), MSG_WAITALL); //Recibe la opción que el cliente quiere ejecutar
  if(r != sizeof(int)){
    perror("Recv error");
    exit(-1);
  }
  switch (option) {
		case 1:
			recvNewReg(args);
			break;
		case 2:
			showReg(args);
			break;
		case 3:
			recvDeleteReg(args);
			break;
		case 4:
      showSearch(args);
			break;
		case 5:
			free(args);
			break;
		default:
			break;
	}
}

char* timeAndDate(char* date){// Función que obtiene la fecha y hora actual del servidor y los guarda en una cadena de caracteres
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(date,"%d-%d-%d %d:%d:%d] ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	return date;
}

void generateLog(char* opType, char* ip, char* registry, char* searchedString){ // Funcion que genera una entrada al log del servidor
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
	//Inicializa variables de adición
	struct threadArgs *args = ptr;
	int clientsd = args->clientsd;
  int j;
	char registry[10];
	bzero(registry, (int)sizeof(registry));
  struct dogType *newDog = (struct dogType*) malloc(sizeof(struct dogType)); // Reserva espacio para estructura temporal
  int r = recv(clientsd, newDog, sizeof(struct dogType), MSG_WAITALL); // Recibe la estructura creada por el cliente

  if(r != sizeof(struct dogType)){
    perror("Recv error");
    exit(-1);
  }

	FILE* fptc = checkfopen(CURRENT_ID_PATH, "r+"); // Abre el archivo que contiene las ids de las mascotas
	unsigned int id;
	fread(&id, sizeof(int), 1, fptc);
	newDog->id = id;
	id += 1;
	rewind(fptc);
	fwrite(&id, sizeof(int), 1, fptc); // Escribe la id de la nueva mascota
	checkfclose(fptc, CURRENT_ID_PATH);

	int code = htHashFunction(newDog->name); // Calcula la hash de la nueva mascota
	int next = hashTable[code]; // Obtiene la posición del anterior con la misma hash
  FILE* fptr = checkfopen(DATA_DOGS_PATH, "r+");
	if(next == -1){ // Si la hash del nombre no tiene entradas, crea una entrada con la posición de la nueva mascota
		fseek(fptr, 0, SEEK_END);
		hashTable[code] = ftell(fptr);
	}
	struct dogType* currDog = malloc(sizeof(struct dogType));
	while(next > 0){ // Actualiza los next de la hash de ese nombre
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
	newDog->position = ftell(fptr)/sizeof(struct dogType) + 1;
	fwrite(newDog, sizeof(struct dogType), 1, fptr);
	sprintf(registry, "%u", newDog->id);
  free(newDog);
  free(currDog);
	checkfclose(fptr, DATA_DOGS_PATH); // Cierra dataDogs
	generateLog("Inserción", args->ip, registry, "Sin cadena de busqueda"); // Genera el log de la inserción
	menu(args);
}

void showReg(void *ptr){
	struct threadArgs *args = ptr; // Inicializa variables de operación
	int clientsd = args->clientsd;
	FILE* fptr = checkfopen(DATA_DOGS_PATH, "r");
	char registryString [10];
	char searchedRegistry[10];
	bzero(registryString, (int)sizeof(registryString));
	bzero(searchedRegistry, (int)sizeof(searchedRegistry));
	fseek(fptr, 0, SEEK_END);
	int totalSize = ftell(fptr) / (sizeof(struct dogType)); // Calcula el tamaño de dataDogs
  int r = send(clientsd, &totalSize, sizeof(int), 0); // Lo envía al cliente
  if(r == -1){
    perror("Send error\n");
    exit(-1);
  }

  int numReg;
  r = recv(clientsd, &numReg, sizeof(int), MSG_WAITALL); //Recibe del cliente el registro a ver
  if(r != sizeof(int)){
    perror("Recv error");
    exit(-1);
  }
  struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
  fseek(fptr, numReg*sizeof(struct dogType), SEEK_SET);
  fread(newDog, sizeof(struct dogType), 1, fptr);
	// showFullDogType(newDog);
  r = send(clientsd, newDog, sizeof(struct dogType), 0); //Envia la mascota deseada al cliente
  if(r == -1){
    perror("Send error\n");
    exit(-1);
  }
	sprintf(searchedRegistry, "%i", newDog->id);
	sprintf(registryString, "%d", numReg+1);
  checkfclose(fptr, DATA_DOGS_PATH);
  //printf("Consulta de registro exitosa\n");
	generateLog("Lectura", args->ip, registryString, searchedRegistry); //Genera el log de la consulta
	openHistory(args, newDog->id);
	free(newDog);

	menu(args);
}

void recvDeleteReg(void *ptr){
	struct threadArgs *args = ptr; //Inicializa las variables de Eliminación
	int r, i=0, numReg, filePointer, clientsd = args->clientsd, code;
	char consultedDog[10];
	char returnedDog[10];
	bzero(consultedDog, (int)sizeof(consultedDog));
	bzero(returnedDog, (int)sizeof(returnedDog));
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r"); // Abre dataDogs
	fseek(dataDogs, 0, SEEK_END);
	int totalSize = ftell(dataDogs) / (sizeof(struct dogType));
	rewind(dataDogs);

  r = send(clientsd, &totalSize, sizeof(int), 0); // Envía el tamaño de dataDogs
  if(r == -1){
    perror("Send error\n");
    exit(-1);
  }

  r = recv(clientsd, &numReg, sizeof(int), MSG_WAITALL); // Recibe el número del registro a eliminar
  if(r != sizeof(int)){
    perror("Recv error");
    exit(-1);
  }

	FILE* tempDataDogs = checkfopen(TEMP_DATA_DOGS_PATH, "w"); // Crea el archivo temporal de eliminación
  struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType));
	for(i=0; i<totalSize; i++){ //Copia todos los registros en el archivo nuevo
		filePointer = ftell(dataDogs);
		fread(currDog, sizeof(struct dogType), 1, dataDogs);
		if(filePointer == numReg*sizeof(struct dogType)){ //Cuando encuentra la mascota a eliminar, la envía al cliente para su visualización
			sprintf(returnedDog, "%d", currDog->id);
			r = send(clientsd, currDog, sizeof(struct dogType), 0);
			if(r == -1){
			  perror("Send error\n");
			  exit(-1);
			}
			continue; //Salta la copia de la estructura a eliminar
		}
		if(filePointer > numReg*sizeof(struct dogType)) // Actualiza la posición de las estructuras que están adelante de la que se eliminó
			currDog->position -= 1;
		// Actualiza los next de las estructuras que sigan
		if(currDog->next == 0){
			// en caso de que el registro que se quiera eliminar sea el primero (numReg = 0)
		}else if(currDog->next > numReg*sizeof(struct dogType)){
			currDog->next -= sizeof(struct dogType);
		}else if(currDog->next == numReg*sizeof(struct dogType)){
			fseek(dataDogs, currDog->next, SEEK_SET);
			fread(&currDog->next, sizeof(int), 1, dataDogs);
			currDog->next -= sizeof(struct dogType);
			fseek(dataDogs, filePointer + sizeof(struct dogType), SEEK_SET);
		}
		fwrite(currDog, sizeof(struct dogType), 1, tempDataDogs);
	}

	free(currDog); // Libera la estructura auxiliar
	checkfclose(dataDogs, DATA_DOGS_PATH); //Cierra dataDogs y tempDataDogs
	checkfclose(tempDataDogs, TEMP_DATA_DOGS_PATH);
	remove(DATA_DOGS_PATH); //Elimina dataDogs renombra tempDataDogs como dataDogs
	rename(TEMP_DATA_DOGS_PATH, DATA_DOGS_PATH);

	htInit(hashTable); //Recarga la hash table
	htLoad(hashTable);
	sprintf(consultedDog, "%d", numReg+1);
	generateLog("Eliminación", args->ip, returnedDog, consultedDog); //Genera el log de le eliminación

	// Envía la confirmación de la eliminación al cliente
	int success = 1;
	r = send(clientsd, &success, sizeof(int), 0);
	if(r == -1){
		perror("Send error\n");
		exit(-1);
	}
	menu(args);
}

void showSearch(void *ptr){
	struct threadArgs *args = ptr; //Inicializa las variables de búsqueda
	int clientsd = args->clientsd;
	int success = 0, i = 0, j = 0;
	char name[NAME_SIZE];
  int r = recv(clientsd, name, NAME_SIZE, MSG_WAITALL); // Recibe del cliente el nombre a buscar
  if(r != NAME_SIZE){
    perror("Recv error");
    exit(-1);
  }
  int code = htHashFunction(name);
	int exists = hashTable[code] == -1 ? 0 : 1; //Verifica si el nombre existe
	r = send(clientsd, &exists, sizeof(int), 0); //Envía el resultado de la verificación al cliente
	if(r == -1){
		perror("Send error\n");
		exit(-1);
	}

  if(!exists){ // Si no existe le informa al cliente y genera el log de la búsqueda
		generateLog("Busqueda", args->ip, "No encontrado", name);
		menu(args);
		return;
	}
	// Siempre tendrá al menos un Valor
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r"); //Abre dataDogs
	struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType)); //Reserva el espacio de la estructura auxiliar
	//poner en minusculas name
	char dogNameAux[NAME_SIZE];
	char nameAux[NAME_SIZE];
	strcpy(nameAux, name);

	for(j = 0; j<strlen(nameAux); j++)
		nameAux[j] = tolower(nameAux[j]);
		int next = hashTable[code];
		int hasDog = 0;
	do{
		fseek(dataDogs, next, SEEK_SET); // Ubica el descriptor en el inicio de una estructura con la hash del nombre a buscar
		fread(newDog, sizeof(struct dogType), 1, dataDogs); //Lee la estructura
		strcpy(dogNameAux, newDog->name); //Copia el nombre

		for(i = 0; i<strlen(dogNameAux); i++){
			dogNameAux[i] = tolower(dogNameAux[i]);
		}
		if(!strcmp(dogNameAux, nameAux)){ // Si el nombre buscado coincide con el nombre de la hash
			hasDog = 1;
			if(success==0){
				success = 1;
				hasDog = 2;
			}
			r = send(clientsd, &hasDog, sizeof(int), 0); // Envía la confirmación de existencia al cliente
			if(r == -1){
				perror("Send error\n");
				exit(-1);
			}
			send(clientsd, newDog, sizeof(struct dogType), 0); // Envía al cliente la mascota encontrada
		}
		next = newDog->next; // Actualiza el next para buscar si la siguiente mascota de la hash coincide con el nombre a buscar
	}while(next != 0); // Se ejecuta mientras aún hayan entradas en la hash

	hasDog = 0;
	r = send(clientsd, &hasDog, sizeof(int), 0); //
	if(r == -1){
		perror("Send error\n");
		exit(-1);
	}
	generateLog("Busqueda", args->ip, "Múltiples", name); // Genera el log de la búsqueda

	free(newDog); // Libera la estructura auxiliar
	checkfclose(dataDogs, DATA_DOGS_PATH); // Cierra dataDogs
	menu(args);
}

void openHistory(void *ptr, int dogId){
	struct threadArgs *args = ptr; //Inicializa las variables de historia clínica
	int clientsd = args->clientsd;
	int r;
	char ans;
	r = recv(clientsd, &ans, sizeof(char), MSG_WAITALL); //Recibe del cliente la solicitud de apertura de historia clínica
	if(r != sizeof(char)){
		perror("Recv error");
		exit(-1);
	}

	if(ans == 's' || ans == 'S'){ // Si es afirmativa
		struct stat file_stat;
		int fileSize, resp;
		char file_name[12];
		char file_name_edit[12];
		sprintf(file_name, "%i.txt", dogId);
		sprintf(file_name_edit, "~%i.txt", dogId);
		if(file_exist(file_name_edit)) //Si la historia clínica se esta editando actualmente
			resp = 2; // La respuesta a enviar al cliente es 2
		else
			if(file_exist(file_name)) //Si la historia clinica existe
				resp = 1; //La respuesta es 1
			else
				resp = 0; //si no existe es 0
		r = send(clientsd, &resp, sizeof(int), MSG_WAITALL); // Envía la respuesta al cliente
		if(r == -1){
			perror("Recv error");
			exit(-1);
		}

		// Si el archivo no existe
		if(resp == 0){
			FILE* edit = checkfopen(file_name_edit, "w"); //Crea el archivo de edición
			checkfclose(edit, file_name_edit); // Y lo cierra

			r = recv(clientsd, &fileSize, sizeof(int), MSG_WAITALL); //Recibe del cliente el tamaño de la historia clínica escrita
			if(r != sizeof(int)){
				perror("Recv error");
				exit(-1);
			}
			int remain_data = fileSize;
			sprintf(file_name, "%i.txt", dogId);
			FILE* myFile = checkfopen(file_name, "w"); //Crea la historia clínica en el servidor
			int len;
			char buffer[BUFSIZ];
			while ((remain_data > 0) && ((len = recv(clientsd, buffer, BUFSIZ, 0)) > 0)){ //Mientras haya información por enviar
				fwrite(buffer, sizeof(char), len, myFile); // Escribir en la historia clínica del servidor
				remain_data -= len;
			}
			checkfclose(myFile, file_name); // Cerrar la historia clínica
			remove(file_name_edit); //Remover el archivo de edición
		}

		else if(resp == 1){ // Si la historia clínica existe
			FILE* edit = checkfopen(file_name_edit, "w"); // Crea el archivo de edición
			checkfclose(edit, file_name_edit); // Lo cierra
			int fd = open(file_name, O_RDONLY); //Abre la historia clínica solicita en modo lectura
			if (fd == -1){
				perror("open error\n");
				exit(-1);
			}
			r = stat(file_name, &file_stat); // Crea el objeto que mide el tamaño de la historia clínica
			if (r == -1){
				perror("fstat error\n");
				exit(-1);
			}

			fileSize = (int)file_stat.st_size;
			r = send(clientsd, &fileSize, sizeof(int), 0); // Envía el tamaño de la historia clínica al cliente
			if(r == -1){
				perror("send error");
				exit(-1);
			}

			off_t offset = 0;
			int sent_bytes = 0;
			int remain_data = fileSize;
			while (((sent_bytes = sendfile(clientsd, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0)){ // Envía la historia clínica al cliente
				remain_data -= sent_bytes;
			}
			close(fd);

			r = recv(clientsd, &fileSize, sizeof(int), MSG_WAITALL); // Recibe el nuevo tamaño de la historia clínica
			if(r != sizeof(int)){
				perror("Recv error");
				exit(-1);
			}
			remain_data = fileSize;
			FILE* myFile = checkfopen(file_name, "w");
			int len;
      char buffer[BUFSIZ];
			while ((remain_data > 0) && ((len = recv(clientsd, buffer, BUFSIZ, 0)) > 0)){ //Recibe la nueva historia clínica desde el cliente
				fwrite(buffer, sizeof(char), len, myFile);
				remain_data -= len;
			}
			checkfclose(myFile, file_name); //Cierra la historia clínica ya editada
			remove(file_name_edit); //Elimina el archivo de edición
		}
	}
}

int file_exist(char *filename){ //confirma si existe la historia clínica 
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}
