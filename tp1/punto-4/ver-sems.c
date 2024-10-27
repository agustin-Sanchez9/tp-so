#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main(void){
    int semid = semget(0xa,0,0);
    if (semid != -1){
        printf("sem 0 = %d\n",semctl(semid,0,GETVAL));
        printf("sem 1 = %d\n",semctl(semid,1,GETVAL));
        printf("sem 2 = %d\n",semctl(semid,2,GETVAL));
        printf("sem 3 = %d\n",semctl(semid,3,GETVAL));
    }
    else{
        printf("error semget\n");
    }
    exit(0);
}