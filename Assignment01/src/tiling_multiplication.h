#ifndef TILING_MULTIPLICATION_H
#define TILING_MULTIPLICATION_H

/* Computes a block size sized to fit 3 tiles (A, B, C) */
int determineOptimalBlockSize(int m, int k, int n);

/* Tiled matrix multiplication*/
void mulMatrixBlocked(int m, int k, int n, int A[m][k], int B[k][n], int C[m][n], int blockSize);

void printBlockedMatrix(int r, int col, int s[r][col]);

#endif