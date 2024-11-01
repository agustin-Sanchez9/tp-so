#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>


//devuelve verdad si pudo hacer semWait() ok, caso contrario devuelve falso
int semWait(int semid,int semnum);    // P()
//devuelve verdad si pudo hacer semSignal() ok, caso contrario devuelve falso
int semSignal(int semid,int semnum);  // V()

