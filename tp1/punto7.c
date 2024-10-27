#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void clean_command(char *command);
void *handle_client(void *client_socket);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // crear el socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // definir la dirección del servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // enlazar el socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // escucha conexiones entrantes
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor esperando conexiones...\n");

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Conexión aceptada\n");

        // crear un nuevo hilo para manejar el cliente
        pthread_t thread_id;
        int *client_socket = malloc(sizeof(int));
        *client_socket = new_socket;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)client_socket) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        // libero recursos cuando el hilo finaliza
        pthread_detach(thread_id);
    }

    return 0;
}


void clean_command(char *command) {
    int len = strlen(command);
    for (int i = 0; i < len; i++) {
        if (command[i] == '\r' || command[i] == '\n') {
            command[i] = '\0';
        }
    }
}

void *handle_client(void *client_socket) {
    int new_socket = *((int *)client_socket);
    free(client_socket);

    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = read(new_socket, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';

        clean_command(buffer);  // limpiar el comando

        if (strcmp(buffer, "salir") == 0) {
            printf("Cliente desconectado\n");
            break;
        }

        printf("Comando recibido: %s\n", buffer);

        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // proceso hijo
            close(pipefd[0]); // cierro el extremo de lectura

            // redirijo hacia el pipe
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);

            char *args[] = {"/bin/sh", "-c", buffer, NULL};
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {
            // proceso padre
            close(pipefd[1]); // cierro el extremo de escritura
            char result[BUFFER_SIZE];
            int read_bytes;
            while ((read_bytes = read(pipefd[0], result, sizeof(result) - 1)) > 0) {
                result[read_bytes] = '\0';
                send(new_socket, result, read_bytes, 0);
            }
            close(pipefd[0]);
            wait(NULL);
        }
    }

    close(new_socket);
    pthread_exit(NULL);
}