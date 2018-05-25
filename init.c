#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>

struct shmem {
	int vetor[18]; // {[0...9] = dados , [10] = tam, [11] = ctl, [12...18] = pid processos}
	sem_t mutex;
};

struct shmem *shared_memory; 
sem_t mutex;

//retorna 1 se sucesso, -1 se a fila estiver cheia
int enfileira(int *shmem, int valor) {
	int i; //shared_memory->vetor[11] = ctl: 0 esvaziando, 1 enchendo
	if(shmem[10] < 10 )	{ //contorle de tamanho, shmem[10] = tamanho lista
		ctl = 1; //fila enchendo
		shmem[shmem[10]] = valor;
		shmem[10]++; //shmem[10] = tam
		printf("Vetor: {");
		for(i = 0; i < shared_memory[10]; i++) {
			printf("%d", shared_memory[i]);
			if(i != shared_memory[10] - 1) printf(", ");
		}
		printf("}\n");
		return 1;
	} else 
	return -1;
}

//retorna 1 se suscesso, -1 se a fila estiver vazia
int desenfileira(int *shmem) {
	int i, valor;
	if(shmem[10] > 0) {
		valor = shmem[0];
		printf("Vetor: {");
		for(i = 0; i < shared_memory[10]; i++) {
			printf("%d", shared_memory[i]);
			if(i = shared_memory[10] - 1) printf(", ");
		}
		printf("}\n");
		for(i = 0; i < shmem[10] - 1; i++) {
			shmem[i] = shmem[i+1];
		}
		shmem[10] --;
		return 1;
	} 
	return -1;
}

void consome() {
	int i;
	sem_wait(&shared_memory->mutex);
	printf("Consome\n");
	for (i = 0; i < 10; i++) {
		desenfileira(shared_memory->vetor);
	}
	sem_post(&shared_memory->mutex);
}

int main() {

	//memória compartilhada
	
	int status;
	int i, segment_id, segment_size = 12 * sizeof(int);
	struct shmid_ds shmbuffer;

	segment_id = shmget(IPC_PRIVATE, segment_size, IPC_CREAT | IPC_EXCL| S_IRUSR | S_IWUSR);
	shared_memory = (int*) shmat(segment_id, 0, 0);
	shmctl(segment_id, IPC_STAT, &shmbuffer);
	segment_size = shmbuffer.shm_segsz;
	shared_memory[10] = 0;

	//semáforo
	int semid;

	sem_init(&mutex, 1, 1); //endereço do semaforo, compartilhado entre threads ou processos, valor inicial

	int seed = 1337;
	srand(seed);
	int random;
	int rand_r(unsigned int*seedp);

	pid_t p1 = -1;
	pid_t p2 = -1;
	pid_t p3 = -1;
	pid_t p4 = -1;
	pid_t p5 = -1;
	pid_t p6 = -1;
	pid_t p7 = -1;


	p1 = fork();
	if(p1 > 0) p2 = fork();
	if(p1 > 0 && p2 > 0) p3 = fork();
	if(p1 > 0 && p2 > 0 && p3 > 0) p4 = fork();
	if(p1 > 0 && p2 > 0 && p3 > 0 && p4 > 0) p5 = fork();
	if(p1 > 0 && p2 > 0 && p3 > 0 && p4 > 0 && p5 > 0) p6 = fork();
	if(p1 > 0 && p2 > 0 && p3 > 0 && p4 > 0 && p5 > 0 && p6>0) p7 = fork();

	if(p1 > 0 && p2 > 0 && p3 > 0 && p4 > 0 && p5 > 0 && p6>0 && p7 > 0) { //pai
		shared_memory[12] = p1;
		shared_memory[13] = p2;
		shared_memory[14] = p3;
		shared_memory[15] = p4;
		shared_memory[16] = p5;
		shared_memory[17] = p6;
		shared_memory[18] = p7;
	} else if(p1 == 0) { //p1
		for(;;) {
			sem_wait(&mutex);
			random = (rand() % 1000) + 1;
			if(enfileira(shared_memory, random) == -1) {
				printf("Processo 1\n");
				kill(shared_memory[16], SIGUSR1);
			}
			sem_post(&mutex);
		}
	} else if(p2 == 0) { //p2
 		for(;;) {
 			sem_wait(&mutex);
			random = (rand() % 1000) + 1;
			if(enfileira(shared_memory, random) == -1) {
				printf("Processo 2\n");
				kill(shared_memory[16], SIGUSR1);
			}
			sem_post(&mutex);
		}
 	} else if(p3 == 0) { //p3
 		for(;;) {
 			sem_wait(&mutex);
			random = (rand() % 1000) + 1;
			if(enfileira(shared_memory, random) == -1) {
				printf("Processo 3\n");
				kill(shared_memory[16], SIGUSR1);
			}
			sem_post(&mutex);
		}
	} else if(p4 == 0) { //p4
		for(;;) {
			signal(SIGUSR1, consome);
		}
	} else if(p5 == 0) { //p5

	} else if(p6 == 0) { //p6

	} else if(p7 == 0) { //p7

	}
	shmdt(shared_memory);
	if(p1 > 0 && p2 > 0 && p3 > 0 && p4 > 0 && p5 > 0 && p6>0 && p7 > 0) { //pai
		shmctl(segment_id, IPC_RMID, 0);
	}
	return 0;
} 