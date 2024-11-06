#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>


#define BUFFER_SIZE 256
#define EXIT_COMMAND "bye"

// estructura para el mensaje
struct mensaje {
    char msg[BUFFER_SIZE];
    int listo[2];
};

// funciones P y V para manejo de semaforos
void P(int semid, int sem) {
    struct sembuf buf;
    buf.sem_num = sem;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    semop(semid, &buf, 1); 
}

void V(int semid, int sem) {
    struct sembuf buf;
    buf.sem_num = sem;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    semop(semid, &buf, 1); 
}

int main(int argc, char *argv[]) {

    // ubicacion de la key creada con create_key.c
    const char *path = "/tmp/chat_shm_key";

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <numero de proceso (0 o 1)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int proc_num = atoi(argv[1]);
    if (proc_num != 0 && proc_num != 1) {
        fprintf(stderr, "ERROR, EL NUMER DE PROCESO DEBE SER 0 O 1\n");
        exit(EXIT_FAILURE);
    }

    key_t key = ftok(path, 65);
    int shmid = shmget(key, sizeof(struct mensaje), 0666 | IPC_CREAT);
    struct mensaje *mem = (struct mensaje *)shmat(shmid, NULL, 0);

    // creacion de semaforos
    int semid = semget(key, 2, 0666 | IPC_CREAT);
    semctl(semid, 0, SETVAL, 1); // sem de escritura
    semctl(semid, 1, SETVAL, 1); // sem de lectura

    mem->listo[0] = 0;
    mem->listo[1] = 0;

    pid_t pid = fork();

    if (pid == 0) {  // proceso hijo que recibe mensajes
        while (1) {
            P(semid, 1);
            if (mem->listo[1 - proc_num] == 1) {
                printf("%s\n", mem->msg);
                if (strcmp(mem->msg, EXIT_COMMAND) == 0) {
                    printf("EL OTRO USUARIO A SALIDO DEL CHAT. ESCRIBA 'bye' PARA SALIR TAMBIEN\n");
                    break;
                }
                mem->listo[1 - proc_num] = 0;
                fflush(stdout);
            }
            V(semid, 1);
        }
    } else {  // proceso padre que envia mensajes
        while (1) {
            char buffer[BUFFER_SIZE];
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            
            P(semid, 0);
            strncpy(mem->msg, buffer, BUFFER_SIZE);
            mem->listo[proc_num] = 1;

            V(semid, 0);

            if (strcmp(buffer, EXIT_COMMAND) == 0) {
                printf("SALIENDO DEL CHAT\n");
                break;
            }
        }
        wait(NULL);
    }

    shmdt(mem);
    if (pid != 0) {
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
    }

    return 0;
}
