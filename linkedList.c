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

void dllInit(void* p){
  struct List* list = p;
  list->size = 0;
  list->head = (struct Node*)malloc(sizeof(struct Node));
  list->head->data = 0;
  list->head->next = list->tail;
  list->head->prev = NULL;
  list->curr = list->head;
}
void dllInit2(void* p){
  struct List* list = p;
  list->size = 0;
  list->head = NULL;
  list->curr = list->head;
}

void dllPrintAll(void* p){
  struct List* list = p;
  printf("list size: %i\n", list->size);
  struct Node* currNode = (struct Node*)malloc(sizeof(struct Node));
  currNode = list->head;
  while(currNode->next != NULL){
    currNode = currNode->next;
    printf("%ld\n", currNode->data);
  }
}
void dllPrintAll2(void* p){
  struct List* list = p;
  printf("list size: %i\n", list->size);
  struct Node* currNode = (struct Node*)malloc(sizeof(struct Node));
  currNode = list->head;
  while(currNode->next != NULL){
    printf("%ld\n", currNode->data);
    currNode = currNode->next;
  }
}

long dllGetCurrData(void* p){
  struct List* list = p;
  return list->curr->data;
}
long dllGetCurrData2(void* p){
  struct List* list = p;
  if(list->curr != NULL)
    return list->curr->data;
  perror("Error getCurrData of NULL");
  exit(-1);
}

void dllAddHead(void* p, long data){
  struct List* list = p;
  struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
  newNode->data = data;
  newNode->prev = list->head;
  newNode->next = list->head->next;
  list->tail->prev->next = newNode;
  list->tail->prev = newNode;
  list->size += 1;
}
void dllAddHead2(void* p, long data){
  struct List* list = p;
  struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
  newNode->data = data;
  newNode->prev = NULL;
  newNode->next = list->head;
  list->head->prev = newNode;
  list->head = newNode;
  list->size += 1;
}

int dllIsEmpty(void* p){
  struct List* list = p;
  return list->head == NULL;
}

void dllRewind(void* p){
  struct List* list = p;
  list->curr = list->head;
}

int dllHasNext(void* p){
  struct List* list = p;
  if(list->curr->next != list->tail)
    return 1;
  return 0;
}
int dllHasNext2(void* p){
  struct List* list = p;
  if(dllIsEmpty(list)){
    perror("Error dllHasNext of a empty list");
    exit(-1);
  }
  if(list->curr->next != NULL)
    return 1;
  return 0;
}

long dllNext(void* p){
  struct List* list = p;
  if(list->curr->next != list->tail){
    list->curr = list->curr->next;
    return list->curr->data;
  }
  perror("Error at call dllNext()");
  exit(-1);
}
long dllNext2(void* p){
  struct List* list = p;
  if(dllHasNext2(list)){
    list->curr = list->curr->next;
    return list->curr->prev->data;
  }
  perror("Error at call dllNext()");
  exit(-1);
}

void dllSetDataCurr(void* p, long data){
  struct List* list = p;
  if(list->curr == list->head){
    perror("Error can't delete head");
    exit(-1);
  }
  list->curr->data = data;
  // list->curr->next->prev->data = data;
  // TODO: Corregir si falla borrar registro
}
void dllSetDataCurr2(void* p, long data){
  struct List* list = p;
  if(list->curr == list->head){
    perror("Error can't delete head");
    exit(-1);
  }
  list->curr->data = data;
  // list->curr->next->prev->data = data;
  // TODO: Corregir si falla borrar registro
}

long dllDeleteCurr(void* p){
  struct List* list = p;
  if(list->curr == list->head){
    perror("Error can't delete head");
    exit(-1);
  }
  list->curr->prev->next = list->curr->next;
  free(list->curr->next->prev);
  list->curr->next->prev = list->curr->prev;
  list->curr = list->curr->prev;
  list->size -= 1;
}


void dllFree(void* p){
  struct List* list = p;
  struct Node* currNode = (struct Node*)malloc(sizeof(struct Node));
  currNode = list->head->next;
  while(currNode != list->tail){
    free(currNode->prev);
    currNode = currNode->next;
  }
  free(list->tail);
  free(list->curr);
  free(currNode);
}
