#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    // ruta definida para la key
    const char *path = "/tmp/cliente_servidor";

    // eliminar el archivo
    if (remove(path) == 0) {
        printf("ARCHIVO key = %s ELIMINADO CON EXITO\n", path);
    } else {
        perror("ERROR AL ELIMINAR EL ARCHIVO");
    }

    return 0;
}
