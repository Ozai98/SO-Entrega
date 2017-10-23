#include <unistd.h>
#include "hashTable.c"
#include <ctype.h>
#include <termios.h>
#include <stdio_ext.h>

//Declaración de las funciones
void menu();
void exeMenu();
void addReg();
void seeReg();
void deleteReg();
void searchReg();

int hashTable[HASH_TABLE_SIZE];
int main(){
	htInit(hashTable);
	htLoad(hashTable);
	menu();
	return 0;
}
// Menú que despliega las opciones
void menu(){
	char input[32];
	int num = 0;
	printf("--------------------------------------------------\n");
	printf("\t\tTienda de mascotas\n");
	printf("--------------------------------------------------\n");
	printf("\t1\tIngresar registro\n");
	printf("\t2\tVer registro\n");
	printf("\t3\tBorrar registro\n");
	printf("\t4\tBuscar registro\n");
	printf("\t5\tSalir\n");
	printf("--------------------------------------------------\n");

	/*En caso de no ingresar un numero dentro de las opciones expuestas,
	  se volverá a solicitar el ingreso de la opcion*/
	do{
		printf("Ingrese la opcion deseada: ");
		scanf("%32s", input);
		num = input[0]-48;
		printf("Opcion %s: ", input);
		if (strlen(input) != 1 || num < 1 || num > 5)
			printf("%s\n", "El valor ingresado no es valido, intente de nuevo");
	}while(strlen(input) != 1 || num < 1 || num > 5);

	switch(num){
		case 1:
			printf("Ingresar registro\n");
			addReg();
			break;
		case 2:
			printf("Ver registro\n");
			seeReg();
			break;
		case 3:
			printf("Borrar registro\n");
			deleteReg();
			break;
		case 4:
			printf("Buscar registro\n");
			searchReg();
			break;
		case 5:
			printf("Salir\n");
			break;
		default:
			printf("default\n");
			break;
	}
}
//Función que reejecuta el menú
void exeMenu(){
  static struct termios newt, oldt;
  printf("\nPresione una tecla para continuar...");
  tcgetattr( STDIN_FILENO, &oldt);  //Obteniendo la configuracion actual de la terminal
  newt = oldt;        //Copiando la configuracion
	//	Desactivando las banderas ECHO e ICANON
	newt.c_lflag &= ~(ICANON | ECHO);  //Desactivando la bandera ICANON para no esperar un caracter especial al final
  newt.c_cc[VMIN] = 1;
  tcsetattr( STDIN_FILENO, TCSANOW, &newt);//Configurando la terminal con los nuevos cambios
	__fpurge(stdin);      //Limpiando el buffer de entrada
  getchar();        //Esperando un caracter
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);//Configurando la terminal a su forma original
	printf("\n");
	menu();
}
// Función que añade un registro a al archivo
void addReg(){
	char term;
	char petName[NAME_SIZE];
	char type[TYPE_SIZE];
	char  breed[BREED_SIZE];
	char sex;
	int age;
	int height;
	float weight;

	int rightValue = 0;
	int next;
	int code = 0;
	int i;

	//Captura de los datos de la mascota
	printf("%s", "Ingrese el nombre de la mascota, este no debe superar los 32 caracteres,\nsi lo hace solo se guardaran los primeros 32: ");
	getchar();
	fgets(petName, NAME_SIZE, stdin);
	petName[strlen(petName)-1]=0;
	printf("Nombre ingresado: %s\n", petName);

	printf("%s", "Ingrese el tipo de la mascota, este no debe superar los 32 caracteres \nsi lo hace solo se guardaran los primeros 32: ");
	fgets(type, NAME_SIZE, stdin);
	type[strlen(type)-1]=0;
	printf("Tipo ingresado: %s\n", type);

	printf("%s","Ingrese la raza de la mascota, este no debe superar los 32 caracteres, \nsi lo hace solo se guardaran los primeros 32: ");
	fgets(breed, NAME_SIZE, stdin);
	breed[strlen(breed)-1]=0;
	printf("Raza ingresada: %s\n", breed);
	printf("%s", "Ingrese el sexo 'H' para Hembra o 'M' para Macho: ");
	fflush(stdin);
	scanf(" %c", &sex);
	while(sex != 'H' && sex != 'M' && sex != 'm' && sex != 'h'){
		printf("%s\n", "Ingrese el sexo 'H' o 'M'");
		scanf(" %c", &sex);
	}
	printf("Sexo ingresado: %c\n", sex);

	printf("Ingrese la edad de la mascota: ");
	do{
		getchar();
		if(scanf("%d%c", &age, &term) != 2 || term != '\n')
		    printf("Valor no valido, intente de nuevo: ");
		else
			rightValue = 1;
	}while(rightValue == 0);
	printf("La edad introducida es: %d\n", age);

	printf("Ingrese la altura de la mascota (En cm): ");
	rightValue = 0;
	do{
		if(scanf(" %d%c", &height, &term) != 2 || term != '\n')
				printf("Valor no valido, intente de nuevo. ");
		else
			rightValue = 1;
	}while(rightValue == 0);
	printf("La altura introducida es: %d\n", height);

	printf("Ingrese el peso de la mascota (En kg): ");
	rightValue = 0;
	do{
		if(scanf(" %f%c", &weight, &term) != 2 || term != '\n')
		    printf("Valor no valido, intente de nuevo. ");
		else
			rightValue = 1;
	}while(rightValue == 0);
	printf("El peso introducido es: %f\n", weight);

	//Creación de la estructura
	struct dogType* newDog = malloc(sizeof(struct dogType));
	FILE* fptr = checkfopen(DATA_DOGS_PATH, "r+");
	FILE* fptc = checkfopen(CURRENT_ID_PATH, "r+");
	unsigned int id;
	fread(&id, sizeof(int), 1, fptc);
	newDog->id = id;
	id += 1;
	rewind(fptc);
	fwrite(&id, sizeof(int), 1, fptc);
	checkfclose(fptc, CURRENT_ID_PATH);

	strcpy(newDog->name, petName);
	strcpy(newDog->type, type);
	strcpy(newDog->breed, breed);
	newDog->age = age;
	newDog->height = height;
	newDog->weight = weight;
	newDog->gender = sex;
	newDog->next = 0;

	//Muestra la estructura creada
	showDogType(newDog);

	//Cambia el next de la estructura anterior del mismo nombre
	code = htHashFunction(petName);
	next = hashTable[code];
	struct dogType* currDog = malloc(sizeof(struct dogType));
	if(next == -1){
		fseek(fptr, 0, SEEK_END);
		hashTable[code] = (int)ftell(fptr);
	}
	while(next > 0){
		fseek(fptr, next, SEEK_SET);
		fread(currDog, sizeof(struct dogType), 1, fptr);
		// showFullDogType(currDog);
		if(currDog->next == 0){
			fseek(fptr, 0, SEEK_END);
			currDog->next = (int)ftell(fptr);
			fseek(fptr, next, SEEK_SET);
			fwrite(currDog, sizeof(struct dogType), 1, fptr);
			break;
		}
		next = currDog->next;
	}
	free(currDog);

	//Añade la estructura a la hashTable y a dataDogs.dat
	fseek(fptr, 0, SEEK_END);
	fwrite(newDog, sizeof(struct dogType), 1, fptr);

	//Libera el espacio en memoria de la estructura cierra el archivo dataDogs.dat y reejecuta el menú
	free(newDog);
	checkfclose(fptr, DATA_DOGS_PATH);
	printf("Registro añadido exitosamente.\n");
	exeMenu();
}

