#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

void comer(int id);
void pensar(int id);
void filosofo(int *id);

int garfos[5] = {0}, filosofos[5];
sem_t mutex;

int main() {
	int i, id;
	srand(time(NULL));
	pthread_t threads[5];
	sem_init(&mutex, 0, 1); //variável do semáforo, 0 para semáforo entre threads e 1 para semáforo entre processos, valor inicial
	for(i = 0; i < 5; i++) {
		filosofos[i] = i;
		pthread_create(&threads[i], NULL, (void*) filosofo, &filosofos[i]);
	}
	for(i = 0; i < 5; i++) {
		pthread_join(threads[i], NULL);
	}
	return 0;
}

void comer(int id) {
	int n = rand() % 3 + 1;
	switch(id) {
		case 0:
			if(!garfos[0] && !garfos[4]) {
				sem_wait(&mutex);
				garfos[0] = 1;
				garfos[4] = 1;
				printf("Filosofo %d comendo por % d segundos\n", id, n);
				//sleep(n);
				garfos[0] = 0;
				garfos[4] = 0;
				sem_post(&mutex);
			} else {
				if(garfos[0])
					printf("Garfo direito do filosofo %d ocupado\n", id);
				else if(garfos[4]) {
					printf("Garfo esquerdo do filosofo %d ocupado\n", id);
				}
				//sleep(1);
				comer(id);
			}
			break;
		case 1:
			if(!garfos[1] && !garfos[0]) {
				sem_wait(&mutex);
				garfos[1] = 1;
				garfos[0] = 1;
				printf("Filosofo %d comendo por % d segundos\n", id, n);
				//sleep(n);
				garfos[1] = 0;
				garfos[0] = 0;
				sem_post(&mutex);
			} else {
				if(garfos[1])
					printf("Garfo direito do filosofo %d ocupado\n", id);
				else if(garfos[0]) {
					printf("Garfo esquerdo do filosofo %d ocupado\n", id);
				}
				//sleep(1);
				comer(id);
			}
			break;
		case 2:
			if(!garfos[2] && !garfos[1]) {
				sem_wait(&mutex);
				garfos[2] = 1;
				garfos[1] = 1;
				printf("Filosofo %d comendo por % d segundos\n", id, n);
				//sleep(n);
				garfos[2] = 0;
				garfos[1] = 0;
				sem_post(&mutex);
			} else {
				if(garfos[2])
					printf("Garfo direito do filosofo %d ocupado\n", id);
				else if(garfos[1]) {
					printf("Garfo esquerdo do filosofo %d ocupado\n", id);
				}
				//sleep(1);
				comer(id);
			}
			break;
		case 3:
			if(!garfos[3] && !garfos[2]) {
				sem_wait(&mutex);
				garfos[3] = 1;
				garfos[2] = 1;
				printf("Filosofo %d comendo por % d segundos\n", id, n);
				//sleep(n);
				garfos[3] = 0;
				garfos[2] = 0;
				sem_post(&mutex);
			} else {
				if(garfos[3])
					printf("Garfo direito do filosofo %d ocupado\n", id);
				else if(garfos[2]) {
					printf("Garfo esquerdo do filosofo %d ocupado\n", id);
				}
				//sleep(1);
				comer(id);
			}
			break;
		case 4:
			if(!garfos[4] && !garfos[3]) {
				sem_wait(&mutex);
				garfos[4] = 1;
				garfos[3] = 1;
				printf("Filosofo %d comendo por % d segundos\n", id, n);
				//sleep(n);
				garfos[4] = 0;
				garfos[3] = 0;
				sem_post(&mutex);
			} else {
				if(garfos[4])
					printf("Garfo direito do filosofo %d ocupado\n", id);
				else if(garfos[3]) {
					printf("Garfo esquerdo do filosofo %d ocupado\n", id);
				}
				//sleep(1);
				comer(id);
			}
			break;
	}
}

void pensar(int id) {
	int n = rand() % 3 + 1;
	printf("Filosofo %d pensando por %d segundos\n", filosofos[id], n);
	//sleep(n);

}

void filosofo(int *id) {
	for(;;) {
		if(rand() % 2) {
			pensar(*id);
		} else {
			comer(*id);
		}
	}
}