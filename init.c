#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>

//retorna 1 se sucesso, -1 se a fila estiver cheia
int enfileira(int *shmem, int valor) {
	if(shmem[10] < 10)	{ //contorle de tamanho, shmem[10] = tamanho lista
		shmem[11]++; //shmem[11] = indice cauda da lista
		shmem[shmem[11]] = valor;
		return 1;
	}
	return -1;
}

int desenfileira(int *shmem) {
	int i, valor;
	if(shmem[10] > 0) {
		valor = shmem[0];
		for(i = 0; i < shmem[11] - 1; i++) {
			shmem[i] = shmem[i+1];
		}
		shmem[11] --;
	} 
	return -1;
}

int main() {

	int *shared_memory; // {[0..9] = dados , [10] = tam, [11] = cabeca}
	int status;
	int i, segment_id, segment_size = 12 * sizeof(int);
	struct shmid_ds shmbuffer;

	segment_id = shmget(IPC_PRIVATE, segment_size, IPC_CREAT | IPC_EXCL| S_IRUSR | S_IWUSR);
	shared_memory = (int*) shmat(segment_id, 0, 0);
	shmctl(segment_id, IPC_STAT, &shmbuffer);
	segment_size = shmbuffer.shm_segsz;
	shared_memory[0] = 42;

	for(i = 0; i < 10; i++) {
		shared_memory[i] = 0;
	}

	printf("Vetor: {");
	for(i = 0; i < 10; i++) {
		printf("%d", shared_memory[i]);
		if(i != 10 - 1) printf(", ");
	}
	printf("}\n");

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


	if(p1 == 0) { //p1

	} else if(p2 == 0) { //p2
 		
 	} else if(p3 == 0) { //p3

	} else if(p4 == 0) { //p4

	} else if(p5 == 0) { //p5

	} else if(p6 == 0) { //p6

	} else if(p7 == 0) { //p7

	}
	
	shmdt(shared_memory);
	shmctl(segment_id, IPC_RMID, 0);
	return 0;
}