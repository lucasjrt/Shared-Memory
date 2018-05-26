#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

struct shmem {
	int vetor[10]; //f1
	int pids[7]; //vetor de pids
	int tam, ctl; //tamanho de f1, controle da f1
	sem_t mutex; //semáforo
};

int enfileira_f1();
int desenfileira_f1();
void consome();
void thread_init();

struct shmem *shared_memory; 

//retorna 1 se sucesso, -1 se a fila estiver cheia
int enfileira_f1() {
	if(shared_memory->ctl) {
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
			printf("Processo %d solicitando a fila esvaziar: %d\n", getpid(), shared_memory->tam);
			shared_memory->ctl = 0; //define a fila como esvaziando
			sem_post(&shared_memory->mutex);
			kill(shared_memory->pids[3], SIGUSR1);
			return 0;
		} else if (shared_memory->tam == 0 && shared_memory->ctl){ // se a fila estiver vazia e ainda não foi definida como enchendo
			shared_memory->ctl = 1;
			sem_post(&shared_memory->mutex);
			return 1;
		}
	}  else {
		return -1;
	}
}

//retorna 1 se suscesso, -1 se a fila estiver vazia
int desenfileira_f1() {
	int i, valor;
	if(shared_memory->tam > 0) {
		printf("Processo de pid %d desenfileirando em f1, tamanho da fila: %d\n", getpid(), shared_memory->tam);
		valor = shared_memory->vetor[0];
		printf("Removendo o valor %d da fila\n", valor);
		shared_memory->tam --;
		for(i = 0; i < shared_memory->tam; i++) {
			shared_memory->vetor[i] = shared_memory->vetor[i+1];
		}

		printf("Vetor em %d: {", getpid());
		for(i = 0; i < shared_memory->tam; i++) {
			printf("[%d] = %d",i , shared_memory->vetor[i]);
			if(i == shared_memory->tam - 2) printf(", ");
		}
		printf("}\n");
		return valor;
	}
	return -1;
}

void consome() {
	int i;
	for (i = 0; i < 5; i++) {
		sem_wait(&shared_memory->mutex);
		desenfileira_f1();
		sem_post(&shared_memory->mutex);
		sleep(1);
	}
	shared_memory->ctl = 1;
}

void thread_init() {
	int i;
	pthread_t threads[2];
	for (i = 0; i < 2; i++) {
		pthread_create(&threads[i], NULL, (void*) consome, NULL);
	}
	for(i = 0; i < 2; i++) {
		pthread_join(threads[i], NULL);
	}
}

int main() {
	int status;
	int i, segment_id, segment_size = sizeof(struct shmem*);
	pid_t pai = getpid();

	//iniciando memória compartilhada
	segment_id = shmget(IPC_PRIVATE, segment_size, IPC_CREAT | 0666); //
	shared_memory = (struct shmem*) shmat(segment_id, 0, 0);
	shmctl(segment_id, IPC_STAT, NULL);
	shared_memory->tam = 0; //tam f1
	shared_memory->ctl = 1; //define a fila como enchendo na inicialização

	//semáforo
	sem_init(&shared_memory->mutex, 1, 1); //endereço do semaforo, 0- compartilhado entre threads e 1 - compartilhado entre processos, valor inicial do semáforo

	int random;

	pid_t pids[7] = {-1};

	for(i = 0; i < 7; i++) {
		if(getpid() == pai) {
			pids[i] = fork();
		} else
			break;
	}

	if(getpid() == pai) {
		for(i = 0; i < 7; i++) {
			shared_memory->pids[i] = pids[i];
		}
		printf("Vetor de pids: {");
		for(i = 0; i < 7; i++) {
			printf("%d", shared_memory->pids[i]);
			if (i != 6) printf(", ");
		}
		printf("}\n");
	} else if(pids[0] == 0) { //p1
		srand(1234);
		for(;;) {
			enfileira_f1();
			sleep(1);
		}
	} else if(pids[1] == 0) { //p2
		srand(4321);
 		for(;;) {
			enfileira_f1();
			sleep(1);
		}
 	} else if(pids[2] == 0) { //p3
 		srand(1423);
 		for(;;) {
			enfileira_f1();
			sleep(1);
		}
	} else if(pids[3] == 0) { //p4
		for(;;) {
			signal(SIGUSR1, thread_init);
		}
	} else if(pids[4] == 0) { //p5

	} else if(pids[5] == 0) { //p6

	} else if(pids[6] == 0) { //p7

	}
	shmdt(shared_memory);
	if(getpid() == pai) { //pai
		for(i = 0; i < 7; i++) {
			wait(&status);
		}
		sem_destroy(&shared_memory->mutex);
		shmctl(segment_id, IPC_RMID, 0);
	}
	return 0;
} 