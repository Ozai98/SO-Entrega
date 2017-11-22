#include <arpa/inet.h>
#include <pthread.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <semaphore.h>
#include "hashTable.c"
// Constantes de operación
#define BACKLOG 2
#define PORT 3535
#define SERVER_LOG_PATH "ServerDogs.log"
#define NUM_THREADS 32

#define MAX_PROCCESS 1
#define SEMAPHORE_NAME "semaphore_name"
#define SEMAPHORE_OPTION 1
#define MUTEX_OPTION 2
#define PIPE_OPTION 3


struct threadArgs{ // Estructura que contiene el descriptor de la socket y la IP de un cliente entrante
	int clientsd;
	char ip[INET6_ADDRSTRLEN];
};
int critialSectionControlOption = SEMAPHORE_OPTION; // Selector de la opcion a usar para el manejo de las secciones criticas
sem_t *semaphore; // Variable semaforo de control para las secciones criticas
static pthread_mutex_t *mymutex; // Variable mutex de control para las secciones criticas
int pipefd[2]; // Variable de tubería de control para las secciones criticas
char guard = 'T'; // Testigo a usar por la tubería de control para las secciones criticas
// Instanciación de las funciones
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
	struct threadArgs* args[NUM_THREADS]; // Inicializa variables del servidor
	int i, r, serversd, serverSize, optval;
	socklen_t clientSize;
	pthread_t threads[NUM_THREADS];
	struct sockaddr_in server, client[32];
	htInit(hashTable); // Inicializa la hashTable
	htLoad(hashTable); // Carga la hashTable
	printf("%s\n","Servidor listo");
	serversd = socket(AF_INET, SOCK_STREAM, 0); // Crea una socket
  if(serversd == -1){
    perror("Socket error\n");
    exit(-1);
  }

  // Configura la socket
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = INADDR_ANY;
  bzero(server.sin_zero, 8);
	serverSize = sizeof(struct sockaddr);

	// Configura la reutilización del puerto de comunicación
	optval = 1;

	if (setsockopt(serversd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");

  // Asigna el nombre a la socket
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


	// Selector de la opcion de control de secciones criticas a usar
	if(critialSectionControlOption == SEMAPHORE_OPTION){ // Inicialicialización del semaforo
		sem_unlink(SEMAPHORE_NAME); // Remueve el semaforo nombrado por SEMAPHORE_NAME
		// Crea o abre el semaforo nombrado por SEMAPHORE_NAME
		// con una cantidad de procesos en ejecución para las secciones criticas de MAX_PROCCESS
		semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0700, MAX_PROCCESS);
		if(semaphore == SEM_FAILED){
			perror("Semaphore create error");
			exit(-1);
		}
	}else if(critialSectionControlOption == MUTEX_OPTION){ // Inicialicialización del mutex
		mymutex = (void*)malloc(sizeof(pthread_mutex_t)); // Separa el espacio en memoria para el mutex
		if(mymutex == NULL){
			perror("Semaphore create error");
			exit(-1);
		}
		if(pthread_mutex_init(mymutex, NULL) != 0){ // Inicializa el mutex
			perror("pthread_mutex_init error");
			exit(-1);
		}
	}else if(critialSectionControlOption == PIPE_OPTION){ // Inicialicialización de la tubería
		if(pipe(pipefd) != 0) { // Crea la tubería y asigna los descriptores a piprfd
			perror("pipe error");
			exit(-1);
		}
		if(write(pipefd[1], &guard, sizeof(char)) != sizeof(char)) { // escribe el testigo una vez, para el uso de un solo proceso en las zonas críticas
			perror("write error");
			exit(-1);
		}
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

	// Selector de la opcion de control de secciones criticas a usar
	if(critialSectionControlOption == SEMAPHORE_OPTION){
		sem_close(semaphore); // Cierra el semaforo nombrado
	}else if(critialSectionControlOption == MUTEX_OPTION){
		if(pthread_mutex_destroy(mymutex) != 0){ // Destruye el mutex referenciado por mymutex
			perror("pthread_mutex_destroy error");
			exit(-1);
		}
		free(mymutex); // libera el espacio en memoría usado por mymutex
	}
  close(serversd);
}

void *menu(void* ptr){
	// Inicializa las variables de operación
	struct threadArgs* args = ptr;
	int clientsd = args->clientsd;
  int option;
  checkRecv(clientsd, &option, sizeof(int), MSG_WAITALL, "option"); // Recibe la opción que el cliente quiere realizar y la ejecuta

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
			close(args->clientsd);
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
	char serverLogString[index];
	char date[100];
	char* currentDate = timeAndDate(date); // Obtiene la fecha y hora del servidor
	FILE* serverLog = checkfopen(SERVER_LOG_PATH, "a");
	snprintf(serverLogString, sizeof(serverLogString), "[Fecha: %s] [Cliente: %s] [Operacion: %s] [Registro: %s] [Cadena buscada: %s]\n",
	 																										currentDate, 		ip, 				opType, 				registry, 				searchedString);
	const char *ptr = strchr(serverLogString, '\n');

	if(ptr)
   index = ptr - serverLogString;

	fseek(serverLog,0, SEEK_END);
	fwrite(serverLogString, 1, index+1, serverLog);
	checkfclose(serverLog, SERVER_LOG_PATH);
}

void recvNewReg(void *ptr){
	// Inicializa variables de adición
	struct threadArgs *args = ptr;
	int clientsd = args->clientsd;
  int j;
	char registry[10];
	bzero(registry, (int)sizeof(registry));
  struct dogType *newDog = (struct dogType*) malloc(sizeof(struct dogType)); // Reserva espacio para estructura temporal
	checkRecv(clientsd, newDog, sizeof(struct dogType), MSG_WAITALL, "newDog"); // Recibe la estructura creada por el cliente

	FILE* fptc = checkfopen(CURRENT_ID_PATH, "r+"); // Abre el archivo que contiene las ids de las mascotas
	unsigned int id;
	fread(&id, sizeof(int), 1, fptc);
	newDog->id = id; // Obtiene el id de la nueva mascota
	id += 1;
	rewind(fptc);
	fwrite(&id, sizeof(int), 1, fptc); // Escribe la id de la nueva mascota
	checkfclose(fptc, CURRENT_ID_PATH); // Cierra fptc
	int code = (int)htHashFunction(newDog->name); // Calcula la hash de la nueva mascota
	int next = hashTable[code]; // Obtiene la posición del anterior con la misma hash

	// Selector de opcion de control de secciones criticas a usar
	if(critialSectionControlOption == SEMAPHORE_OPTION){
		if(sem_wait(semaphore) == -1){ // Decrementa el semaforo
			perror("Semaphore wait error");
			exit(-1);
		}
	}else if(critialSectionControlOption == MUTEX_OPTION){
		if(pthread_mutex_lock(mymutex) != 0){ // Bloquea el mutex
			perror("pthread_mutex_lock error");
			exit(-1);
		}
	}else if(critialSectionControlOption == PIPE_OPTION){
		char result;
		if(read(pipefd[0], &result, sizeof(char)) != sizeof(char)){ // Lee el testigo escrito en la tubería
			perror("read error");
			exit(-1);
		}
	}
  FILE* fptr = checkfopen(DATA_DOGS_PATH, "r+"); //Abre dataDogs

	if(next == -1){ // Si la hash del nombre no tiene entradas, crea una entrada con la posición de la nueva mascota
		fseek(fptr, 0, SEEK_END);
		hashTable[code] = ftell(fptr);
	}

	struct dogType* currDog = malloc(sizeof(struct dogType));
	while(next > 0){ // Actualiza los next de la hash de ese nombre
		fseek(fptr, next, SEEK_SET);
		fread(currDog, sizeof(struct dogType), 1, fptr);

		if(currDog->next == 0){
			fseek(fptr, 0, SEEK_END);
			currDog->next = (int)ftell(fptr);
			fseek(fptr, next, SEEK_SET);
			fwrite(&currDog->next, sizeof(int), 1, fptr);
			break;
		}
		next = currDog->next;
	}

	// Añade la estructura a la hashTable y a dataDogs.dat
	fseek(fptr, 0, SEEK_END);
	newDog->position = ftell(fptr)/sizeof(struct dogType) + 1;
	fwrite(newDog, sizeof(struct dogType), 1, fptr);
	sprintf(registry, "%u", newDog->id);
  free(newDog);
  free(currDog);
	checkfclose(fptr, DATA_DOGS_PATH); // Cierra dataDogs

	// Selector de opcion de control de secciones criticas a usar
	if(critialSectionControlOption == SEMAPHORE_OPTION){
		if(sem_post(semaphore) == -1){ // Incrementa el semaforo
			perror("Semaphore post error");
			exit(-1);
		}
	}else if(critialSectionControlOption == MUTEX_OPTION){
		if(pthread_mutex_unlock(mymutex) != 0){ // Desbloquea el mutex
			perror("pthread_mutex_unlock error");
			exit(-1);
		}
	}else if(critialSectionControlOption == PIPE_OPTION){
		if(write(pipefd[1], &guard, sizeof(char)) != sizeof(char)){ // Escribe el testigo en la tubería
			perror("write error");
			exit(-1);
		}
	}
	generateLog("Inserción", args->ip, registry, "Sin cadena de busqueda"); // Genera el log de la inserción
	menu(args);
}

void showReg(void *ptr){
	struct threadArgs *args = ptr; // Inicializa variables de visualización
	int clientsd = args->clientsd;
	FILE* fptr = checkfopen(DATA_DOGS_PATH, "r");
	char registryString [10];
	char searchedRegistry[10];
	bzero(registryString, (int)sizeof(registryString));
	bzero(searchedRegistry, (int)sizeof(searchedRegistry));
	fseek(fptr, 0, SEEK_END);
	int totalSize = ftell(fptr) / (sizeof(struct dogType)); // Calcula el tamaño de dataDogs

	checkSend(clientsd, &totalSize, sizeof(int), 0, "totalSize"); // Lo envía al cliente
  int numReg;

	checkRecv(clientsd, &numReg, sizeof(int), MSG_WAITALL, "numReg"); // Recibe del cliente el registro a ver

	struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType)); // Captura la mascota solicitada
  fseek(fptr, numReg*sizeof(struct dogType), SEEK_SET);
  fread(newDog, sizeof(struct dogType), 1, fptr);

	checkSend(clientsd, newDog, sizeof(struct dogType), 0, "newDog"); // Envia la mascota deseada al cliente
  sprintf(searchedRegistry, "%i", newDog->id);
	sprintf(registryString, "%d", numReg+1);
  checkfclose(fptr, DATA_DOGS_PATH);
	generateLog("Lectura", args->ip, registryString, searchedRegistry); // Genera el log de la consulta
	openHistory(args, newDog->id);
	free(newDog);
	menu(args);
}

