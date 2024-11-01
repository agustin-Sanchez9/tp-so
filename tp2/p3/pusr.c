#include "mysem.h"

/* Creo memoria compartida, semaforos e inicializo semaforos */

// gcc -Wall -o shmcrear shmcrear.c -I. mysem.c
int main(int argc,char *argv[]) {
	if ( argc != 2 ) {
		printf("Forma de Uso:\n$ ./pusr <id usuario>\n");
		exit(-1);
	}
	// shm de escritura y lectura
	int shmw=0,idusr=atoi(argv[1]),i=0,N=8;
	int idpsem0,idpsem1;
	int idhsem0,idhsem1;
	char *pshmw, *pshmr;
	int offsetw,offsetr; // desplazamientos dentro de la shm para escritura y lectura
	if ( idusr == 1 ) {
		shmw=shmget(0xA,0,0);
		idpsem0 = 0;
		idpsem1 = 1;
		idhsem0 = 3;
		idhsem1 = 2;
		offsetw=0;
		offsetr=8*256;
	} else { // asumo que es 2
		shmw=shmget(0xA,0,0);
		idpsem0 = 2;
		idpsem1 = 3;
		idhsem0 = 1;
		idhsem1 = 0;
		offsetw=8*256;
		offsetr=0;
	}
	char *p = (char *) shmat(shmw,0,0);
	pshmw = p + offsetw;
	pshmr = p + offsetr;
	int semid = semget(0xA,0,0);	
	char linea[256];
	
	pid_t pid = fork();
	if ( pid > 0 ) { // escritor
		do {
			printf("%d>",idusr);
			fgets(linea,256,stdin);
			// si el usuario es 1 el valor de idsem0 es 0
			// si el usuario es 2 el valor de idsem0 es 2
			//printf("usuario=%d idpsem0=%d idpsem1=%d\n",idusr,idpsem0,idpsem1);
			semWait(semid,idpsem0); // P(S0)
				// SC
				linea[strlen(linea)-1] = '\0';
				strcpy(pshmw+(i*256),linea);
			// si el usuario es 1 el valor de idsem1 es 1
			// si el usuario es 2 el valor de idsem1 es 3
			semSignal(semid,idpsem1); // V(S1)
			i=(i+1)%N;
		} while(strncmp(linea,"chau",4) != 0);
		wait(0);	
		printf("fin escritor(%d)\n",idusr);
	} else {	// lector
		do {
			//printf("usuario=%d idhsem0=%d idhsem1=%d\n",idusr,idhsem0,idhsem1);
			semWait(semid,idhsem0); // P(S1)
				// SC
				strcpy(linea,pshmr+(i*256));
			semSignal(semid,idhsem1); // V(S0)
			printf("%d<%s\n",idusr,linea);
			i=(i+1)%N;
		} while(strncmp(linea,"chau",4) != 0);
		printf("fin lector(%d)\n",idusr);
	}	
	
	// desconecto memoria compartida
	shmdt(pshmw);
	//shmdt(pshmr);
	printf("fin(%d)\n",idusr);
	return 0;
}
