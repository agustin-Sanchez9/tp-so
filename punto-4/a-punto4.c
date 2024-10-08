#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "pvsem.h"

int main(int argc, char *argv[]){

    // mensaje de error por exceso de argumentos.
    if(argc!=2){
        printf("error, solo se permite 1 argumento para el proceso.\n");
        exit(1);
    }

    int n = atoi(argv[1]);
    n = n*2; // multiplico por 2 dado que A esta 2 veces por iteracion.

    if(n<0){
        printf("el numero de iteraciones debe ser mayor a 0\n");
        exit(1);
    }

    // obtencion de los semaforos creados
    int semid = semget(0xa,0,0);

    int fd = open("punto4-fd",  O_WRONLY, 0777);
    char letra = 'A';

    for(int i=0 ; i<n ; i++){
        // bloquea samforo a
        P(semid,0);
        // seccion critica
        lseek(fd,0,2);
        write(fd, &letra, 1);
        // libera semaforo x
        V(semid,3);
    }
    exit(0);
}