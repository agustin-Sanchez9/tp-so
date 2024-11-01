#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_MSG_LEN 256

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <fifo_escritura> <fifo_lectura>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *fifo_write = argv[1];
    const char *fifo_read = argv[2];

    // crear los FIFOs si no existen
    mkfifo(fifo_write, 0666);
    mkfifo(fifo_read, 0666);

    char mensaje[MAX_MSG_LEN];
    int fd_write, fd_read;

    // abrir el FIFO de escritura en modo lectura/escritura para evitar bloqueo
    fd_write = open(fifo_write, O_RDWR);
    if (fd_write == -1) {
        perror("Error al abrir fifo_write");
        exit(EXIT_FAILURE);
    }

    // abrir el FIFO de lectura en modo solo lectura
    fd_read = open(fifo_read, O_RDONLY);
    if (fd_read == -1) {
        perror("Error al abrir fifo_read");
        close(fd_write);
        exit(EXIT_FAILURE);
    }

    printf("Chat iniciado. Escribe 'bye' para salir.\n");

    pid_t pid = fork();
    if (pid == -1) {
        perror("Error al crear el proceso");
        close(fd_write);
        close(fd_read);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // proceso hijo: encargado de leer mensajes
        while (1) {
            memset(mensaje, 0, MAX_MSG_LEN);
            if (read(fd_read, mensaje, MAX_MSG_LEN) > 0) {
                if (strcmp(mensaje, "bye\n") == 0) {
                    printf("El otro usuario ha salido del chat.\n");
                    break;
                }
                printf("%s", mensaje);
            }
        }
    } else {
        // proceso padre: encargado de enviar mensajes
        while (1) {
            fgets(mensaje, MAX_MSG_LEN, stdin);

            if (write(fd_write, mensaje, strlen(mensaje) + 1) == -1) {
                perror("Error al enviar el mensaje");
                break;
            }

            if (strcmp(mensaje, "bye\n") == 0) {
                printf("Saliendo del chat\n");
                break;
            }
        }
        
        wait(NULL);
    }

    close(fd_write);
    close(fd_read);


    unlink(fifo_write);
    unlink(fifo_read);

    return 0;
}
