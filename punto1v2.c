#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

// flag para manejar signint
volatile sig_atomic_t sigint_received = 0; 

// handler para la señal SIGCHLD
void sigchld_handler(int s) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// handler para la señal SIGINT
void sigint_handler(int s) {
    sigint_received = 1; // cambio la flag
}

int main(void) {

    // seteo los handler
    signal(SIGCHLD, sigchld_handler);
    signal(SIGINT, sigint_handler);

    // variables
    pid_t pid;
    char comando[1000];
    char *token;
    char *args[11]; // 10 argumentos + 1 por el NULL al final
    int argsCount = 0;
    int background = 0; // flag para ejecucion en background
    int salir = 0; // flag para salir del bucle

    while(!salir) {
        // introduccion del comando
        printf("$>> ");
        memset(comando, '\0', 1000); // seteo a comando con todo \0
        fgets(comando, 1000, stdin); // leo lo ingresado por teclado
        comando[strcspn(comando, "\n")] = '\0'; // eliminar el salto de línea

        // si el comando es "salir", cambiar flag salir
        if(strcmp(comando, "salir") == 0) {
            salir = 1;
            continue;
        }

        // si se recibio SIGINT, cambiar flag salir y continuar para salir del bucle
        if(sigint_received) {
            salir = 1;
            continue;
        }

        // parseo
        token = strtok(comando, " ");
        argsCount = 0;
        background = 0; // reiniciar flag de background

        while(token != NULL) {
            args[argsCount] = token;
            argsCount++;
            token = strtok(NULL, " ");
        }

        // verificar si el ultimo argumento es "&"
        if (argsCount > 0 && strcmp(args[argsCount - 1], "&") == 0) {
            background = 1;             // marcar para ejecución en background
            args[argsCount - 1] = NULL; // remover "&" de los argumentos
        } else {
            args[argsCount] = NULL; // asegurar terminacion de la lista de argumentos
        }

        pid = fork();
        // codigo proceso hijo
        if(pid == 0) {
            execvp(args[0], args);
            printf("comando no valido \n");
            exit(0);
        }
        // codigo proceso padre
        else if(pid > 0) {
            if (!background) { // si no es background, esperar al hijo
                waitpid(pid, 0, 0);
            }
            // si es background, lo manjena el handler de SIGCHLD
        }
        // error
        else {
            perror("ERROR FORK \n");
        }
    }

    // si se recibio SIGINT, esperar a que todos los procesos hijos terminen
    if (sigint_received) {
        printf("\nEsperando a que terminen todos los procesos hijos...\n");
        while(waitpid(-1, NULL, 0) > 0);
        printf("Todos los procesos hijos han terminado. Saliendo.\n");
    }

    return 0;
}
