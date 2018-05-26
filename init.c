#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>

struct shmem {
	int vetor[18]; // {[0...9] = dados f1, [10] = tam f1, [11] = ctl f1, [12...18] = pid processos}
	sem_t mutex;
};

int enfileira_f1();
int desenfileira_f1();
void consome();

struct shmem *shared_memory; 

//retorna 1 se sucesso, -1 se a fila estiver cheia, 0 se a fila estiver esvaziando
int enfileira_f1() {
	sem_wait(&shared_memory->mutex);
	int valor = rand() % 1000 + 1;
	int i; //shared_memory->vetor[11] = ctl: 0 esvaziando, 1 enchendo
	if(shared_memory->vetor[10] < 10 && shared_memory->vetor[11]) { //contorle de tamanho, vetor[10] = tamanho fila
		printf("Processo de pid %d enfileirando em f1\n", getpid());
		shared_memory->vetor[shared_memory->vetor[10]] = valor;
		shared_memory->vetor[10]++; //vetor[10] = tam
		printf("Vetor em %d: {", getpid());
		for(i = 0; i < shared_memory->vetor[10]; i++) {
			printf("[%d] = %d",i , shared_memory->vetor[i]);
			if(i != shared_memory->vetor[10] - 1) printf(", ");
		}
		printf("}\n");
		return 1;
	} else if (shared_memory->vetor[10] == 10 && shared_memory->vetor[11]) { //se a fila estiver cheia e ainda não foi definida como esvaziando
		printf("Fila esvaziando, tamanho atual: %d\n", shared_memory->vetor[10]);
		shared_memory->vetor[11] = 0; //define a fila como esvaziando
		return -1;
	} else if (shared_memory->vetor[10] == 0 && shared_memory->vetor[11]){ // se a fila estiver vazia e ainda não foi definida como enchendo
		shared_memory->vetor[11] = 1;
		return 1;
	} else if(shared_memory->vetor[10] > 0 && !shared_memory->vetor[11]) {
		printf("Fila esvaziando, tamanho atual: %d\n", shared_memory->vetor[10]);
		return 0;
	} else {
		printf("Fila cheia\n");
		return -1;
	}
}

//retorna 1 se suscesso, -1 se a fila estiver vazia
int desenfileira_f1() {
	int i, valor;
	if(shared_memory->vetor[10] > 0 && !shared_memory->vetor[11]) {
		printf("Processo de pid %d desenfileirando em f1\n", getpid());
		valor = shared_memory->vetor[0];
		printf("Vetor: {");
		for(i = 0; i < shared_memory->vetor[10]; i++) {
			printf("%d", shared_memory->vetor[i]);
			if(i = shared_memory->vetor[10] - 1) printf(", ");
		}
		printf("}\n");
		for(i = 0; i < shared_memory->vetor[10] - 1; i++) {
			shared_memory->vetor[i] = shared_memory->vetor[i+1];
		}
		shared_memory->vetor[10] --;
		return valor;
	} 
	return -1;
}

void consome() {
	int i;
	sem_wait(&shared_memory->mutex);
	printf("Consome\n");
	for (i = 0; i < 10; i++) {
		desenfileira_f1();
	}
	sem_post(&shared_memory->mutex);
}

int main() {

	//memória compartilhada
	
	int status;
	int i, segment_id, segment_size = 12 * sizeof(int);

	segment_id = shmget(IPC_PRIVATE, segment_size, IPC_CREAT | S_IRUSR | S_IWUSR); //
	shared_memory = (struct shmem*) shmat(segment_id, 0, 0);
	shmctl(segment_id, IPC_STAT, NULL);
	shared_memory->vetor[10] = 0; //tam f1
	shared_memory->vetor[11] = 1; //define a fila como enchendo na inicialização

	//semáforo
	sem_init(&shared_memory->mutex, 1, 1); //endereço do semaforo, 0- compartilhado entre threads e 1 - compartilhado entre processos, valor inicial do semáforo

	srand(time(NULL));
	int random;

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
		shared_memory->vetor[12] = p1;
		shared_memory->vetor[13] = p2;
		shared_memory->vetor[14] = p3;
		shared_memory->vetor[15] = p4;
		shared_memory->vetor[16] = p5;
		shared_memory->vetor[17] = p6;
		shared_memory->vetor[18] = p7;
	} 

	if(p1 == 0) { //p1
		for(;;) {
			sleep(1);
			if(enfileira_f1() == -1) {
				kill(shared_memory->vetor[16], SIGUSR1);
			}
			sem_post(&shared_memory->mutex);
		}
	}
	if(p2 == 0) { //p2
 		for(;;) {
			sleep(1);
			if(enfileira_f1() == -1) {
				kill(shared_memory->vetor[16], SIGUSR1);
			}
			sem_post(&shared_memory->mutex);
		}
 	} else if(p3 == 0) { //p3
 		for(;;) {
			sleep(1);
			if(enfileira_f1() == -1) {
				kill(shared_memory->vetor[16], SIGUSR1);
			}
			sem_post(&shared_memory->mutex);
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
		for(i = 0; i < 7; i++) {
			wait(&status);
		}
		shmctl(segment_id, IPC_RMID, 0);
	}
	return 0;
} 