#ifndef PROTOCOLO_H
#define PROTOCOLO_H
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define COLA_MSG 0xA
#define MAX_DESC 100
#define MAX_REGISTROS 1000

// estructura del mensaje del cliente al servidor
typedef struct {
    long tipo;               
    int pid;                 
    int num_registro;        
    char descripcion[MAX_DESC]; 
} MensajeCliente;

// estructura del mensaje de respuesta del servidor al cliente
typedef struct {
    long tipo;               
    int estado; // 0 para error, 1 para éxito
    int num_registro;        
    char descripcion[MAX_DESC]; 
} MensajeServidor;

// prototipos de funciones
void enviar_mensaje_cliente(int cola, MensajeCliente *msg);
void recibir_mensaje_servidor(int cola, MensajeServidor *msg, int pid);
void manejar_cliente(int cola);
void manejar_servidor(int cola, const char *archivo);


#endif