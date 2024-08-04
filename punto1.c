#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void){

    // variables
    pid_t pid;
    char comando[64];
    char *token;
    char *args[11]; // 10 argumentos + 1 por el NULL al final
    int argsCount = 0;


    // introduccion del comando
    printf("$>> ");
    memset(comando,'\0',64);
    scanf("%s",comando);


    // parseo
    while((token = strtok(comando," ")) != NULL ){
        args[argsCount] = token;
        argsCount++;
    }

    while(strcmp(comando,"salir")){

        pid = fork();
        // hijo
        if(pid == 0){
            execvp(args[0],args);
            printf("comando no valido \n");
            exit(0);
        }
        // padre
        else if(pid > 0){
            waitpid(pid,0,0);
            printf("$>> ");
            memset(comando,'\0',64);
            scanf("%s",comando);
            while((token = strtok(comando," ")) != NULL ){
                args[argsCount] = token;
                argsCount++;
            }

        }
        // error
        else{
            perror("ERROR FORK \n");
        }
    }

    return 0;
    exit(0);
}
