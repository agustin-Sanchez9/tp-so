#include "protocolo.h"

int cola_msg;
FILE *file;

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
            msg_servidor.estado = 0;  // registro libre
            msg_servidor.pid_lock = 0; // sin lock
            memset(msg_servidor.descripcion, 0, MAX_DESC);
            fwrite(&msg_servidor, sizeof(MensajeServidor), 1, file);
        }
    }

    signal(SIGINT, manejador_signal);

    while (1) {
        msgrcv(cola, &msg_cliente, sizeof(MensajeCliente) - sizeof(long), 1, 0);

        // leer estado del registro
        fseek(file, msg_cliente.num_registro * sizeof(MensajeServidor), SEEK_SET);
        fread(&msg_servidor, sizeof(MensajeServidor), 1, file);

        if (strcmp(msg_cliente.descripcion, "lock") == 0) {
            if (msg_servidor.pid_lock == 0) {
                msg_servidor.pid_lock = msg_cliente.pid;
                msg_servidor.estado = 1;
                snprintf(msg_servidor.descripcion, MAX_DESC, "Registro %d bloqueado con éxito", msg_cliente.num_registro);

                // actualizar pid_lock y estado
                fseek(file, msg_cliente.num_registro * sizeof(MensajeServidor) + offsetof(MensajeServidor, pid_lock), SEEK_SET);
                fwrite(&msg_servidor.pid_lock, sizeof(int), 1, file);
                fwrite(&msg_servidor.estado, sizeof(int), 1, file);
                fflush(file);
            } else {
                msg_servidor.estado = 0;
                snprintf(msg_servidor.descripcion, MAX_DESC, "Registro %d ya bloqueado por pid %d", msg_cliente.num_registro, msg_servidor.pid_lock);
            }
        } else if (strcmp(msg_cliente.descripcion, "unlock") == 0) {
            if (msg_servidor.pid_lock == msg_cliente.pid) {
                msg_servidor.pid_lock = 0;
                msg_servidor.estado = 1;
                snprintf(msg_servidor.descripcion, MAX_DESC, "Registro %d desbloqueado con éxito", msg_cliente.num_registro);

                // actualizar pid_lock y estado               
                fseek(file, msg_cliente.num_registro * sizeof(MensajeServidor) + offsetof(MensajeServidor, pid_lock), SEEK_SET);
                fwrite(&msg_servidor.pid_lock, sizeof(int), 1, file);
                fwrite(&msg_servidor.estado, sizeof(int), 1, file);
                fflush(file);
            } else {
                msg_servidor.estado = 0;
                snprintf(msg_servidor.descripcion, MAX_DESC, "Registro %d no está bloqueado por pid %d", msg_cliente.num_registro, msg_cliente.pid);
            }
        } else if (msg_servidor.pid_lock == msg_cliente.pid) {
            if (strcmp(msg_cliente.descripcion, "leer") == 0) {
                msg_servidor.tipo = msg_cliente.pid;
                if (msg_servidor.estado == 1 && strlen(msg_servidor.descripcion) > 0) {
                } else {
                    snprintf(msg_servidor.descripcion, MAX_DESC, "Registro %d está vacío", msg_cliente.num_registro);
                }
            } else if (strcmp(msg_cliente.descripcion, "borrar") == 0) {
                if (msg_servidor.estado == 1) {
                    msg_servidor.estado = 0;
                    // msg_servidor.pid_lock = 0;
                    memset(msg_servidor.descripcion, 0, MAX_DESC);
                    snprintf(msg_servidor.descripcion, MAX_DESC, "Registro %d borrado", msg_cliente.num_registro);

                    fseek(file, msg_cliente.num_registro * sizeof(MensajeServidor), SEEK_SET);
                    fwrite(&msg_servidor, sizeof(MensajeServidor), 1, file);
                    fflush(file);
                } else {
                    msg_servidor.estado = 0;
                    snprintf(msg_servidor.descripcion, MAX_DESC, "Registro %d ya está vacío o borrado", msg_cliente.num_registro);
                }
            } else { // escritura
                msg_servidor.estado = 1;
                msg_servidor.num_registro = msg_cliente.num_registro;
                strncpy(msg_servidor.descripcion, msg_cliente.descripcion, MAX_DESC);

                // actualizar registro
                fseek(file, msg_cliente.num_registro * sizeof(MensajeServidor), SEEK_SET);
                fwrite(&msg_servidor, sizeof(MensajeServidor), 1, file);
                fflush(file);
            }
        } else {
            msg_servidor.estado = 0;
            snprintf(msg_servidor.descripcion, MAX_DESC, "No tiene el lock para el registro %d", msg_cliente.num_registro);
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
