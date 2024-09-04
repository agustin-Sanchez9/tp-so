#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    // verificar uso correcto de los argumentos
    if (argc < 2) {
        fprintf(stderr, "argumentos insuficientes\n");
        exit(0);
    }
    
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        printf("fallo del pipe()\n");
        exit(0);
    }

    pid_t pid = fork();

    if (pid == -1) {
        printf("fallo de fork()\n");
        exit(0);
    }

    // proceso hijo
    if (pid == 0) { 
        // cerrar el extremo de lectura del pipe
        close(pipefd[0]); 

        // redirigir la salida estandar al extremo de escritura del pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(0);
        }
        
        // cerrar el descriptor duplicado
        close(pipefd[1]); 

        // ejecutar el comando
        execvp(argv[1], &argv[1]);


    } 
    // proceso padre
    else { 
        // cerrar el extremo de escritura del pipe
        close(pipefd[1]); 

        char buffer[1024];
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            printf("%s", buffer);
        }

        // cerrar el extremo de lectura del pipe
        close(pipefd[0]); 

        // esperar al hijo
        if (waitpid(pid, NULL, 0) == -1) {
            printf("fallo waitpid");
            exit(0);
        }
    }

    return 0;
}
