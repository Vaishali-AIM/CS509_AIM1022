#include <stdio.h>
#include "tiling_multiplication.h"
#include "readmatrix.h"

/* Computes a block size that fits 3 tiles (A, B, C) inside an assumed L1 cache budget */
#define ASSUMED_L1_CACHE_BYTES 32768

int determineOptimalBlockSize(int m, int k, int n) {
    int smallestDim = m;
    if (k < smallestDim) smallestDim = k;
    if (n < smallestDim) smallestDim = n;

    long valuesThatFit = ASSUMED_L1_CACHE_BYTES / sizeof(int);
    long budgetPerTile = valuesThatFit / 3;

    int blockSize = 1;
    while ((long)(blockSize + 1) * (blockSize + 1) <= budgetPerTile) {
        blockSize++;
    }

    if (blockSize < 1) blockSize = 1;
    if (blockSize > smallestDim) blockSize = smallestDim;

    return blockSize;
}

/* Tiled matrix multiplication, same result as mulMatrix (simple version).
 * No timing code here -- driver.c wraps this call with clock_gettime. */
void mulMatrixBlocked(int m, int k, int n, int a[m][k], int b[k][n], int c[m][n], int blockSize) {
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            c[i][j] = 0;

    for (int ii = 0; ii < m; ii += blockSize) {
        int iMax = ii + blockSize < m ? ii + blockSize : m;

        for (int jj = 0; jj < n; jj += blockSize) {
            int jMax = jj + blockSize < n ? jj + blockSize : n;

            for (int kk = 0; kk < k; kk += blockSize) {
                int kMax = kk + blockSize < k ? kk + blockSize : k;

                for (int i = ii; i < iMax; i++) {
                    for (int j = jj; j < jMax; j++) {
                        int sum = c[i][j];
                        for (int kIdx = kk; kIdx < kMax; kIdx++) {
                            sum += a[i][kIdx] * b[kIdx][j];
                        }
                        c[i][j] = sum;
                    }
                }
            }
        }
    }
}

void printBlockedMatrix(int r, int col, int s[r][col]) {
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < col; j++) {
            printf("%d\t", s[i][j]);
        }
        printf("\n");
    }
}