#ifndef CSR_H
#define CSR_H

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int vertices;      
    int edges;           
    int *rowPtr;         
    int *colIndex;        
    int *edgeWeights;      
} CSRGraph;


CSRGraph readAdjacencyListAsCSR(FILE *filename, int isWeighted, int isDirected);

int readSourceVertex(FILE *filename);

void freeCSRGraph(CSRGraph *graph);
void printCSRGraph(CSRGraph *graph);
#ifdef __cplusplus
}
#endif
#endif