#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>


int status;

// funcion calcular numero pseudo-aleatorio
/*
int calc(void){
    int num;
    srand(getpid());
    num = (rand() % 256);
    return num;
}
*/


// declaracion del handler para las señales SIGINT y SIGUSR1
void handler1(int s){
    // exit(calc());
    exit(status);
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
        if(pid>0){
            pids[i]=pid;
        }
        else if(pid==0){
            status=i+1;
            printf("hijo1 pid: %d \n",getpid());
            signal(SIGUSR1,handler1);
            signal(SIGINT,handler1);
            pause();
        }
        else{
            perror("fork");
            exit(1);
        }
    }

    // codigo de espera del padre.
    for(int i=0; i<n ; i++){
        wait(&status);
        retornos[i] = WEXITSTATUS(status);
        printf("valor del hijo %i: %i\n",pids[i],WEXITSTATUS(status));
    }

    // codigo de la sumatoria.
    int resultado = 0;
    for(int i=0; i<n ; i++){
        resultado += retornos[i] ;
    }

    printf("resultado: %i\n",resultado);
    return 0;
}

