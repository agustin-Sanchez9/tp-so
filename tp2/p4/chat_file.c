#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/wait.h>

#define MAX_MSG_SIZE 256
#define FILE_CHAT "chat.txt"

typedef struct {
    int user_id;
    char timestamp[20];
    char content[MAX_MSG_SIZE];
} Message;

void getCurrentTime(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%H:%M:%S", t);
}

void sendMessage(int user_id, const char *content) {
    FILE *file = fopen(FILE_CHAT, "a");
    if (file == NULL) {
        perror("Error al abrir el archivo de chat");
        return;
    }

    // Bloquear el archivo
    flock(fileno(file), LOCK_EX);

    // Crear el mensaje
    Message msg;
    msg.user_id = user_id;
    getCurrentTime(msg.timestamp, sizeof(msg.timestamp));
    strncpy(msg.content, content, MAX_MSG_SIZE);

    // Escribir el mensaje en el archivo
    fprintf(file, "%d [%s]: %s\n", msg.user_id, msg.timestamp, msg.content);

    // Desbloquear y cerrar el archivo
    flock(fileno(file), LOCK_UN);
    fclose(file);
}

void receiveMessages(int *last_message_pos) {
    FILE *file = fopen(FILE_CHAT, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo de chat");
        return;
    }

    // Bloquear el archivo para lectura
    flock(fileno(file), LOCK_SH);

    // Mover al último mensaje leído
    fseek(file, *last_message_pos, SEEK_SET);

    // Leer y mostrar mensajes nuevos
    char line[300];
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }

    // Actualizar la posición actual para la próxima lectura
    *last_message_pos = ftell(file);

    // Desbloquear y cerrar el archivo
    flock(fileno(file), LOCK_UN);
    fclose(file);
}

void listenForMessages(int *last_message_pos) {
    while (1) {
        receiveMessages(last_message_pos);
        usleep(500000); // Pausa de 500 ms antes de volver a leer
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <user_id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int user_id = atoi(argv[1]);
    char buffer[MAX_MSG_SIZE];
    int last_message_pos = 0;

    // Crear un proceso hijo para escuchar mensajes
    pid_t pid = fork();

    if (pid == 0) {
        // Proceso hijo: se encarga de recibir mensajes
        listenForMessages(&last_message_pos);
        exit(0);
    } else if (pid > 0) {
        // Proceso padre: se encarga de enviar mensajes
        while (1) {
            fgets(buffer, MAX_MSG_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';

            if (strcmp(buffer, "bye") == 0) {
                printf("Cerrando chat.\n");
                break;
            }

            // Enviar el mensaje
            sendMessage(user_id, buffer);
        }

        // Esperar a que el proceso hijo termine
        kill(pid, SIGTERM); // Terminar el proceso de escucha
        wait(NULL);          // Esperar al proceso hijo
    } else {
        perror("Error al crear el proceso");
        exit(EXIT_FAILURE);
    }

    return 0;
}