void recvDeleteReg(void *ptr){
	struct threadArgs *args = ptr; // Inicializa las variables de Eliminación
	int r, i=0, numReg, filePointer, clientsd = args->clientsd, code;
	char consultedDog[10];
	char returnedDog[10];
	bzero(consultedDog, (int)sizeof(consultedDog));
	bzero(returnedDog, (int)sizeof(returnedDog));
	// Selector de opcion de control de secciones criticas a usar
	if(critialSectionControlOption == SEMAPHORE_OPTION){
		if(sem_wait(semaphore) == -1){ // Decrementa el semaforo
			perror("Semaphore wait error");
			exit(-1);
		}
	}else if(critialSectionControlOption == MUTEX_OPTION){
		if(pthread_mutex_lock(mymutex) != 0){ // Bloquea el mutex
			perror("pthread_mutex_lock error");
			exit(-1);
		}
	}else if(critialSectionControlOption == PIPE_OPTION){
		char result;
		if(read(pipefd[0], &result, sizeof(char)) != sizeof(char)) { // Lee el testigo escrito en la tubería
			perror("read error");
			exit(-1);
		}
	}
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r"); // Abre dataDogs
	fseek(dataDogs, 0, SEEK_END);
	int totalSize = ftell(dataDogs) / (sizeof(struct dogType)); // Calcula el tamaño de dataDogs
	rewind(dataDogs);

  checkSend(clientsd, &totalSize, sizeof(int), 0, "totalSize"); // Envía el tamaño de dataDogs
  checkRecv(clientsd, &numReg, sizeof(int), MSG_WAITALL, "numReg"); // Recibe el número del registro a eliminar
  FILE* tempDataDogs = checkfopen(TEMP_DATA_DOGS_PATH, "w"); // Crea el archivo temporal de eliminación
  struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType)); // Reserva el espacio de la estructura temporal

	for(i = 0; i < totalSize; i++){ // Copia todos los registros en el archivo nuevo
		filePointer = ftell(dataDogs);
		fread(currDog, sizeof(struct dogType), 1, dataDogs); // Lee una mascota de dataDogs

		if(filePointer == numReg*sizeof(struct dogType)){ // Si la mascota leída es la que desea eliminar, se la envía al cliente para su visualización
			sprintf(returnedDog, "%d", currDog->id);
			checkSend(clientsd, currDog, sizeof(struct dogType), 0, "currDog");
			continue; // Salta la copia de la estructura a eliminar
		}

		if(filePointer > numReg*sizeof(struct dogType)) // Si la estructura a copiar esta adelante de la que se va a eliminar, actualiza su posición
			currDog->position -= 1;

		// Actualiza los next de las estructuras que sigan
		if(currDog->next > numReg*sizeof(struct dogType)){ // En caso de que la referencia al siguiente, sea posterior al registro a eliminar
			// Decrementa su valor en una estructura o deja en cero en caso de que el registro sea el ultimo en la lista enlazada
			currDog->next = currDog->next==0 ? 0 : currDog->next - sizeof(struct dogType);
		}else if(currDog->next == numReg*sizeof(struct dogType)){ // En caso de que referencie al registro a eliminar
			// Se posiciona en registro a eliminar
			fseek(dataDogs, currDog->next, SEEK_SET);
			// Guarda en la referencia al siguiente, el valor que tenía el registro a eliminar
			fread(&currDog->next, sizeof(int), 1, dataDogs);
			// Decrementa su valor en una estructura o deja en cero en caso de que el registro a eliminar sea el ultimo en la lista enlazada
			currDog->next = currDog->next==0 ? 0 : currDog->next - sizeof(struct dogType);
			// Reposiciona el indicador de posición del archivo al estado imediatamente posterior a la lectura de la estuctura actual currDog
			fseek(dataDogs, filePointer + sizeof(struct dogType), SEEK_SET);
		}
		// Escribe la estructura actual en el archivo temporal
		fwrite(currDog, sizeof(struct dogType), 1, tempDataDogs);
	}

	free(currDog); // Libera la estructura auxiliar
	checkfclose(dataDogs, DATA_DOGS_PATH); // Cierra dataDogs y tempDataDogs
	checkfclose(tempDataDogs, TEMP_DATA_DOGS_PATH);
	remove(DATA_DOGS_PATH); // Elimina dataDogs renombra tempDataDogs como dataDogs
	rename(TEMP_DATA_DOGS_PATH, DATA_DOGS_PATH);

	// Selector de opcion de control de secciones criticas a usar
	if(critialSectionControlOption == SEMAPHORE_OPTION){
		if(sem_post(semaphore) == -1){ // Incrementa el semaforo
			perror("Semaphore post error");
			exit(-1);
		}
	}else if(critialSectionControlOption == MUTEX_OPTION){
		if(pthread_mutex_unlock(mymutex) != 0){ // Desbloquea el mutex
			perror("pthread_mutex_unlock error");
			exit(-1);
		}
	}else if(critialSectionControlOption == PIPE_OPTION){
		if(write(pipefd[1], &guard, sizeof(char)) != sizeof(char)){ // Escribe el testigo en la tubería
			perror("write error");
			exit(-1);
		}
	}

	htInit(hashTable); // Recarga la hash table
	htLoad(hashTable);
	sprintf(consultedDog, "%d", numReg+1);
	generateLog("Eliminación", args->ip, returnedDog, consultedDog); // Genera el log de le eliminación

	// Envía la confirmación de la eliminación al cliente
	int success = 1;
	checkSend(clientsd, &success, sizeof(int), 0, "success");
	menu(args);
}

