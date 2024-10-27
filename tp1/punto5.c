#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>

void proceso_padre(int, int);
void proceso_hijo_b(int, int);
void proceso_hijo_c(int, int);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("error, solo se permite 1 argumento para el proceso.\n");
        exit(1);
    }

    int n = atoi(argv[1]);
    if (n < 0) {
        printf("el numero de procesos hermanos debe ser mayor a 0\n");
        exit(1);
    }

    // crear archivo donde escribirán los procesos
    int fd = open("punto5-fd", O_CREAT | O_WRONLY | O_TRUNC, 0777);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    sem_unlink("sem_padre");
    sem_unlink("sem_hijo_b");
    sem_unlink("sem_hijo_c");

    // crear semáforos
    sem_t *sem_padre = sem_open("sem_padre", O_CREAT, 0600, 1);
    sem_t *sem_hijo_b = sem_open("sem_hijo_b", O_CREAT, 0600, 0);
    sem_t *sem_hijo_c = sem_open("sem_hijo_c", O_CREAT, 0600, 0);

    if (sem_padre == SEM_FAILED || sem_hijo_b == SEM_FAILED || sem_hijo_c == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    pid_t pid_b, pid_c;

    // creación de proceso hijo B
    pid_b = fork();
    if (pid_b < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid_b == 0) {
        proceso_hijo_b(n, fd);
        exit(EXIT_SUCCESS);
    }

    // creación de proceso hijo C
    pid_c = fork();
    if (pid_c < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid_c == 0) {
        proceso_hijo_c(n, fd);
        exit(EXIT_SUCCESS);
    }

    // ejecución del padre
    proceso_padre(n, fd);

    wait(NULL);
    wait(NULL);

    // cerrar semáforos
    sem_close(sem_padre);
    sem_close(sem_hijo_b);
    sem_close(sem_hijo_c);
    sem_unlink("sem_padre");
    sem_unlink("sem_hijo_b");
    sem_unlink("sem_hijo_c");

    // cerrar archivo
    close(fd);

    return 0;
}

void proceso_padre(int n, int fd) {
    sem_t *sem_padre = sem_open("sem_padre", 0);
    sem_t *sem_hijo_b = sem_open("sem_hijo_b", 0);
    sem_t *sem_hijo_c = sem_open("sem_hijo_c", 0);

    char letra = 'A';

    for (int i = 0; i < n; i++) {
        sem_wait(sem_padre);
        write(fd, &letra, 1);
        sem_post(sem_hijo_b);

        sem_wait(sem_padre);
        write(fd, &letra, 1);
        sem_post(sem_hijo_c);
    }

    sem_close(sem_padre);
    sem_close(sem_hijo_b);
    sem_close(sem_hijo_c);
}

void proceso_hijo_b(int n, int fd) {
    sem_t *sem_padre = sem_open("sem_padre", 0);
    sem_t *sem_hijo_b = sem_open("sem_hijo_b", 0);

    char letra = 'B';

    for (int i = 0; i < n; i++) {
        sem_wait(sem_hijo_b);
        write(fd, &letra, 1);
        sem_post(sem_padre);
    }

    sem_close(sem_padre);
    sem_close(sem_hijo_b);
}

void proceso_hijo_c(int n, int fd) {
    sem_t *sem_padre = sem_open("sem_padre", 0);
    sem_t *sem_hijo_c = sem_open("sem_hijo_c", 0);

    char letra = 'C';

    for (int i = 0; i < n; i++) {
        sem_wait(sem_hijo_c);
        write(fd, &letra, 1);
        sem_post(sem_padre);
    }

    sem_close(sem_padre);
    sem_close(sem_hijo_c);
}
