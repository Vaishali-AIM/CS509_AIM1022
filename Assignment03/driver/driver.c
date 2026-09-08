#include <stdio.h>
#include <stdlib.h>
#include "csr.h"
#include "mst.h"

static void printMSTResult(const char *algoName, const MSTResult *result) {
    printf("Algorithm: %s\n", algoName);
    printf("MST edges:\n");
    for (int i = 0; i < result->edgeCount; i++) {
        printf("%d %d %d\n", result->edges[i].u, result->edges[i].v, result->edges[i].weight);
    }
    printf("Total MST weight: %lld\n", result->totalWeight);
    printf("Execution time: %.6f ms\n", result->executionTimeMs);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open file '%s'\n", argv[1]);
        return 1;
    }

    CSRGraph graph = readAdjacencyListAsCSR(fp, 1, 0);
    fclose(fp);

    MSTResult kruskalResult = kruskalMST(&graph);
    printMSTResult("Kruskal's MST", &kruskalResult);
    printf("\n");

    MSTResult primResult = primMST(&graph);
    printMSTResult("Prim's MST", &primResult);

    if (kruskalResult.edgeCount != graph.vertices - 1 || primResult.edgeCount != graph.vertices - 1) {
        fprintf(stderr, "\nWarning: input graph appears disconnected — MST could not span all vertices.\n");
    }

    freeMSTResult(&kruskalResult);
    freeMSTResult(&primResult);
    freeCSRGraph(&graph);

    return 0;
}