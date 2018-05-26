#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <signal.h>

struct shmem {
	int vetor[10]; // {[0...9] = dados f1, [10] = tam f1, [11] = ctl f1, [12...18] = pid processos}
	int pids[7];
	int tam, ctl;
	sem_t mutex;
};

int enfileira_f1();
int desenfileira_f1();
void consome();

struct shmem *shared_memory; 

//retorna 1 se sucesso, -1 se a fila estiver cheia
int enfileira_f1() {
	sem_wait(&shared_memory->mutex);
	int valor = rand() % 1000 + 1;
	int i; //shared_memory->ctl = ctl: 0 esvaziando, 1 enchendo
	if(shared_memory->tam < 10 && shared_memory->ctl) { //contorle de tamanho, tam = tamanho fila
		printf("Processo de pid %d enfileirando em f1\n", getpid());
		shared_memory->vetor[shared_memory->tam] = valor;
		shared_memory->tam++; //tam = tam
		printf("Vetor em %d: {", getpid());
		for(i = 0; i < shared_memory->tam; i++) {
			printf("[%d] = %d",i , shared_memory->vetor[i]);
			if(i != shared_memory->tam - 1) printf(", ");
		}
		printf("}\n");
		sem_post(&shared_memory->mutex);
		return 1;
	} else if ((shared_memory->tam == 10 && shared_memory->ctl) || (shared_memory->tam > 0 && !shared_memory->ctl)) { //se a fila estiver cheia e ainda não foi definida como esvaziando
		printf("Solicitando a fila esvaziar: %d\n", shared_memory->tam);
		shared_memory->ctl = 0; //define a fila como esvaziando
		kill(shared_memory->vetor[15], SIGUSR1);
		sem_post(&shared_memory->mutex);
		return -1;
	} else if (shared_memory->tam == 0 && shared_memory->ctl){ // se a fila estiver vazia e ainda não foi definida como enchendo
		shared_memory->ctl = 1;
		sem_post(&shared_memory->mutex);
		return 1;
	} else {
		printf("Fila cheia\n");
		sem_post(&shared_memory->mutex);
		return -1;
	}
}

//retorna 1 se suscesso, -1 se a fila estiver vazia
int desenfileira_f1() {
	sem_wait(&shared_memory->mutex);
	int i, valor;
	printf("Processo de pid %d desenfileirando em f1, tamanho da fila: %d\n", getpid(), shared_memory->tam);
	valor = shared_memory->vetor[0];
	printf("Removendo o valor %d da fila\n", valor);
	printf("Valor da posicao 4 da fila: %d\n", shared_memory->vetor[3]);
	printf("Vetor em %d: {", getpid());
	for(i = 0; i < shared_memory->tam - 1; i++) {
		shared_memory->vetor[i] = shared_memory->vetor[i+1];
	}
	shared_memory->tam --;
	for(i = 0; i < shared_memory->tam; i++) {
		printf("[%d] = %d",i , shared_memory->vetor[i]);
		if(i = shared_memory->tam - 1) printf(", ");
	}
	printf("}\n");
	sem_post(&shared_memory->mutex);
	return valor;
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
	int i, segment_id, segment_size = sizeof(struct shmem*);

	segment_id = shmget(IPC_PRIVATE, segment_size, IPC_CREAT | S_IRUSR | S_IWUSR); //
	shared_memory = (struct shmem*) shmat(segment_id, 0, 0);
	shmctl(segment_id, IPC_STAT, NULL);
	shared_memory->tam = 0; //tam f1
	shared_memory->ctl = 1; //define a fila como enchendo na inicialização

	//semáforo
	sem_init(&shared_memory->mutex, 1, 1); //endereço do semaforo, 0- compartilhado entre threads e 1 - compartilhado entre processos, valor inicial do semáforo

	srand(time(NULL));
	int random;

	for(i = 0; i < 7; i++) {
		shared_memory->pids[i] = -1;
	}

	shared_memory->pids[0] = fork();
	if(shared_memory->pids[0] > 0) shared_memory->pids[1] = fork();
	if(shared_memory->pids[0] > 0 && shared_memory->pids[1] > 0) shared_memory->pids[2] = fork();
	if(shared_memory->pids[0] > 0 && shared_memory->pids[1] > 0 && shared_memory->pids[2] > 0) shared_memory->pids[3] = fork();
	if(shared_memory->pids[0] > 0 && shared_memory->pids[1] > 0 && shared_memory->pids[2] > 0 && shared_memory->pids[3] > 0) shared_memory->pids[4] = fork();
	if(shared_memory->pids[0] > 0 && shared_memory->pids[1] > 0 && shared_memory->pids[2] > 0 && shared_memory->pids[3] > 0 && shared_memory->pids[4] > 0) shared_memory->pids[5] = fork();
	if(shared_memory->pids[0] > 0 && shared_memory->pids[1] > 0 && shared_memory->pids[2] > 0 && shared_memory->pids[3] > 0 && shared_memory->pids[4] > 0 && shared_memory->pids[5]>0) shared_memory->pids[6] = fork();

	if(shared_memory->pids[0] == 0) { //p1
		for(;;) {
			enfileira_f1();
			sleep(1);
		}
	}
	if(shared_memory->pids[1] == 0) { //p2
 		for(;;) {
			enfileira_f1();
			sleep(1);
		}
 	} else if(shared_memory->pids[2] == 0) { //p3
 		for(;;) {
			enfileira_f1();
			sleep(1);
		}
	} else if(shared_memory->pids[3] == 0) { //p4
		for(;;) {
			signal(SIGUSR1, consome);
		}
	} else if(shared_memory->pids[4] == 0) { //p5

	} else if(shared_memory->pids[5] == 0) { //p6

	} else if(shared_memory->pids[6] == 0) { //p7

	}
	shmdt(shared_memory);
	if(shared_memory->pids[0] > 0 && shared_memory->pids[1] > 0 && shared_memory->pids[2] > 0 && shared_memory->pids[3] > 0 && shared_memory->pids[4] > 0 && shared_memory->pids[5] > 0 && shared_memory->pids[6] > 0) { //pai
		for(i = 0; i < 7; i++) {
			wait(&status);
		}
		sem_destroy(&shared_memory->mutex);
		shmctl(segment_id, IPC_RMID, 0);
	}
	return 0;
} 