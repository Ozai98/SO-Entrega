#include <unistd.h>
#include "atributes.c"
#include "hashTable.c"
#include <ctype.h>

//Declaración de las funciones
void menu();
void addReg();
void seeReg();
void deleteReg();
void searchReg();

int main(){
	struct List* hashTable[HASH_TABLE_SIZE];
	//initHashTable(hashTable);
	//loadHashTable(hashTable);
	menu();
//  freeHashTable(hashTable);
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
		if (strlen(input) != 1 || num < 1 || num > 5){
			printf("%s\n", "El valor ingresado no es valido, intente de nuevo");
		}
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
// Función que añade un registro a al archivo
void addReg(){
	char term;
	int numberRight = 0;
	char petName[NAME_SIZE];
	char type[TYPE_SIZE];
	char  breed[BREED_SIZE];
	char sex;
	int age;
	char* strAge;
	int height;
	float weight;
	int rightValue = 0;

	//Captura de los datos de la mascota

	//check else in get string cycles
	printf("%s\n", "Ingrese el nombre de la mascota, este no debe superar los 32 caracteres,\nsi lo hace solo se guardaran los primeros 32");
	scanf("%32s", petName);
	printf("Nombre ingresado: %s\n", petName);

	printf("%s\n", "Ingrese el tipo de la mascota, este no debe superar los 32 caracteres \nsi lo hace solo se guardaran los primeros 32");
	scanf("%32s", type);
	printf("Tipo ingresado: %s\n", type);

	printf("%s\n","Ingrese la raza de la mascota, este no debe superar los 32 caracteres, \nsi lo hace solo se guardaran los primeros 32" );
	scanf("%32s", breed);
	printf("Raza ingresada: %s\n", breed);

	printf("%s\n", "Ingrese el sexo 'H' para Hembra o 'M' para Macho");
	scanf(" %c", &sex);
	while(sex != 'H' && sex != 'M' && sex != 'm' && sex != 'h'){
		printf("%s\n", "Ingrese el sexo 'H' o 'M'");
		scanf(" %c", &sex);
	}
	printf("Sexo ingresado: %c\n", sex);

	printf("Ingrese la edad de la mascota");
	do{
		getchar();
		if(scanf("%d%c", &age, &term) != 2 || term != '\n')
		    printf("Valor no valido, intente de nuevo: ");
		else
			rightValue = 1;
	}while(rightValue == 0);
	printf("La edad introducida es: %d\n", age);

	printf("Ingrese la altura de la mascota");
	rightValue = 0;
	do{
		if(scanf(" %d%c", &height, &term) != 2 || term != '\n')
				printf("Valor no valido, intente de nuevo: ");
		else
			rightValue = 1;
	}while(rightValue == 0);
	printf("La altura introducida es: %d\n", height);

	printf("Ingrese el peso de la mascota");
	rightValue = 0;
	do{
		if(scanf(" %f%c", &weight, &term) != 2 || term != '\n')
		    printf("Valor no valido, intente de nuevo: ");
		else
			rightValue = 1;
	}while(rightValue == 0);
	printf("El peso introducido es: %f\n", weight);

	//Creación de la estructura

	struct dogType* newDog = malloc(sizeof(struct dogType));
	FILE* fptr = checkfopen(DATA_DOGS_PATH, "r+");
	FILE* fptc = checkfopen(CURRENT_ID_PATH, "r+");
	unsigned long id;
	fread(&id, sizeof(long), 1, fptc);
	printf("%lu\n", id);
	id += 1;
	newDog->id = id;
	rewind(fptc);
	fwrite(&id, sizeof(long), 1, fptc);
	checkfclose(fptc, CURRENT_ID_PATH);
	strcpy(newDog->name, petName);
	strcpy(newDog->type, type);
	strcpy(newDog->breed, breed);
	newDog->age = age;
	newDog->height = height;
	newDog->weight = weight;
	newDog->gender = sex;
	showDogType(newDog);
	free(newDog);

	// fseek(fptr, 0, SEEK_END);
  // long data = ftell(fptr);
  // int code = hashFunction(newDog->name);
  // printf("%s%i\n", "hashTable[code]->head->next: ", (int)hashTable[code]->head->next);
  // // if(hashTable[code]->head->next == NULL){
  //   // dllInit(hashTable[code]);
  // // }
  // dllAddBack(hashTable[code], data);
	// int num = fwrite(newDog, sizeof(struct dogType), 1, fptr);
	// checkfclose(fptr, DATA_DOGS_PATH);
	// printf("%d",num);
	// printf("Registro añadido exitosamente. Presione enter para continuar");
	// char newLine;
	// scanf("%c", &newLine);
	// // if (newLine == '\n')
	// 	menu(hashTable);
}

//	Función que permite ver un registro de dataDogs.dat
void seeReg(){
	// int numberReg;
	// int value = 0;
	// long totalSize;
	// char ans;
	// FILE* fptr = checkfopen(DATA_DOGS_PATH, "r");
	// fseek(fptr, 0, SEEK_END);
	// totalSize = ftell(fptr) / (sizeof(struct dogType));
	// printf("El número de registros es de: %ld\n",totalSize);
	// do{
	// 	printf("%s\n", "Ingrese el número del registro a consultar");
	// 	scanf("%d", &numberReg);
	// 	numberReg-= 1;
	// 	if(numberReg < 0 || numberReg >= totalSize){
	// 		printf("%s\n", "Este registro no existe");
	// 	}else{
	// 		value = 1;
	// 		struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
	// 		fseek(fptr, numberReg*sizeof(struct dogType), SEEK_SET);
	// 		fread(newDog, sizeof(struct dogType), 1, fptr);
	// 		showDogType(newDog);
  //     free(newDog);
  //     checkfclose(fptr, DATA_DOGS_PATH);
	// 		printf("Consulta de registro exitosa\n");
	// 		char* file_name_1 = "Historia clínica de ";
	// 		char file_name_2 [10];
	// 		sprintf(file_name_2, "%d", numberReg);
	// 		int length = strlen(file_name_1) + strlen(file_name_2) + 1;
	// 		char* new_arr = malloc(length);
	// 		strcpy(new_arr, file_name_1);
	// 		strcat(new_arr, file_name_2);
	//
	// 		printf("%s\n", "Desea abrir la historia clínica del registro seleccionado Escriba S o N");
	// 		scanf(" %c", &ans);
	// 		if(ans == 's' || ans == 'S'){
	// 			if(!fork()){
  //  				execlp("gedit", "gedit", new_arr, NULL);
	// 				printf("%s\n", "Creacion/consulta de historia clinica exitosa");
	// 				char newLine;
	// 				scanf("%c", &newLine);
	// 				if (newLine == '\n'){
	// 					menu(hashTable);
  //         }
	// 			}
	// 		}else{
	// 			menu(hashTable);
	// 		}
	// 	}
	// }while (value == 0);
}

//	Función que elimina un registro de datadogs.dat
void deleteReg(){

}

//	Función que busca en dataDogs.dat las mascotas con el mismo nombre
void searchReg(){
	// char petName[NAME_SIZE];
  // printf("%s\n", "Ingrese el nombre de la mascota, este no debe superar los 32 caracteres, si lo hace solo se guardaran los primeros 32");
  // scanf("%32s", petName);
  // printf("%s\n", petName);
  // int a=0;
  // printf("%s", "numero: ");
  // scanf("%i\n", &a);
  // printf("%i\n", a);
  // FILE* dataDogs = checkfopen("archivo.txt", "w");
  // struct dogType* myDog = (struct dogType*)malloc(sizeof(struct dogType));
  // struct List* myList = (struct List*)malloc(sizeof(struct List));
  // myList = hashTable[hashFunction("Stitch")];
  // dllRewind(myList);
  // long pos=0;
  // while(dllHasNext(myList)){
  //   pos=dllNext(myList);
  //   fseek(dataDogs, pos, SEEK_SET);
  //   fread(myDog, sizeof(struct dogType), 1, dataDogs);
  //   if(strcmp(myDog->name, "Stitch")==0){
  //     showDogType(myDog);
  //     break;
  //   }
  // };
  // free(myList);
  // free(myDog);
  // checkfclose(dataDogs, "archivo.txt");
}
