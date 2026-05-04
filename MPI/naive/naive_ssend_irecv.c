#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

int primo(long int n) {
    for (long int i = 3; i < (long int)(sqrt(n) + 1); i += 2)
        if (n % i == 0) return 0;
    return 1;
}

int main(int argc, char *argv[]) {
double t_inicial, t_final;
int cont = 0, total = 0;
long int i, n;
int meu_ranque, num_procs, salto, origem;
int inicio;
MPI_Status estado;
MPI_Request request;

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

    if (meu_ranque == 0) {
        total = cont;

        for (origem = 1; origem < num_procs; origem++) {

            // MESTRE ← ESCRAVO: recebe resultado parcial (não-bloqueante)
            // Correspondente ao MPI_Ssend dos escravos
            MPI_Irecv(&cont, 1, MPI_INT, origem, 0, MPI_COMM_WORLD, &request);

            // MESTRE: aguarda conclusão da recepção
            MPI_Wait(&request, &estado);

            total += cont;
        }

        total += 1;
    } else {

        // ESCRAVO → MESTRE: envia resultado parcial (síncrono)
        // Correspondente ao MPI_Irecv do mestre
        MPI_Ssend(&cont, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    t_final = MPI_Wtime();

    if (meu_ranque == 0) {
        printf("Quant. de primos entre 1 e %ld: %d\n", n, total);
        printf("Tempo de execucao: %1.3f\n", t_final - t_inicial);
    }

    MPI_Finalize();
    return 0;
}