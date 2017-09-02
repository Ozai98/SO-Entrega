#include "linkedList.c"

int main(){
  struct List* list = (struct List*)malloc(sizeof(struct List));
  dllPrintAll(list);
  dllInit(list);
  printf("%i %i\n", (int)list->head, (int)list->curr);
  dllPrintAll(list);
  long a = 111;
  dllAddHead(list,a);
  printf("%i %i\n", (int)list->head, (int)list->curr);
  dllPrintAll(list);
  a = 222;
  dllAddHead(list,a);
  printf("%i %i\n", (int)list->head, (int)list->curr);
  dllPrintAll(list);
  a = 333;
  dllAddHead(list,a);
  printf("%i %i\n", (int)list->head, (int)list->curr);
  dllPrintAll(list);
  a = 444;
  dllAddHead(list,a);
  printf("%i %i\n", (int)list->head, (int)list->curr);
  dllPrintAll(list);

  printf("%ld\n", dllGetCurrData(list));
  printf("%ld\n", dllNext(list));
  printf("%ld\n", dllNext(list));
  printf("%ld\n", dllNext(list));
  dllNext(list);
  dllNext(list);
  dllNext(list);
  printf("%ld\n", dllGetCurrData(list));
  dllRewind(list);
  printf("%ld\n", dllGetCurrData(list));
  dllNext(list);
  printf("%ld\ndelete mid\n", dllGetCurrData(list));
  dllDeleteCurr(list);
  printf("%ld\n", dllGetCurrData(list));
  dllPrintAll(list);
  dllRewind(list);
  printf("%ld\ndelete head\n", dllGetCurrData(list));
  dllDeleteCurr(list);
  printf("%ld\n", dllGetCurrData(list));
  dllPrintAll(list);
  dllNext(list);
  printf("%ld\ndelete tail\n", dllGetCurrData(list));
  dllDeleteCurr(list);
  printf("%ld\n", dllGetCurrData(list));
  dllPrintAll(list);
  dllSetCurrData(list,1234);
  printf("%ld\n", dllGetCurrData(list));
  dllPrintAll(list);

  dllPrintAll(list);
  dllAddHead(list,15);
  dllAddHead(list,25);
  dllAddHead(list,50);
  dllPrintAll(list);
  dllFree(list);
  printf("%i\n", list->size);
  printf("%i\n", list->head->data);
  // dllPrintAll(list);




  //
  // printf("------------------------------");
  // a = 555;
  // dllAddBack(list,a);
  // dllPrintAll(list);
  //
  //
  // dllRewind(list);
  // dllNext(list);
  // dllNext(list);
  // dllNext(list);
  // dllPrintAll(list);
  // printf("%lu\n", dllCurrData(list));
  // dllSetDataCurr(list, 999);
  // dllPrintAll(list);
  // printf("%lu\n", dllCurrData(list));
  //
  // dllFree(list);
}
