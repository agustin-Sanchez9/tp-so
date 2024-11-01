#include "mysem.h"

int semWait(int semid,int semnum) {
	struct sembuf buf;
	buf.sem_num = semnum;  
	buf.sem_op  = -1;  
	buf.sem_flg = 0;

	int ret = semop(semid,&buf,1);

	if(ret != 0) {
		// error
		return 0;
	} else { 
		// todo ok
		return 1;
	}
}

int semSignal(int semid,int semnum) {
	struct sembuf buf;
	buf.sem_num = semnum;
	buf.sem_op  = 1;
	buf.sem_flg = 0;

	int ret = semop(semid,&buf,1);

	if(ret != 0) {
		// error
		return 0;
	} else { 
		// todo ok
		return 1;
	}
}
