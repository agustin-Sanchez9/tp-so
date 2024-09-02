#ifndef PVSEM_H
#define PVSEM_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// P bloquea un semáforo
void P(int semid, int sem);

// V desbloquea un semáforo
void V(int semid, int sem);

#endif // PVSEM_H
