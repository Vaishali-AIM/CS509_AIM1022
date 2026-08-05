#include <stdio.h>
#include "matrix_multiplication.h"
#include "readmatrix.h"

void mulMatrix(int m, int k, int n, int a[m][k], int b[k][n], int c[m][n])
{
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            c[i][j] = 0;

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            for (int d = 0; d < k; d++)
                c[i][j] += a[i][d] * b[d][j];
}

void printMatrix(int r, int col, int s[r][col])
{
    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < col; j++)
        {
            printf("%d\t", s[i][j]);
        }
        printf("\n");
    }
}
