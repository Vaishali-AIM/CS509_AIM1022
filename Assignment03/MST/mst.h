#ifndef MST_H
#define MST_H

#include "csr.h"

struct MSTEdge {
    int u, v, weight;
};

struct MSTResult {
    MSTEdge *edges;        
    int edgeCount;      
    long long totalWeight;
    double executionTimeMs;
};

MSTResult kruskalMST(const CSRGraph *graph);
MSTResult primMST(const CSRGraph *graph);
void freeMSTResult(MSTResult *result);

#endif