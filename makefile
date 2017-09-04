# Argumentos Gcc

# These options specify directories to search for header files, for 	libraries and for parts of the compiler:
# -I dir

# -c
# Compile or assemble the source files, but do not link. The linking stage simply is not done. The ultimate output is in the form of an object file for each source file.

# -o
# <

# Plano
#p1-dogProgram: structGenerator.c p1-dogProgram.c
	#gcc -o p1-dogProgram structGenerator.c p1-dogProgram.c -I.

# Usando constantes
# Creamos "archivos objeto" .o

CC=gcc
CFLAGS=-I.

p1-dogProgram: structGenerator.o p1-dogProgram.o
	$(CC) -o p1-dogProgram structGenerator.o p1-dogProgram.o -I.

# Agregando dependencias al .h

#CC=gcc
#CFLAGS=-I.
#DEPS = hellomake.h

#%.o: %.c $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS)

#hellomake: hellomake.o hellofunc.o
#	gcc -o hellomake hellomake.o hellofunc.o -I.

# %.ext incluye todos los archivos que terminen en esa extención.
# $(CC) - usar el gcc
# $@ el output tendrá el output de
