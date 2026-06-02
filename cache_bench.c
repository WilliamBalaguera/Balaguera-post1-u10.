#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* REPEAT reducido a 10 para que el benchmark termine en segundos.
   Con REPEAT=100 y arrays de 64 MB el tiempo total supera los 10 minutos. */
#define REPEAT 10

/* ---------------------------------------------------------------
   bench_seq: mide la latencia media de acceso SECUENCIAL a un
   array de n_bytes bytes. El recorrido lineal explota la localidad
   espacial y el prefetcher del hardware, por lo que arrays que
   caben en L1/L2/L3 muestran latencias mucho menores que los que
   desbordan a RAM.
   Retorna: nanosegundos por byte accedido.
--------------------------------------------------------------- */
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

/* ---------------------------------------------------------------
   bench_rand: mide la latencia media de acceso ALEATORIO usando
   un indice pre-mezclado con Fisher-Yates. Destruye la localidad
   espacial: cada acceso es casi seguro un cache miss cuando el
   array supera el ultimo nivel de cache. Para arrays > ~1-2 MB
   ademas se producen TLB misses que suman penalizacion adicional.
   Retorna: nanosegundos por elemento accedido.
--------------------------------------------------------------- */
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

/* ---------------------------------------------------------------
   main: ejecuta ambos benchmarks sobre 12 tamanos que cubren las
   regiones L1 -> L2 -> L3 -> RAM del sistema.
--------------------------------------------------------------- */
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