//	Función que permite ver un registro de dataDogs.dat
void seeReg(){
	int numberReg;
	int value = 0;
	int totalSize;
	char ans;
	FILE* fptr = checkfopen(DATA_DOGS_PATH, "r");
	//Calcula el numero de registros
	fseek(fptr, 0, SEEK_END);
	totalSize = ftell(fptr) / (sizeof(struct dogType));
	printf("El número de registros es de: %i\n",totalSize);
	do{
		printf("%s\n", "Ingrese el número del registro a consultar");
		scanf("%d", &numberReg);
		//Si el numero ingresado se sale del rango de registros el registro solicitado no existe
		if(numberReg < 1 || numberReg > totalSize)
			printf("%s\n", "Este registro no existe");
		else{
			numberReg -= 1;
			//Imprime el registro solicitado
			value = 1;
			struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
			fseek(fptr, numberReg*sizeof(struct dogType), SEEK_SET);
			fread(newDog, sizeof(struct dogType), 1, fptr);
			showFullDogType(newDog);
      free(newDog);
      checkfclose(fptr, DATA_DOGS_PATH);
			printf("Consulta de registro exitosa\n");

			printf("%s\n", "Desea abrir la historia clínica del registro seleccionado Escriba S o N");
			scanf(" %c", &ans);
			//Si la respuesta es afirmativa, abre la historia clinima
			if(ans == 's' || ans == 'S'){
				char file_name_2[12];
				sprintf(file_name_2, "gedit %d.txt", (numberReg+1));
				system(file_name_2);
			}
			//Si no, reejecuta el menu
			exeMenu();
		}
	}while (value == 0);
}

