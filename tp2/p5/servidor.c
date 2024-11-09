#include "protocolo.h"

int cola_msg;
FILE *file;

// manejador de señal sigint
void manejador_signal(int signo) {
    if (file) fclose(file);
    msgctl(cola_msg, IPC_RMID, NULL);
    exit(0);
}

void manejar_servidor(int cola, const char *archivo) {
    MensajeCliente msg_cliente;
    MensajeServidor msg_servidor;
    cola_msg = cola;

    // abrir o crear archivo de acceso directo
    file = fopen(archivo, "r+b");
    if (!file) {
        file = fopen(archivo, "w+b");
        for (int i = 0; i < MAX_REGISTROS; i++) {
            msg_servidor.estado = 0;
            memset(msg_servidor.descripcion, 0, MAX_DESC);
            fwrite(&msg_servidor, sizeof(MensajeServidor), 1, file);
        }
    }

    signal(SIGINT, manejador_signal);

    while (1) {
        // esperar mensaje de cliente
        msgrcv(cola, &msg_cliente, sizeof(MensajeCliente) - sizeof(long), 1, 0);

        fseek(file, msg_cliente.num_registro * sizeof(MensajeServidor), SEEK_SET);
        fread(&msg_servidor, sizeof(MensajeServidor), 1, file);

        if (strcmp(msg_cliente.descripcion, "leer") == 0) {
            if (msg_servidor.estado == 1) {
                msg_servidor.tipo = msg_cliente.pid;
                msg_servidor.estado = 1;
            } else {
                msg_servidor.estado = 0;
                snprintf(msg_servidor.descripcion, MAX_DESC, "Registro %d está vacío", msg_cliente.num_registro);
            }
        } else if (strcmp(msg_cliente.descripcion, "borrar") == 0) {
            if (msg_servidor.estado == 1) {
                msg_servidor.estado = 2;
                snprintf(msg_servidor.descripcion, MAX_DESC, "Registro %d borrado", msg_cliente.num_registro);
                
                // sobrescribir registro con el estado actualizado
                fseek(file, msg_cliente.num_registro * sizeof(MensajeServidor), SEEK_SET);
                fwrite(&msg_servidor, sizeof(MensajeServidor), 1, file);
            } else {
                msg_servidor.estado = 0;
                snprintf(msg_servidor.descripcion, MAX_DESC, "Registro %d ya está vacío o borrado", msg_cliente.num_registro);
            }
         } else {
            msg_servidor.estado = 1;
            msg_servidor.num_registro = msg_cliente.num_registro;
            strcpy(msg_servidor.descripcion, msg_cliente.descripcion);
            fseek(file, msg_cliente.num_registro * sizeof(MensajeServidor), SEEK_SET);
            fwrite(&msg_servidor, sizeof(MensajeServidor), 1, file);
        }

        // enviar respuesta al cliente
        msg_servidor.tipo = msg_cliente.pid;
        msgsnd(cola, &msg_servidor, sizeof(MensajeServidor) - sizeof(long), 0);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <archivo>\n", argv[0]);
        exit(1);
    }

    key_t key = COLA_MSG;
    int cola = msgget(key, 0666 | IPC_CREAT);
    manejar_servidor(cola, argv[1]);
    return 0;
}