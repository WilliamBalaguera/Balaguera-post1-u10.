#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#define REPEAT 10


double bench_seq(size_t n_bytes) {
    volatile char *arr = (volatile char *)malloc(n_bytes);
    if (!arr) return -1.0;
    memset((void*)arr, 1, n_bytes);   /* warm-up + evitar paginas zero */

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int r = 0; r < REPEAT; r++)
        for (size_t i = 0; i < n_bytes; i++)
            (void)arr[i];             /* lectura volatile: el compilador no la elimina */
    clock_gettime(CLOCK_MONOTONIC, &t1);

    free((void*)arr);
    double ns = (t1.tv_sec - t0.tv_sec)*1e9
              + (t1.tv_nsec - t0.tv_nsec);
    return ns / (REPEAT * (double)n_bytes);  /* ns/byte */
}


double bench_rand(size_t n_bytes) {
    size_t n = n_bytes / sizeof(int);
    int    *arr = (int    *)malloc(n * sizeof(int));
    size_t *idx = (size_t *)malloc(n * sizeof(size_t));
    if (!arr || !idx) { free(arr); free(idx); return -1.0; }

    for (size_t i = 0; i < n; i++) { arr[i] = (int)i; idx[i] = i; }

    /* Mezcla Fisher-Yates para permutacion uniformemente aleatoria */
    srand(42);   /* semilla fija para reproducibilidad */
    for (size_t i = n-1; i > 0; i--) {
        size_t j = rand() % (i+1);
        size_t tmp = idx[i]; idx[i] = idx[j]; idx[j] = tmp;
    }

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    volatile long sum = 0;
    for (int r = 0; r < REPEAT; r++)
        for (size_t i = 0; i < n; i++)
            sum += arr[idx[i]];       /* acceso indirecto: patron totalmente aleatorio */
    clock_gettime(CLOCK_MONOTONIC, &t1);

    free(arr); free(idx);
    double ns = (t1.tv_sec-t0.tv_sec)*1e9+(t1.tv_nsec-t0.tv_nsec);
    return ns / (REPEAT * (double)n);  /* ns/elemento */
}


int main(void) {
    size_t sizes[] = {
        4*1024,         /*  4 KB  — region L1 */
        8*1024,         /*  8 KB */
        16*1024,        /* 16 KB */
        32*1024,        /* 32 KB  — limite tipico L1d */
        64*1024,        /* 64 KB  — region L2 */
        128*1024,       /*128 KB */
        256*1024,       /*256 KB  — limite tipico L2 */
        512*1024,       /*512 KB  — region L3 */
        1024*1024,      /*  1 MB */
        4*1024*1024,    /*  4 MB  — posible limite L3 */
        16*1024*1024,   /* 16 MB  — region RAM */
        64*1024*1024    /* 64 MB */
    };
    int n = sizeof(sizes)/sizeof(sizes[0]);

    printf("%-12s %14s %14s\n", "Tamano(KB)", "seq(ns/byte)", "rand(ns/elem)");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        double s = bench_seq(sizes[i]);
        double r = bench_rand(sizes[i]);
        printf("%-12zu %14.3f %14.3f\n", sizes[i]/1024, s, r);
        fflush(stdout);
    }
    return 0;
}
