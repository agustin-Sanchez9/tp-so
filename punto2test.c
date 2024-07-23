#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

// declaracin del handler para las señales SIGINT y SIGUSR1
void handler1(int s){
    exit(0);
}

int main(int argc,char *argv[]){
    
    // mensaje de error por exceso de argumentos.
    if(argc!=2){
        printf("error, solo se permite 1 argumento para el proceso.");
        exit(1);
    }

    // declaracion de variables y transformo argv[1] a int con atoi().
    pid_t pid;
    int n = atoi(argv[1]);
    int retornos[20];
    pid_t pids[20];

    // chequeo que argv[1] sea positivo.
    if(n<0){
        printf("el numero de procesos hermanos debe ser mayor a 0\n");
        exit(1);
    }

    // ciclo for de creacion de hermanos.
    for(int i=0 ; i<n && pid>0 ; i++){
        pid = fork();
        if(pid<0){
            perror("fork");
            exit(1);
        }
        if(pid==0){
            signal(SIGUSR1,handler1);
            signal(SIGINT,handler1);
            pause();
        }
        if(pid>0){
            printf("pid hijo: %i\n",pid);
            pids[i]=pid;
            printf("pid hijo: %i\n",pids[i]);
        }

    }

    // codigo de espera del padre.
    for(int i=0; i<n ; i++){
        kill(pids[i],SIGUSR1);
        retornos[i]=i+1;
        printf("valor del hijo %i: %i\n",pids[i],retornos[i]);
    }

    // codigo de la sumatoria.
    int resultado = 0;
    for(int i=0; i<n ; i++){
        resultado += retornos[i] ;
    }

    printf("resultado: %i\n",resultado);
    return 0;
}

