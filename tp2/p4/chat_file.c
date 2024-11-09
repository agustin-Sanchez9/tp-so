#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/wait.h>

// utilice un archivo .txt para la comunicacion
// simplemente me parecio la forma mas facil y rapida para llegar a la solucion
// ademas de que tiene la pequeña ventaje que podemos abrir el archivo para ver mensajes pasados

#define MAX_MSG_SIZE 256
#define FILE_CHAT "chat.txt"

struct Message {
    int user_id;
    char timestamp[20];  // usando para debug borrar
    char content[MAX_MSG_SIZE];
};

void get_current_time(char *buffer, size_t size) { 
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%H:%M:%S", t);
}

void send_msg(int user_id, const char *content) {
    FILE *file = fopen(FILE_CHAT, "a");
    if (file == NULL) {
        perror("ERROR AL ABRIR EL ARCHIVO");
        return;
    }


    struct Message msg;
    msg.user_id = user_id;
    get_current_time(msg.timestamp, sizeof(msg.timestamp));
    strncpy(msg.content, content, MAX_MSG_SIZE);

    fprintf(file, "%d [%s]: %s\n", msg.user_id, msg.timestamp, msg.content);

    fclose(file);
}

void read_msg(int *last_message_pos) {
    FILE *file = fopen(FILE_CHAT, "r");
    if (file == NULL) {
        perror("ERROR AL ABRIR EL ARCHIVO");
        return;
    }

    fseek(file, *last_message_pos, SEEK_SET);

    char line[300];
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }

    // actualizar la posicion actual para la proxima lectura
    *last_message_pos = ftell(file);

    fclose(file);
}

void wait_msg(int *last_message_pos) {
    while (1) {
        read_msg(last_message_pos);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "FORMA DE USO: %s <user_id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int user_id = atoi(argv[1]);
    char buffer[MAX_MSG_SIZE];
    int last_message_pos = 0;

    // proceso hijo para leer mensajes
    pid_t pid = fork();

    if (pid == 0) {
        wait_msg(&last_message_pos);
        exit(0);
    } else if (pid > 0) {
        while (1) {
            fgets(buffer, MAX_MSG_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';

            if (strcmp(buffer, "bye") == 0) {
                printf("CERRANDO CHAT\n");
                break;
            }

            send_msg(user_id, buffer);
        }

        kill(pid, SIGTERM);
        wait(NULL);
    } else {
        perror("ERROR AL CREAR EL PROCESO");
        exit(EXIT_FAILURE);
    }

    return 0;
}
