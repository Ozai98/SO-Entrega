#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <math.h>

int main(){
  int* algo[5];
  // mlock(algo, pow(2,31));
  double i;
  int j;
  for(j=0; j<5; j++){
    algo[j] = (int*)malloc(pow(2,31));
    printf("%li\n", algo[j]);
    for(i=0; i<pow(2,31); i++){
      if((int)i%100000000==0)printf("%lf\n", i);
      algo[j][(int)i] = i;
    }
  }

  printf("Sleep\n");
  printf("Sleep\n");
}
