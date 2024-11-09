#include "protocolo.h"
#include <unistd.h>

void enviar_mensaje_cliente(int cola, MensajeCliente *msg) {
    msgsnd(cola, msg, sizeof(MensajeCliente) - sizeof(long), 0);
}

void recibir_mensaje_servidor(int cola, MensajeServidor *msg, int pid) {
    msgrcv(cola, msg, sizeof(MensajeServidor) - sizeof(long), pid, 0);
}

void manejar_cliente(int cola) {
    MensajeCliente msg_cliente;
    MensajeServidor msg_servidor;
    char entrada[200];
    int pid, num_registro;
    char descripcion[MAX_DESC];

    msg_cliente.tipo = 1; 
    msg_cliente.pid = getpid();

    printf("FORMATO DEL MENSAJE <pid>,<número de registro>,<descripción>:\n");
    printf("EJEMPLO LEER: %d,1,leer\n", msg_cliente.pid);
    printf("EJEMPLO BORRAR: %d,2,borrar\n", msg_cliente.pid);
    printf("EJEMPLO ESCRIBIR: %d,10,hola mundo\n", msg_cliente.pid);
    printf("EJEMPLO LOCK: %d,5,lock\n", msg_cliente.pid);
    printf("EJEMPLO UNLOCK: %d,5,unlock\n\n", msg_cliente.pid);


    while (1) {
        fgets(entrada, sizeof(entrada), stdin);
        if (sscanf(entrada, "%d,%d,%99[^\n]", &pid, &num_registro, descripcion) != 3) {
            printf("Formato incorrecto. Intente de nuevo.\n");
            continue;
        }

        msg_cliente.pid = pid;
        msg_cliente.num_registro = num_registro;
        strncpy(msg_cliente.descripcion, descripcion, MAX_DESC);

        enviar_mensaje_cliente(cola, &msg_cliente);
        recibir_mensaje_servidor(cola, &msg_servidor, msg_cliente.pid);

        printf("SERVER: %s\n", msg_servidor.descripcion);
    }
}

int main() {
    key_t key = COLA_MSG;
    int cola = msgget(key, 0666 | IPC_CREAT);
    manejar_cliente(cola);
    return 0;
}