void showSearch(void *ptr){
	struct threadArgs *args = ptr; // Inicializa las variables de búsqueda
	int clientsd = args->clientsd;
	int success = 0, i = 0, j = 0;
	char name[NAME_SIZE];

	checkRecv(clientsd, name, NAME_SIZE, MSG_WAITALL,"name"); // Recibe del cliente el nombre a buscar
  int code = (int)htHashFunction(name); // Cacula la hash del nombre
	int exists = hashTable[code] == -1 ? 0 : 1; // Verifica si el nombre existe
	checkSend(clientsd, &exists, sizeof(int), 0, "exists"); // Envía el resultado de la verificación al cliente

  if(!exists){ // Si no existe le informa al cliente, genera el log de la búsqueda y sale de la operación
		generateLog("Busqueda", args->ip, "No encontrado", name);
		menu(args);
		return;
	}
	// Selector de opcion de control de secciones criticas a usar
	if(critialSectionControlOption == SEMAPHORE_OPTION){
		if(sem_wait(semaphore) == -1){ // Decrementa el semaforo
			perror("Semaphore wait error");
			exit(-1);
		}
	}else if(critialSectionControlOption == MUTEX_OPTION){
		if(pthread_mutex_lock(mymutex) != 0){ // Bloquea el mutex
			perror("pthread_mutex_lock error");
			exit(-1);
		}
	}else if(critialSectionControlOption == PIPE_OPTION){
		char result;
		if(read(pipefd[0], &result, sizeof(char)) != sizeof(char)) { // Lee el testigo escrito en la tubería
			perror("read error");
			exit(-1);
		}
	}
	// Siempre tendrá al menos un Valor
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r"); //Abre dataDogs
	struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType)); // Reserva el espacio de la estructura auxiliar

	char dogNameAux[NAME_SIZE]; // Crea variables auxiliares
	char nameAux[NAME_SIZE];
	strcpy(nameAux, name);

	for(j = 0; j < strlen(nameAux); j++)
		nameAux[j] = tolower(nameAux[j]);

	int next = hashTable[code];
	int hasDog = 0;

	do{
		fseek(dataDogs, next, SEEK_SET); // Ubica el descriptor en el inicio de una estructura con la hash del nombre a buscar
		fread(newDog, sizeof(struct dogType), 1, dataDogs); // Lee la estructura
		strcpy(dogNameAux, newDog->name); // Copia el nombre

		for(i = 0; i<strlen(dogNameAux); i++){
			dogNameAux[i] = tolower(dogNameAux[i]);
		}
		if(!strcmp(dogNameAux, nameAux)){ // Si el nombre buscado coincide con el nombre de la hash
			hasDog = 1;
			if(success==0){
				success = 1;
				hasDog = 2;
			}
			checkSend(clientsd, &hasDog, sizeof(int), 0, "hasDog"); // Envía la confirmación de existencia al cliente
			checkSend(clientsd, newDog, sizeof(struct dogType), 0, "newDog"); // Envía al cliente la mascota encontrada
		}
		next = newDog->next; // Actualiza el next para buscar si la siguiente mascota de la hash coincide con el nombre a buscar
	}while(next != 0); // Se ejecuta mientras aún hayan entradas en la hash

	hasDog = 0;
	checkSend(clientsd, &hasDog, sizeof(int), 0, "hasDog"); //
	generateLog("Busqueda", args->ip, "Múltiples", name); // Genera el log de la búsqueda
	free(newDog); // Libera la estructura auxiliar
	checkfclose(dataDogs, DATA_DOGS_PATH); // Cierra dataDogs

	// Selector de opcion de control de secciones criticas a usar
	if(critialSectionControlOption == SEMAPHORE_OPTION){
		if(sem_post(semaphore) == -1){ // Incrementa el semaforo
			perror("Semaphore post error");
			exit(-1);
		}
	}else if(critialSectionControlOption == MUTEX_OPTION){
		if(pthread_mutex_unlock(mymutex) != 0){ // Desbloquea el mutex
			perror("pthread_mutex_unlock error");
			exit(-1);
		}
	}else if(critialSectionControlOption == PIPE_OPTION){
		if(write(pipefd[1], &guard, sizeof(char)) != sizeof(char)){ // Escribe el testigo en la tubería
			perror("write error");
			exit(-1);
		}
	}
	menu(args);
}

