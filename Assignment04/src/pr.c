#include "pr.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

PageRankResult runPageRank(CSRGraph *graph, double damping, double tolerance, int maxIterations) {
    PageRankResult result;
    int n = graph->vertices;
    int i, u, v, iter, edgeIdx;

    result.vertices = n;
    result.iterations = 0;
    result.converged = 0;
    result.sumOfRanks = 0.0;
    result.ranks = NULL;

    if (n <= 0) {
        return result;
    }

    double *rank = (double *)malloc((size_t)n * sizeof(double));
    double *newRank = (double *)malloc((size_t)n * sizeof(double));
    int *outdegree = (int *)malloc((size_t)n * sizeof(int));

    if (rank == NULL || newRank == NULL || outdegree == NULL) {
        free(rank);
        free(newRank);
        free(outdegree);
        return result;
    }

   
    for (v = 0; v < n; v++) {
        outdegree[v] = graph->rowPtr[v + 1] - graph->rowPtr[v];
    }

  
    double initRank = 1.0 / (double)n;
    for (v = 0; v < n; v++) {
        rank[v] = initRank;
    }

    int converged = 0;
    int performedIterations = 0;

    for (iter = 0; iter < maxIterations; iter++) {
       
        double danglingSum = 0.0;
        for (u = 0; u < n; u++) {
            if (outdegree[u] == 0) {
                danglingSum += rank[u];
            }
        }

        double baseValue = (1.0 - damping) / (double)n + damping * danglingSum / (double)n;
        for (v = 0; v < n; v++) {
            newRank[v] = baseValue;
        }

        for (u = 0; u < n; u++) {
            int deg = outdegree[u];
            if (deg == 0) {
                continue;
            }
            double contribution = damping * rank[u] / (double)deg;
            int start = graph->rowPtr[u];
            int end = graph->rowPtr[u + 1];
            for (edgeIdx = start; edgeIdx < end; edgeIdx++) {
                int dest = graph->colIndex[edgeIdx];
                newRank[dest] += contribution;
            }
        }

       
        double totalChange = 0.0;
        for (v = 0; v < n; v++) {
            totalChange += fabs(newRank[v] - rank[v]);
        }

        
        double *tmp = rank;
        rank = newRank;
        newRank = tmp;

        performedIterations++;

        if (totalChange <= tolerance) {
            converged = 1;
            break;
        }
    }

    double sum = 0.0;
    for (i = 0; i < n; i++) {
        sum += rank[i];
    }

    result.ranks = rank;
    result.iterations = performedIterations;
    result.converged = converged;
    result.sumOfRanks = sum;

    free(newRank);
    free(outdegree);

    return result;
}

void freePageRankResult(PageRankResult *result) {
    if (result == NULL) {
        return;
    }
    free(result->ranks);
    result->ranks = NULL;
    result->vertices = 0;
    result->iterations = 0;
    result->converged = 0;
    result->sumOfRanks = 0.0;
}

void printPageRankResult(const PageRankResult *result, double damping) {
    int v;

    printf("Algorithm: PageRank\n");
    printf("Damping: %g\n", damping);
    printf("Vertex ranks:\n");
    for (v = 0; v < result->vertices; v++) {
        printf("%d %f\n", v, result->ranks[v]);
    }
    printf("Sum of ranks: %f\n", result->sumOfRanks);
    printf("Iterations: %d\n", result->iterations);
    printf("Converged: %s\n", result->converged ? "true" : "false");
}