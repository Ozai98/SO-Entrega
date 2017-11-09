#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>


#define FILE_LENGTH 400


int main (int argc, char* const argv[])
{
  int fd, i;
  void* file_memory;
  FILE* f2 = fopen("archivo.dat","w+");
  for(i=100; i<200; i++) {
    fwrite(&i, sizeof(int), 1, f2);
  }
  fclose(f2);


  fd = open (argv[1], O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

  file_memory = mmap (0, FILE_LENGTH, PROT_WRITE, MAP_SHARED, fd, 0);

  for(i=0; i<100; i++) {
      sprintf((char*) file_memory + (i*4), "\n%03d", i);
  }

  munmap (file_memory, FILE_LENGTH);
  close(fd);


  FILE* f3 = fopen("archivo.dat","r");
  int a;
  for(i=100; i<200; i++) {
    fread(&a, sizeof(int), 1, f3);
    printf("%i\n", *a);
  }
  fclose(f3);

  return 0;
}
