#include <stdio.h>
#include <time.h>
#include "../src/matrix_multiplication.h"
#include "../src/tiling_multiplication.h"
#include "../src/readmatrix.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <test_file.txt>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("Error: could not open file %s\n", argv[1]);
        return 1;
    }

    int m, k, n;
    if (fscanf(fp, "%d %d %d", &m, &k, &n) != 3) {
    printf("ERROR: expected 'M K N' on the first line of the input file.\n");
    fclose(fp);
    return 1;
}

    int a[m][k];
    int b[k][n];
    int c_simple[m][n];
    int c_blocked[m][n];

    rm(fp, m, k, a);
    rm(fp, k, n, b);
    fclose(fp);

    struct timespec start1, end1;
    clock_gettime(CLOCK_MONOTONIC, &start1);
    mulMatrix(m, k, n, a, b, c_simple);
    clock_gettime(CLOCK_MONOTONIC, &end1);
    double simple_ms = (end1.tv_sec - start1.tv_sec) * 1000.0
                      + (end1.tv_nsec - start1.tv_nsec) / 1e6;

    int blockSize = determineOptimalBlockSize(m, k, n);

    struct timespec start2, end2;
    clock_gettime(CLOCK_MONOTONIC, &start2);
    mulMatrixBlocked(m, k, n, a, b, c_blocked, blockSize);
    clock_gettime(CLOCK_MONOTONIC, &end2);
    double blocked_ms = (end2.tv_sec - start2.tv_sec) * 1000.0
                       + (end2.tv_nsec - start2.tv_nsec) / 1e6;

    printf("A:\n");
    printMatrix(m, k, a);
    printf("B:\n");
    printMatrix(k, n, b);

    printf("\nAlgorithm: GEMM Simple\n");
    printf("Result matrix:\n");
    printMatrix(m, n, c_simple);
    printf("Execution time: %.6f ms\n", simple_ms);

    printf("\nAlgorithm: GEMM Blocking\n");
    printf("Block size used: %d\n", blockSize);
    printf("Result matrix:\n");
    printBlockedMatrix(m, n, c_blocked);
    printf("Execution time: %.6f ms\n", blocked_ms);

    return 0;
}