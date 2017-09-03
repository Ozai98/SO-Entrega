#include <unistd.h>
#include "hashTable.c"
#include <ctype.h>



//Declaración de las funciones
void menu(struct List** hashTable);
void addReg(struct List** hashTable);
void seeReg(struct List** hashTable);
void deleteReg(struct List** hashTable);
void searchReg(struct List** hashTable);

int main(){
	struct List** hashTable = (struct List**)malloc(sizeof(struct List*)*HASH_TABLE_SIZE);
	// struct List* hashTable[HASH_TABLE_SIZE]
	htInit(hashTable);
	htLoad(hashTable);
	menu(hashTable);
}


// Menú que despliega las opciones
void menu(struct List** hashTable){
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
			htLoad(hashTable);
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
// Función que añade un registro a al archivo
void addReg(struct List** hashTable){
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

	fseek(fptr, 0, SEEK_END);
  long data = ftell(fptr);
	htAdd(hashTable, newDog->name, data);

	fwrite(newDog, sizeof(struct dogType), 1, fptr);
	free(newDog);
	checkfclose(fptr, DATA_DOGS_PATH);

	printf("Registro añadido exitosamente. Presione enter para continuar");
	char newLine;
	scanf("%c", &newLine);
		menu(hashTable);
}

//	Función que permite ver un registro de dataDogs.dat
void seeReg(struct List** hashTable){
	int numberReg;
	int value = 0;
	long totalSize;
	char ans;
	FILE* fptr = checkfopen(DATA_DOGS_PATH, "r");
	fseek(fptr, 0, SEEK_END);
	totalSize = ftell(fptr) / (sizeof(struct dogType));
	printf("El número de registros es de: %ld\n",totalSize);
	do{
		printf("%s\n", "Ingrese el número del registro a consultar");
		scanf("%d", &numberReg);
		numberReg-= 1;
		if(numberReg < 0 || numberReg >= totalSize){
			printf("%s\n", "Este registro no existe");
		}else{
			value = 1;
			struct dogType* newDog = (struct dogType*)malloc(sizeof(struct dogType));
			fseek(fptr, numberReg*sizeof(struct dogType), SEEK_SET);
			fread(newDog, sizeof(struct dogType), 1, fptr);
			showDogType(newDog);
      free(newDog);
      checkfclose(fptr, DATA_DOGS_PATH);
			printf("Consulta de registro exitosa\n");

			printf("%s\n", "Desea abrir la historia clínica del registro seleccionado Escriba S o N");
			scanf(" %c", &ans);
			if(ans == 's' || ans == 'S'){
				char file_name_2[12];

				int potato = numberReg+1;
				sprintf(file_name_2, "gedit %d.txt", potato);
				system(file_name_2);
				char newLine;
				scanf("%c", &newLine);
					menu(hashTable);

			}else{
				char newLine;
				scanf("%c", &newLine);
					menu(hashTable);
			}

		}
	}while (value == 0);
}

//	Función que elimina un registro de datadogs.dat
void deleteReg(struct List** hashTable){
	int i = 0;
	long filePointer = 0;
	int code = 0;
	int delReg = 0;
	FILE* dataDogs = checkfopen(DATA_DOGS_PATH, "r");
	FILE* tempDataDogs = checkfopen(TEMP_DATA_DOGS_PATH, "w");
	struct dogType* currDog = (struct dogType*)malloc(sizeof(struct dogType));
	printf("%s\n", "Ingrese el id del registro a eliminar");
	scanf("%i", &delReg);
	delReg-=1;
	fseek(dataDogs, 0, SEEK_END);
	long numberOfStructures = ftell(dataDogs)/sizeof(struct dogType);
	printf("number of structures: %ld\n", numberOfStructures);
	rewind(dataDogs);
	for(i; i<numberOfStructures; i++){
		filePointer = ftell(dataDogs);
		fread(currDog, sizeof(struct dogType), 1, dataDogs);
		if(filePointer == delReg*sizeof(struct dogType)){
			fseek(dataDogs, sizeof(struct dogType), SEEK_CUR);
			code = htHashFunction(currDog->name);
			dllRewind(hashTable[code]);
			while(dllHasNext(hashTable[code])){
				if(dllGetCurrData(hashTable[code]) == filePointer){
					dllDeleteCurr(hashTable[code]);
					break;
				}
			}
			continue;
		}
		fwrite(currDog, sizeof(struct dogType), 1, tempDataDogs);
	}
	free(currDog);
	checkfclose(dataDogs, DATA_DOGS_PATH);
	checkfclose(tempDataDogs, TEMP_DATA_DOGS_PATH);
	remove(DATA_DOGS_PATH);
	rename(TEMP_DATA_DOGS_PATH, DATA_DOGS_PATH);
}

//	Función que busca en dataDogs.dat las mascotas con el mismo nombre
void searchReg(struct List** hashTable){
	char petName[NAME_SIZE];
  printf("%s\n", "Ingrese el nombre de la mascota, este no debe superar los 32 caracteres, si lo hace solo se guardaran los primeros 32");
	fgets(petName, 32, stdin);
	// scanf("%32s[^\n]", petName);
  printf("%s\n", petName);
	dllPrintAll(hashTable[htHashFunction(petName)]);
  htSearch(hashTable,petName);
	printf("%s", "Busqueda exitosa, presione enter para continuar");
	char newLine;
	fflush(stdin);
	scanf(" %c", &newLine);
		menu(hashTable);
}
