#ifndef READ_MATRIX_H
#define READ_MATRIX_H

#include <stdio.h>

/* Sentinel value for "infinity" in the adjacency matrix. Must be a
 * non-negative integer. */
typedef struct {
    int vertices;
    long long **dist;   /* V x V matrix; dist[i][j] = weight of edge i->j,
                          * or INF_SENTINEL if none. dist[i][i] must be 0
                          * on read (validated). */
} DenseGraph;

/* Reads and validates the matrix file, allocating and filling dist[][].
 * Exits with an error message on any malformed input (wrong row length, */
DenseGraph readAdjacencyMatrix(FILE *filename);

void freeDenseGraph(DenseGraph *g);

#endif