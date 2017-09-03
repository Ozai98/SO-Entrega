#include<stdio.h>
#include<stdlib.h>

struct List{
  struct Node* head;
  struct Node* curr;
  int size;
};

struct Node{
	long data;
	struct Node* next;
	struct Node* prev;
};
//  Inicia la estructura
void dllInit(void* p){
  struct List* list = p;
  list->size = 0;
  list->head = NULL;
  list->curr = list->head;
}
// Verifica si la estructura está vacía
int dllIsEmpty(void* p){
  struct List* list = p;
  if(list->head == NULL){
    return 1;
  }
  return 0;
}
//  Rebobina la estructura
void dllRewind(void* p){
  struct List* list = p;
  list->curr = list->head;
}
//  Verifica si el nodo actual tiene siguiente
int dllHasNext(void* p){
  struct List* list = p;
  if(dllIsEmpty(list)){
    perror("Error dllHasNext of a empty list");
    exit(-1);
  }
  if(list->curr->next != NULL)
    return 1;
  return 0;
}
//  Retorna el nodo actual y lo mueve al siguiente
long dllNext(void* p){
  struct List* list = p;
  if(dllHasNext(list)){
    list->curr = list->curr->next;
    return list->curr->prev->data;
  }
  printf("%s\n","Error at call dllNext()\n");
}
//  Obtiene el dato del nodo actual
long dllGetCurrData(void* p){
  struct List* list = p;
  if(list->curr != NULL)
    return list->curr->data;
  perror("Error getCurrData of NULL");
  exit(-1);
}
//  Asigna el dato l nodo actual
void dllSetCurrData(void* p, long data){
  struct List* list = p;
  if(list->curr != NULL)
    list->curr->data = data;
  else{
    perror("Error setCurrData of NULL");
    exit(-1);
  }
}
//  Añade como cabeza un nodo a la estructura
void dllAddHead(void* p, long data){
  struct List* list = p;
  struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
  newNode->data = data;
  newNode->prev = NULL;
  newNode->next = list->head;
  if(!dllIsEmpty(list))
    list->head->prev = newNode;
  list->head = newNode;
  list->curr = list->head;
  list->size += 1;
}
//  Retorna el valor del nodo actual y elimina el nodo
long dllDeleteCurr(void* p){
  struct List* list = p;

  if(dllIsEmpty(list)){
    perror("Error can't delete from empty list");
    exit(-1);
  }
  // Proceso para lista unitaria y borrar cabeza
  if(list->curr == list->head){
    list->head = list->head->next;
  }
  // Proceso para intermedios y final
  else{
    list->curr->prev->next = list->curr->next;
    if(list->curr->next != NULL)
      list->curr->next->prev = list->curr->prev;
  }
  long data = list->curr->data;
  free(list->curr);
  list->curr = list->head;
  list->size -= 1;
  return data;
}
//  Imprime el dato de cada uno de los nodos de la estructura
void dllPrintAll(void* p){
  struct List* list = p;
  struct Node* currNode = list->head;
  if(!dllIsEmpty(list)){
    while(currNode->next != NULL){
      printf("%ld\n", currNode->data);
      currNode = currNode->next;
    }
    printf("%ld\n", currNode->data);
  }
}
//  Libera cada uno de los nodos de la estructura y la estructura misma
void dllFree(void* p){
  struct List* list = p;
  struct Node* currNode = list->head;
  if(!dllIsEmpty(list)){
    while(currNode->next != NULL){
      currNode = currNode->next;
      free(currNode->prev);
    }
    free(currNode);
  }
  free(list);
}
