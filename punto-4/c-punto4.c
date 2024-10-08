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

    // obtencion de los semaforos creados
    int n = atoi(argv[1]);

    if(n<0){
        printf("el numero de iteraciones debe ser mayor a 0\n");
        exit(1);
    }

    int semid = semget(0xa,0,0);

    int fd = open("punto4-fd", O_WRONLY, 0777);
    char letra = 'C';

    for(int i=0 ; i<n ; i++){
        // bloquea semaforos c y x
        P(semid,2);
        P(semid,3);
        // seccion critica
        lseek(fd,0,2);
        write(fd, &letra, 1);
        // desbloquea semaforos a y b
        V(semid,1);
        V(semid,0);
    }
    exit(0);
}