#include "linkedList.c"

int main(){

  struct List* list = (struct List*)malloc(sizeof(struct List)*3000);
  // int*3000
  dllInit(list);
  long a = 111;
  dllAddBack(list,a);
  dllPrintAll(list);
  a = 222;
  dllAddBack(list,a);
  dllPrintAll(list);
  a = 333;
  dllAddBack(list,a);
  dllPrintAll(list);
  a = 444;
  dllAddBack(list,a);
  dllPrintAll(list);

  printf("%i\n", dllHasNext(list));
  while(dllHasNext(list)){
    printf("%ld\n", dllNext(list));
    // printf("%ld\n", dllCurrData(list));
    // if(dllHasNext(list))
    //   printf("%ld\n", dllNext(list));
    // else
    //   printf("%s\n", "ni idea");
  }
  dllRewind(list);
  dllNext(list);
  dllDeleteCurr(list);
  dllPrintAll(list);

  printf("------------------------------");
  a = 555;
  dllAddBack(list,a);
  dllPrintAll(list);


  dllRewind(list);
  dllNext(list);
  dllNext(list);
  dllNext(list);
  dllPrintAll(list);
  printf("%lu\n", dllCurrData(list));
  dllSetDataCurr(list, 999);
  dllPrintAll(list);
  printf("%lu\n", dllCurrData(list));

  dllFree(list);

}