//	Función que elimina un registro de datadogs.dat
void deleteReg(){
	int i = 0;
	int filePointer = 0;
	int code = 0;
	int delReg = 0;
	int success = 0;

	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
	FILE* tempDataDogs = checkfopen(TEMP_DATA_DOGS_PATH, "w");
	struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType));

	fseek(dataDogs, 0, SEEK_END);
	int nStructures = ftell(dataDogs)/sizeof(struct dogType);
	printf("El número de registros es de: %i\n",nStructures);
	rewind(dataDogs);

	printf("%s\n", "Ingrese la posición del registro a eliminar");
	scanf("%i", &delReg);
	if(delReg < 1 || delReg > nStructures){
		printf("%s\n", "El registro que desea eliminar no existe");
		checkfclose(dataDogs, DATA_DOGS_PATH);
		checkfclose(tempDataDogs, TEMP_DATA_DOGS_PATH);
	}else{
		delReg-=1;
		for(i; i<nStructures; i++){
			//Copia todos los registros en el archivo nuevo
			filePointer = ftell(dataDogs);
			fread(currDog, sizeof(struct dogType), 1, dataDogs);
			if(filePointer == delReg*sizeof(struct dogType)){
				//Si encuentra el archivo a eliminar, lo elimina de la hash y salta la copia de este registro en el nuevo archivo
				success = 1;
				continue;
			}
			currDog->next = 0;
			fwrite(currDog, sizeof(struct dogType), 1, tempDataDogs);
		}
		printf("%s\n", "Eliminación exitosa... espere por favor");

		checkfclose(dataDogs, DATA_DOGS_PATH);
		checkfclose(tempDataDogs, TEMP_DATA_DOGS_PATH);
		remove(DATA_DOGS_PATH);
		rename(TEMP_DATA_DOGS_PATH, DATA_DOGS_PATH);
		htInit(hashTable);
		htLoad(hashTable);
	}
	free(currDog);
	exeMenu();
}

//	Función que busca en dataDogs.dat las mascotas con el mismo nombre
void searchReg(){
	char petName[NAME_SIZE];
  printf("Ingrese el nombre de la mascota, este no debe superar los 32 caracteres, si lo hace solo se guardaran los primeros 32: ");
	getchar();
	fgets(petName, NAME_SIZE, stdin);
	petName[strlen(petName)-1]=0;
	if(htSearch(hashTable, petName)){
		printf("%s\n", "Busqueda exitosa");
	}else{
		printf("%s\n", "Este registro no existe");
	}
	exeMenu();
}
