#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <parametro>\n", argv[0]);
        return 1;
    }

    char *parametro = argv[1];
    char *programas[] = {"./apunto4", "./bpunto4", "./cpunto4"};

    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Error en fork");
            return 1;
        }

        if (pid == 0) {
            // Proceso hijo: ejecuta uno de los programas con el parámetro dado
            execl(programas[i], programas[i], parametro, NULL);
            perror("Error en execl");
            exit(1);
        }
    }

    // Proceso padre espera a que todos los hijos terminen
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    printf("Todos los programas han terminado.\n");

    return 0;
}
