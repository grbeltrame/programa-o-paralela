#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>
#define TAMANHO 500000

int primo (int n) {
int i;
	for (i = 3; i < (int)(sqrt(n) + 1); i+=2) {
			if(n%i == 0) return 0;
	}
	return 1;
}

int main(int argc, char *argv[]) {
double t_inicial, t_final;
int cont = 0, total = 0;
int i, n;
int meu_ranque, num_procs, inicio, dest, raiz=0, tag=1, stop=0;
MPI_Status estado;
MPI_Request request;

	if (argc < 2) {
        printf("Entre com o valor do maior inteiro como parâmetro para o programa.\n");
       	 return 0;
    } else {
        n = strtol(argv[1], (char **) NULL, 10);
    }

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (num_procs < 2) {
        printf("Este programa deve ser executado com no mínimo dois processos.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
       	return(1);
    }

    t_inicial = MPI_Wtime();

    if (meu_ranque == 0) { 
        for (dest=1, inicio=3; dest < num_procs && inicio < n; dest++, inicio += TAMANHO) {

            // MESTRE → ESCRAVO: envio inicial de tarefas
            // Correspondente ao MPI_Irecv no escravo
            MPI_Isend(&inicio, 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
        }

        while (dest < num_procs) {

            // MESTRE → ESCRAVO: envio de parada (tag=99)
            // Correspondente ao MPI_Irecv no escravo
            MPI_Isend(&inicio, 1, MPI_INT, dest, 99, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUS_IGNORE);

            stop++;
            dest++;
        }

        while (stop < (num_procs-1)) {

            // MESTRE ← ESCRAVO: recebe resultado parcial
            // Correspondente ao MPI_Isend do escravo
		    MPI_Irecv(&cont, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &estado);

            total += cont;
            dest = estado.MPI_SOURCE;

            if (inicio > n) {
                tag = 99;
                stop++;
            }

            // MESTRE → ESCRAVO: envio dinâmico (bag-of-tasks)
            // Correspondente ao MPI_Irecv no escravo
            MPI_Isend(&inicio, 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUS_IGNORE);

            inicio += TAMANHO;
        }
    }       
    else { 
        while (estado.MPI_TAG != 99) {

            // ESCRAVO ← MESTRE: recebe tarefa
            // Correspondente ao MPI_Isend do mestre
            MPI_Irecv(&inicio, 1, MPI_INT, raiz, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &estado);

            if (estado.MPI_TAG != 99) {
                for (i = inicio, cont=0; i < (inicio + TAMANHO) && i < n; i+=2) 
		            if (primo(i) == 1)
                        cont++;

                // ESCRAVO → MESTRE: envia resultado parcial
                // Correspondente ao MPI_Irecv do mestre
                MPI_Isend(&cont, 1, MPI_INT, raiz, tag, MPI_COMM_WORLD, &request);
                MPI_Wait(&request, MPI_STATUS_IGNORE);
            } 
        } 

        t_final = MPI_Wtime();
    }

	if (meu_ranque == 0) {
		t_final = MPI_Wtime();
        total += 1;
		printf("Quant. de primos entre 1 e %d: %d \n", n, total);
		printf("Tempo de execucao: %1.3f \n", t_final - t_inicial);  	 
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return(0);
}