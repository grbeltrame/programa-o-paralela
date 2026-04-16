#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int primo(long int n) {
    for (long int i = 3; i < (long int)(sqrt(n) + 1); i += 2)
        if (n % i == 0) return 0;
    return 1;
}

int main(int argc, char *argv[]) { /* sequencial.c */
double t_inicial, t_final;
int cont = 1; /* já conta o 2 */
long int i, n;
struct timespec t0, t1;

    if (argc < 2) {
        printf("Entre com o valor do maior inteiro como parâmetro.\n");
        return 0;
    }
    n = strtol(argv[1], (char **) NULL, 10);

    clock_gettime(CLOCK_MONOTONIC, &t0);

    for (i = 3; i <= n; i += 2)
        if (primo(i)) cont++;

    clock_gettime(CLOCK_MONOTONIC, &t1);
    t_inicial = t0.tv_sec + t0.tv_nsec / 1e9;
    t_final   = t1.tv_sec + t1.tv_nsec / 1e9;

    printf("Quant. de primos entre 1 e %ld: %d\n", n, cont);
    printf("Tempo de execucao: %1.3f\n", t_final - t_inicial);

    return 0;
}