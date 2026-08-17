#ifndef CSR_H
#define CSR_H

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int vertices;       /* V */
    int edges;           /* E, as given in the file (see readAdjacencyListAsCSR
                           * for how this relates to array sizes for
                           * directed vs undirected graphs) */
    int *rowPtr;          /* size vertices + 1 */
    int *colIndex;         /* neighbor ids */
    int *edgeWeights;       /* NULL if isWeighted was 0 */
} CSRGraph;


CSRGraph readAdjacencyListAsCSR(FILE *filename, int isWeighted, int isDirected);

/* Reads a trailing "SOURCE s" line. Only used by algorithms with a single
 * source vertex (e.g. Bellman-Ford) — not called for Triangle Counting,
 * Betweenness Centrality, or Connected Components. */
int readSourceVertex(FILE *filename);

void freeCSRGraph(CSRGraph *graph);
void printCSRGraph(CSRGraph *graph);
#ifdef __cplusplus
}
#endif
#endif