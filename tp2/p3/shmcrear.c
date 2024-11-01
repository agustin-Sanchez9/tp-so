#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

int main(void) {

	int shmid1 = shmget(0xA,4096,0777|IPC_CREAT);

	int semid = semget(0xA,4,0777|IPC_CREAT);	
	
	printf("shmid1 = %d\n",shmid1);
	printf("semid = %d\n",semid);

	semctl(semid,0,SETVAL,8); // S0=8
	semctl(semid,1,SETVAL,0); // S1=0
	semctl(semid,2,SETVAL,8); // S2=8
	semctl(semid,3,SETVAL,0); // S3=0

	return 0;
}
