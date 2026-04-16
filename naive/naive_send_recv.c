#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

int primo(long int n) {
    for (long int i = 3; i < (long int)(sqrt(n) + 1); i += 2)
        if (n % i == 0) return 0;
    return 1;
}

int main(int argc, char *argv[]) { /* naive_send_recv.c */
double t_inicial, t_final;
int cont = 0, total = 0;
long int i, n;
int meu_ranque, num_procs, salto, origem;
int inicio;
MPI_Status estado;

    if (argc < 2) {
        printf("Entre com o valor do maior inteiro como parâmetro.\n");
        return 0;
    } else {
        n = strtol(argv[1], (char **) NULL, 10);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    t_inicial = MPI_Wtime();

    /* Cada processo pega números intercalados */
    inicio = 3 + meu_ranque * 2;
    salto  = num_procs * 2;
    for (i = inicio; i <= n; i += salto)
        if (primo(i) == 1) cont++;

    /* Processo 0 coleta os resultados parciais via MPI_Recv */
    if (meu_ranque == 0) {
        total = cont;
        for (origem = 1; origem < num_procs; origem++) {
            MPI_Recv(&cont, 1, MPI_INT, origem, 0, MPI_COMM_WORLD, &estado);
            total += cont;
        }
        total += 1; /* acrescenta o 2, que também é primo */
    } else {
        /* Demais processos enviam seu parcial para o processo 0 */
        MPI_Send(&cont, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    t_final = MPI_Wtime();

    if (meu_ranque == 0) {
        printf("Quant. de primos entre 1 e %ld: %d\n", n, total);
        printf("Tempo de execucao: %1.3f\n", t_final - t_inicial);
    }

    MPI_Finalize();
    return 0;
}