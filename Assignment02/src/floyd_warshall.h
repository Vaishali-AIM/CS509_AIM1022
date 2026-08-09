#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

#include <stdbool.h>

/*
 * Dense V x V distance matrix, used as both input and output */
 
typedef struct {
    int vertices;
    long long **dist;   /* V x V matrix; dist[i][j] = weight of edge i->j */
                         
} DenseGraph;
void floyd_warshall(DenseGraph *g, bool *has_negative_cycle);

#endif