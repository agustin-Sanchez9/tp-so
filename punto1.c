#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

// Manejador para la señal SIGCHLD
void sigchld_handler(int s) {
    // Esperar a todos los hijos que hayan terminado
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void sigint_handler(int s){
    // while(wait(-1,) > 0);
}

int main(void){

    // Registrar el manejador de SIGCHLD
    signal(SIGCHLD, sigchld_handler);
    // signal(SIGINT,sigint_handler);

    // variables
    pid_t pid;
    char comando[1000];
    char *token;
    char *args[11]; // 10 argumentos + 1 por el NULL al final
    int argsCount = 0;
    int background = 0; // bandera para ejecución en background

    while(1){
        // introduccion del comando
        printf("$>> ");
        memset(comando,'\0',1000); // seteo a comando con todo \0
        fgets(comando, 1000, stdin); // leo lo ingresado por teclado
        comando[strcspn(comando, "\n")] = '\0'; // eliminar el salto de línea

        // si el comando es "salir", rompe el bucle
        if(strcmp(comando, "salir") == 0){
            break;
        }

        // parseo
        token = strtok(comando," ");
        argsCount = 0;
        background = 0; // Reiniciar bandera de background

        while(token != NULL) {
            args[argsCount] = token;
            argsCount++;
            token = strtok(NULL, " ");
        }

        // Verificar si el último argumento es "&"
        if (argsCount > 0 && strcmp(args[argsCount - 1], "&") == 0) {
            background = 1;          // Marcar para ejecución en background
            args[argsCount - 1] = NULL; // Remover "&" de los argumentos
        } else {
            args[argsCount] = NULL; // Asegurar terminación de la lista de argumentos
        }

        pid = fork();
        // hijo
        if(pid == 0){
            execvp(args[0],args);
            printf("comando no valido \n");
            exit(0);
        }
        // padre
        else if(pid > 0){
            if (!background) { // Si no es background, esperar al hijo
                waitpid(pid,0,0);
            }
            // Si es background, no se llama a waitpid aquí, pero el manejador de SIGCHLD lo gestionará
        }
        // error
        else{
            perror("ERROR FORK \n");
        }
    }

    return 0;
}
