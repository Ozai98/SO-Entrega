#include <unistd.h>
#include "hashTable.c"
#include <ctype.h>

//Declaración de las funciones
void menu(char** hashTable);
void exeMenu(char** hashTable);
void addReg(char** hashTable);
void seeReg(char** hashTable);
void deleteReg(char** hashTable);
void searchReg(char** hashTable);

int main(){
	// char* hashTable[]={"12341234.dat", "12341234.dat"};
	// char* hashTable[]={"pepe.dat", "rufus.dat"};
	// name	---->		code	---->		"hashTable10242350235.dat"
	// char hashTable[HASH_TABLE_SIZE][15];
	char** hashTable = (char**)malloc(HASH_TABLE_SIZE*20);
	htInit(hashTable);
	htLoad(hashTable);
	// int i=0;
	// for(i=0; i<HASH_TABLE_SIZE; i++){
	// 	printf("%s\n", hashTable[i]);
	// }
	// menu(hashTable);
	free(hashTable);
	return 0;
}
// Menú que despliega las opciones
void menu(char** hashTable){
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
		if (strlen(input) != 1 || num < 1 || num > 5){
			printf("%s\n", "El valor ingresado no es valido, intente de nuevo");
		}
	}while(strlen(input) != 1 || num < 1 || num > 5);
	switch(num){
		case 1:
			printf("Ingresar registro\n");
			addReg(hashTable);
			break;
		case 2:
			printf("Ver registro\n");
			seeReg(hashTable);
			break;
		case 3:
			printf("Borrar registro\n");
			deleteReg(hashTable);
			// htLoad(hashTable);
			break;
		case 4:
			printf("Buscar registro\n");
			searchReg(hashTable);
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
void exeMenu(char** hashTable){
	getchar();
	menu(hashTable);
}
// Función que añade un registro a al archivo
void addReg(char** hashTable){
// 	char term;
// 	int numberRight = 0;
// 	char petName[NAME_SIZE];
// 	char type[TYPE_SIZE];
// 	char  breed[BREED_SIZE];
// 	char sex;
// 	int age;
// 	char* strAge;
// 	int height;
// 	float weight;
// 	int rightValue = 0;
//
// 	//Captura de los datos de la mascota
// 	printf("%s", "Ingrese el nombre de la mascota, este no debe superar los 32 caracteres,\nsi lo hace solo se guardaran los primeros 32: ");
// 	getchar();
// 	fgets(petName, NAME_SIZE, stdin);
// 	petName[strlen(petName)-1]=0;
// 	printf("Nombre ingresado: %s\n", petName);
//
// 	printf("%s", "Ingrese el tipo de la mascota, este no debe superar los 32 caracteres \nsi lo hace solo se guardaran los primeros 32: ");
// 	fgets(type, NAME_SIZE, stdin);
// 	type[strlen(type)-1]=0;
// 	printf("Tipo ingresado: %s\n", type);
//
// 	printf("%s","Ingrese la raza de la mascota, este no debe superar los 32 caracteres, \nsi lo hace solo se guardaran los primeros 32: ");
// 	fgets(breed, NAME_SIZE, stdin);
// 	breed[strlen(breed)-1]=0;
// 	printf("Raza ingresada: %s\n", breed);
// 	printf("%s", "Ingrese el sexo 'H' para Hembra o 'M' para Macho: ");
// 	fflush(stdin);
// 	scanf(" %c", &sex);
// 	while(sex != 'H' && sex != 'M' && sex != 'm' && sex != 'h'){
// 		printf("%s\n", "Ingrese el sexo 'H' o 'M'");
// 		scanf(" %c", &sex);
// 	}
// 	printf("Sexo ingresado: %c\n", sex);
//
// 	printf("Ingrese la edad de la mascota: ");
// 	do{
// 		getchar();
// 		if(scanf("%d%c", &age, &term) != 2 || term != '\n')
// 		    printf("Valor no valido, intente de nuevo: ");
// 		else
// 			rightValue = 1;
// 	}while(rightValue == 0);
// 	printf("La edad introducida es: %d\n", age);
//
// 	printf("Ingrese la altura de la mascota (En cm): ");
// 	rightValue = 0;
// 	do{
// 		if(scanf(" %d%c", &height, &term) != 2 || term != '\n')
// 				printf("Valor no valido, intente de nuevo. ");
// 		else
// 			rightValue = 1;
// 	}while(rightValue == 0);
// 	printf("La altura introducida es: %d\n", height);
//
// 	printf("Ingrese el peso de la mascota (En kg): ");
// 	rightValue = 0;
// 	do{
// 		if(scanf(" %f%c", &weight, &term) != 2 || term != '\n')
// 		    printf("Valor no valido, intente de nuevo. ");
// 		else
// 			rightValue = 1;
// 	}while(rightValue == 0);
// 	printf("El peso introducido es: %f\n", weight);
//
// 	//Creación de la estructura
// 	struct dogType* newDog = malloc(sizeof(struct dogType));
// 	FILE* fptr = checkfopen(DATA_DOGS_PATH, "r+");
// 	FILE* fptc = checkfopen(CURRENT_ID_PATH, "r+");
// 	unsigned int id;
// 	fread(&id, sizeof(int), 1, fptc);
// 	id += 1;
// 	newDog->id = id;
// 	rewind(fptc);
// 	fwrite(&id, sizeof(int), 1, fptc);
// 	checkfclose(fptc, CURRENT_ID_PATH);
// 	strcpy(newDog->name, petName);
// 	strcpy(newDog->type, type);
// 	strcpy(newDog->breed, breed);
// 	newDog->age = age;
// 	newDog->height = height;
// 	newDog->weight = weight;
// 	newDog->gender = sex;
// 	//Muestra la estructura creada
// 	showDogType(newDog);
//   //Añade la estructura a la hashTable y a dataDogs.dat
// 	fseek(fptr, 0, SEEK_END);
//   int data = ftell(fptr);
// 	htAdd(hashTable, newDog->name, data);
// 	fwrite(newDog, sizeof(struct dogType), 1, fptr);
// 	//Libera el espacio en memoria de la estructura cierra el archivo dataDogs.dat y reejecuta el menú
// 	free(newDog);
// 	checkfclose(fptr, DATA_DOGS_PATH);
// 	printf("Registro añadido exitosamente. Presione enter para continuar");
// 	exeMenu(hashTable);
}
//	Función que permite ver un registro de dataDogs.dat
void seeReg(char** hashTable){
// 	int numberReg;
// 	int value = 0;hTFileName
// 	int totalSize;
// 	char ans;
// 	FILE* fptr = checkfopen(DATA_DOGS_PATH, "r");
// 	//Calcula el numero de registros
// 	fseek(fptr, 0, SEEK_END);
// 	totalSize = ftell(fptr) / (sizeof(struct dogType));
// 	printf("El número de registros es de: %i\n",totalSize);
// 	do{
// 		printf("%s\n", "Ingrese el número del registro a consultar");
// 		scanf("%d", &numberReg);
// 		numberReg-= 1;
// 		//Si el numero ingresado se sale del rango de registros el registro solicitado no existe
// 		if(numberReg < 0 || numberReg >= totalSize)
// 			printf("%s\n", "Este registro no existe");
// 		else{
// 			//Imprime el registro solicitado
// 			value = 1;
// 			struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
// 			fseek(fptr, numberReg*sizeof(struct dogType), SEEK_SET);
// 			fread(newDog, sizeof(struct dogType), 1, fptr);
// 			showDogType(newDog);
//       free(newDog);
//       checkfclose(fptr, DATA_DOGS_PATH);
// 			printf("Consulta de registro exitosa\n");
//
// 			printf("%s\n", "Desea abrir la historia clínica del registro seleccionado Escriba S o N");
// 			scanf(" %c", &ans);
// 			//Si la respuesta es afirmativa, abre la historia clinima
// 			if(ans == 's' || ans == 'S'){
// 				char file_name_2[12];
// 				int potato = numberReg+1;
//
// 				sprintf(file_name_2, "gedit %d.txt", potato);
// 				system(file_name_2);
// 				printf("%s\n", "Presione enter para continuar");
// 					getchar();
// 					exeMenu(hashTable);
// 			//Si no, reejecuta el menu
// 			}else{
// 				printf("%s\n", "Presione enter para continuar");
// 				getchar();
// 				exeMenu(hashTable);
// 			}
// 		}
// 	}while (value == 0);
}
// Función que elimina un registro de datadogs.dat
void deleteReg(char** hashTable){
// 	int i = 0;
// 	int filePointer = 0;
// 	int code = 0;
// 	int delReg = 0;
// 	int success = 0;
//
// 	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
// 	FILE* tempDataDogs = checkfopen(TEMP_DATA_DOGS_PATH, "w");
// 	struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType));
//
// 	printf("%s\n", "Ingrese la posición del registro a eliminar");
// 	scanf("%i", &delReg);
// 	delReg-=1;
// 	fseek(dataDogs, 0, SEEK_END);
// 	int numberOfStructures = ftell(dataDogs)/sizeof(struct dogType);
// 	rewind(dataDogs);
// 	if(delReg < 1 || delReg > numberOfStructures){
// 		printf("%s\n", "El registro que desea eliminar no existe, presione enter para continuar");
// 		checkfclose(dataDogs, DATA_DOGS_PATH);
// 		checkfclose(tempDataDogs, TEMP_DATA_DOGS_PATH);
// 	}else{
// 		for(i; i<numberOfStructures; i++){
// 			//Copia todos los registros en el archivo nuevo
// 			filePointer = ftell(dataDogs);
// 			fread(currDog, sizeof(struct dogType), 1, dataDogs);
// 			if(filePointer == delReg*sizeof(struct dogType)){
// 				//Si encuentra el archivo a eliminar, lo elimina de la hash y salta la copia de este registro en el nuevo archivo
// 				code = htHashFunction(currDog->name);
// 				dllRewind(hashTable[code]);
//
// 				while(dllHasNext(hashTable[code])){
// 					if(dllGetCurrData(hashTable[code]) == filePointer){
// 						dllDeleteCurr(hashTable[code]);
// 						break;
// 					}
// 					dllNext(hashTable[code]);
// 				}
// 				if(dllGetCurrData(hashTable[code]) == filePointer){
// 					dllDeleteCurr(hashTable[code]);
// 				}
// 				success = 1;
// 				continue;
// 			}
// 			fwrite(currDog, sizeof(struct dogType), 1, tempDataDogs);
// 		}
// 		printf("%s\n", "Eliminación exitosa... espere por favor");
// 		htFree(hashTable);
// 		struct List** hashTable = (struct List**)malloc(sizeof(struct List*)*HASH_TABLE_SIZE);
// 		free(currDog);
// 		checkfclose(dataDogs, DATA_DOGS_PATH);
// 		checkfclose(tempDataDogs, TEMP_DATA_DOGS_PATH);
// 		remove(DATA_DOGS_PATH);
// 		rename(TEMP_DATA_DOGS_PATH, DATA_DOGS_PATH);
// 		htInit(hashTable);
// 		htLoad(hashTable);
// 		printf("%s\n", "Presione enter para continuar");
// 	}
// 	getchar();
// 	exeMenu(hashTable);
}
//	Función que busca en dataDogs.dat las mascotas con el mismo nombre
void searchReg(char** hashTable){
// 	char petName[NAME_SIZE];
//   printf("Ingrese el nombre de la mascota, este no debe superar los 32 caracteres, si lo hace solo se guardaran los primeros 32: ");
// 	getchar();
// 	fgets(petName, NAME_SIZE, stdin);
// 	petName[strlen(petName)-1]=0;
// 	int code = htHashFunction(petName);
// 	if(htSearch(hashTable,petName)){
// 		printf("%s", "Busqueda exitosa, presione enter para continuar");
// 	}else{
// 		printf("%s", "Este registro no existe, presione enter para continuar");
// 	}
// 	exeMenu(hashTable);
}
