#include <sys/types.h>

#define MAX_DESCRIPCION 100

// Estructura de mensajes de cliente a servidor
typedef struct {
    long mtype;                   
    pid_t pid;                    
    int numero_registro;          
    char descripcion[MAX_DESCRIPCION];
} mensaje_cliente;

// Estructura de mensajes de servidor a cliente
typedef struct {
    long mtype;                   
    int retorno;                  
    int numero_registro;          
    char descripcion[MAX_DESCRIPCION]; 
} mensaje_servidor;



