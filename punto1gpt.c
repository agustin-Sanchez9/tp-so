#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

// handler para la señal SIGCHLD
void handle_sigchld(int sig) {
    int status;
    pid_t pid;
    // espera final de todos los procesos hijos
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("Proceso %d terminó con estado %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Proceso %d terminó por señal %d\n", pid, WTERMSIG(status));
        }
    }
}

int main(void) {
    // variables
    pid_t pid;
    char comando[64];
    char *token;
    char *args[11]; // 10 argumentos + 1 por el NULL al final
    int background; // 0 = no y 1 = si

    // Configurar el manejador de SIGCHLD
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Error al configurar el manejador de SIGCHLD");
        exit(EXIT_FAILURE);
    }

    // introduccion del comando
    printf("$>> ");
    while (fgets(comando, sizeof(comando), stdin) != NULL) {
        comando[strcspn(comando, "\n")] = '\0'; // Eliminar el salto de línea al final del comando

        // parseo
        int argsCount = 0;
        token = strtok(comando, " ");
        while (token != NULL && argsCount < 10) {
            args[argsCount] = token;
            argsCount++;
            token = strtok(NULL, " ");
        }
        args[argsCount] = NULL; // Terminar la lista de argumentos con NULL

        // Comprobar si el último argumento es "&"
        if (argsCount > 0 && strcmp(args[argsCount - 1], "&") == 0) {
            background = 1;
            args[argsCount - 1] = NULL; // Remover el "&" de los argumentos
        } else {
            background = 0;
        }

        if (args[0] == NULL) {
            printf("$>> ");
            continue; // Si no se ingresó comando, solicitar nuevamente
        }

        if (strcmp(args[0], "salir") == 0) {
            break; // Salir del bucle si el comando es "salir"
        }

        pid = fork();
        // hijo
        if (pid == 0) {
            execvp(args[0], args);
            perror("comando no valido");
            exit(EXIT_FAILURE);
        }
        // padre
        else if (pid > 0) {
            if (!background) {
                waitpid(pid, NULL, 0); // Esperar solo si no es en background
            }
            printf("$>> ");
        }
        // error
        else {
            perror("ERROR FORK");
        }
    }

    return 0;
}
