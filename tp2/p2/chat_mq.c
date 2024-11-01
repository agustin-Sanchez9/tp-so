#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_MSG_LEN 256
#define TYPE1 1 // Tipo de mensaje para el proceso 1
#define TYPE2 2 // Tipo de mensaje para el proceso 2

// Estructura del mensaje
struct msgbuf {
    long mtype;                 // Tipo de mensaje
    char mtext[MAX_MSG_LEN];    // Contenido del mensaje
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <tipo_proceso>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int tipo_proceso = atoi(argv[1]);
    if (tipo_proceso != 1 && tipo_proceso != 2) {
        fprintf(stderr, "Tipo de proceso debe ser 1 o 2\n");
        exit(EXIT_FAILURE);
    }

    // Generar una clave única para la cola de mensajes
    key_t key = ftok("/tmp/chat_mq_key", 'A');
    if (key == -1) {
        perror("Error al generar clave de la cola");
        exit(EXIT_FAILURE);
    }

    // Crear la cola de mensajes
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("Error al crear cola de mensajes");
        exit(EXIT_FAILURE);
    }

    // Variables de tipo de mensaje según el proceso
    long tipo_envio = (tipo_proceso == 1) ? TYPE1 : TYPE2;
    long tipo_lectura = (tipo_proceso == 1) ? TYPE2 : TYPE1;

    pid_t pid = fork();
    if (pid == -1) {
        perror("Error al crear proceso");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Proceso hijo: encargado de recibir mensajes
        struct msgbuf mensaje;
        while (1) {
            // Leer mensaje del tipo correspondiente
            if (msgrcv(msgid, &mensaje, sizeof(mensaje.mtext), tipo_lectura, 0) == -1) {
                perror("Error al recibir mensaje");
                exit(EXIT_FAILURE);
            }
            if (strcmp(mensaje.mtext, "bye\n") == 0) {
                printf("El otro usuario ha salido del chat.\n");
                break;
            }
            printf("%s", mensaje.mtext);
        }
    } else {
        // Proceso padre: encargado de enviar mensajes
        struct msgbuf mensaje;
        mensaje.mtype = tipo_envio;
        while (1) {
            fgets(mensaje.mtext, MAX_MSG_LEN, stdin);

            // Enviar mensaje a la cola
            if (msgsnd(msgid, &mensaje, strlen(mensaje.mtext) + 1, 0) == -1) {
                perror("Error al enviar mensaje");
                break;
            }

            if (strcmp(mensaje.mtext, "bye\n") == 0) {
                break;
            }
        }
        // Esperar a que el proceso hijo termine
        wait(NULL);
    }

    // Eliminar la cola de mensajes cuando el proceso de tipo 1 finaliza
    if (tipo_proceso == 1) {
        msgctl(msgid, IPC_RMID, NULL);
    }

    return 0;
}