void openHistory(void *ptr, int dogId){
	struct threadArgs *args = ptr; // Inicializa las variables de historia clínica
	int clientsd = args->clientsd;
	int r;
	char ans;
	checkRecv(clientsd, &ans, sizeof(char), MSG_WAITALL, "ans"); // Recibe del cliente la solicitud de apertura de historia clínica

	if(ans == 's' || ans == 'S'){ // Si es afirmativa
		struct stat file_stat;
		int fileSize, resp;
		char file_name[12];
		char file_name_edit[12];
		sprintf(file_name, "%i.txt", dogId);
		sprintf(file_name_edit, "~%i.txt", dogId);
		if(file_exist(file_name_edit)) // Si la historia clínica se esta editando actualmente
			resp = 2; 									 // La respuesta a enviar al cliente es 2
		else
			if(file_exist(file_name)) // Si la historia clinica existe y no esta abierta
				resp = 1; 						  // La respuesta es 1
			else
				resp = 0; // Si no existe es 0

		checkSend(clientsd, &resp, sizeof(int), MSG_WAITALL, "resp"); // Envía la respuesta al cliente

		// Si el archivo no existe
		if(resp == 0){
			FILE* edit = checkfopen(file_name_edit, "w"); // Crea el archivo que indica que esta historia clínica se encuentra abierta
			checkfclose(edit, file_name_edit); 					  // Y lo cierra

			checkRecv(clientsd, &fileSize, sizeof(int), MSG_WAITALL, "fileSize"); // Recibe del cliente el tamaño de la nueva historia clínica
			int remain_data = fileSize; // Variable que cuenta la información pendiente por enviar
			sprintf(file_name, "%i.txt", dogId);
			FILE* myFile = checkfopen(file_name, "w"); // Crea la historia clínica en el servidor
			int len;
			char buffer[BUFSIZ];
			while ((remain_data > 0) && ((len = checkRecv(clientsd, buffer, BUFSIZ, 0, "buffer")) > 0)){ // Mientras haya información por enviar
				fwrite(buffer, sizeof(char), len, myFile); // Escribir en la historia clínica del servidor
				remain_data -= len; //Reducir la cantidad de información pendiente
			}
			checkfclose(myFile, file_name); // Cerrar la historia clínica
			remove(file_name_edit); // Remover el archivo de edición
		}

		else if(resp == 1){ // Si la historia clínica existe
			FILE* edit = checkfopen(file_name_edit, "w"); // Crea el archivo que indica que esta historia clínica se encuentra abierta
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

			fileSize = (int)file_stat.st_size; // Genera el tamaño de la historia clinica
			checkSend(clientsd, &fileSize, sizeof(int), 0, "fileSize"); // Envía el tamaño de la historia clínica al cliente
			off_t offset = 0;
			int sent_bytes = 0;
			int remain_data = fileSize; // Variable que cuenta la información pendiente por enviar
			while (((sent_bytes = sendfile(clientsd, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0)){ // Envía la historia clínica al cliente
				remain_data -= sent_bytes;
			}
			close(fd);

			checkRecv(clientsd, &fileSize, sizeof(int), MSG_WAITALL, "fileSize"); // Recibe el nuevo tamaño de la historia clínica
			remain_data = fileSize;
			FILE* myFile = checkfopen(file_name, "w");
			int len;
      char buffer[BUFSIZ];
			while ((remain_data > 0) && ((len = checkRecv(clientsd, buffer, BUFSIZ, 0, "buffer")) > 0)){ // Recibe la nueva historia clínica desde el cliente
				fwrite(buffer, sizeof(char), len, myFile);
				remain_data -= len;
			}
			checkfclose(myFile, file_name); // Cierra la historia clínica ya editada
			remove(file_name_edit); // Elimina el archivo de edición
		}
	}
}

int file_exist(char *filename){ // Confirma si existe la historia clínica
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}
