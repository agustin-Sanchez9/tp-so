#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

// gcc -Wall -o shmborrar shmborrar.c -I. mysem.c

int main(void) {
	int shmid1 = shmget(0xA,0,0);
	int semid = semget(0xA,0,0);	

	printf("shmid1 = %d\n",shmid1);
	printf("semid = %d\n",semid);

	// borro arreglo de semaforos SVR4
	semctl(semid,0,IPC_RMID);
	// borro memoria compartida
	shmctl(shmid1,IPC_RMID,0);

	return 0;
}

