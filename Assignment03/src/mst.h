#ifndef MST_H
#define MST_H

#include "csr.h"

typedef struct {
    int u, v, weight;
} MSTEdge;

typedef struct {
    MSTEdge *edges;
    int edgeCount;
    long long totalWeight;
    double executionTimeMs;
} MSTResult;

MSTResult kruskalMST(const CSRGraph *graph);
MSTResult primMST(const CSRGraph *graph);
void freeMSTResult(MSTResult *result);

#endif