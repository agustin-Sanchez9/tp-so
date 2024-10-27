#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>

/* para simular la seccion critica los hilos escribiran en consola la letra correspodiente a cada uno
y modificaran una variable, siendo que el hilo A sumara 1 y los hilos B y C restaran 1 
de forma que la variable debera terminar con el mismo valor con le que comienza (10). */

// creo var global
int var = 10;

// incio los mutex para cada hilo
pthread_mutex_t mA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mC = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mX = PTHREAD_MUTEX_INITIALIZER; 

// rutinas de los hilos
void *rutina_hiloA(void *arg){
    pthread_mutex_lock(&mA);
    printf("A");
    var++;
    pthread_mutex_unlock(&mX);
    pthread_exit("salida hilo A");
    return 0;
}

void *rutina_hiloB(void *arg){
    pthread_mutex_lock(&mB);
    pthread_mutex_lock(&mX);
    printf("B");
    var--;
    pthread_mutex_unlock(&mA);
    pthread_mutex_unlock(&mC);
    pthread_exit("salida hilo B");
    return 0;

}

void *rutina_hiloC(void *arg){
    pthread_mutex_lock(&mC);
    pthread_mutex_lock(&mX);
    printf("C\n");
    var--;
    pthread_mutex_unlock(&mA);
    pthread_mutex_unlock(&mB);
    pthread_exit("salida hilo C");
    return 0;
}



int main(int argc, char *argv[]){

    // mensaje de error por exceso de argumentos.
    if(argc!=2){
        printf("error, solo se permite 1 argumento para el proceso.");
        exit(1);
    }

    int n = atoi(argv[1]);

    // chequeo que argv[1] sea positivo.
    if(n<0){
        printf("el numero de procesos hermanos debe ser mayor a 0\n");
        exit(1);
    }

    pthread_t hiloA, hiloB, hiloC;


    // cierro el mutex C y X
    // mA = 1 ; mB = 1 ; mC = 0 ; mX = 0
    pthread_mutex_lock(&mC);
    pthread_mutex_lock(&mX);


    // creacion de hilos
    for(int i=0 ; i < n; i++){
        pthread_create(&hiloA,NULL,rutina_hiloA,NULL);
        pthread_create(&hiloB,NULL,rutina_hiloB,NULL);
        pthread_create(&hiloA,NULL,rutina_hiloA,NULL);
        pthread_create(&hiloC,NULL,rutina_hiloC,NULL);
    }

    // join para los hilos
    for(int i=0 ; i < n ; i++){
        pthread_join(hiloA,NULL);
        pthread_join(hiloB,NULL);
        pthread_join(hiloC,NULL);
    }

    // destruir mutex para liberar recursos
    pthread_mutex_destroy(&mA);
    pthread_mutex_destroy(&mB);
    pthread_mutex_destroy(&mC);
    pthread_mutex_destroy(&mX);

    printf("debug var: %i\n",var);

    return 0;
}