#ifndef PR_H
#define PR_H

#include "csr.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    double *ranks;     
    int vertices;        
    int iterations;       
    int converged;         
    double sumOfRanks;     
} PageRankResult;


PageRankResult runPageRank(CSRGraph *graph, double damping, double tolerance, int maxIterations);

void freePageRankResult(PageRankResult *result);

void printPageRankResult(const PageRankResult *result, double damping);

#ifdef __cplusplus
}
#endif

#endif 