#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bf_driver.h"
#include "csr.h"
#include "bellman_ford.h"
#include "floyd_warshall.h"

#define INF_DISPLAY ((long long)1e15)

int run_bellman_ford(const char *filepath) {
    
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "Error: could not open input file '%s'\n", filepath);
        return 1;
    }

    CSRGraph g = readAdjacencyListAsCSR(fp, /*isWeighted=*/1, /*isDirected=*/1);
    int source = readSourceVertex(fp);
    fclose(fp);

    long long *dist = (long long *)malloc(g.vertices * sizeof(long long));
    bool neg_cycle;

    
    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    bellman_ford(&g, source, dist, &neg_cycle);

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double elapsed_ms = (t_end.tv_sec - t_start.tv_sec) * 1000.0 +
                         (t_end.tv_nsec - t_start.tv_nsec) / 1e6;

    
    printf("Algorithm: Bellman-Ford\n");
    printf("Source: %d\n", source);

    if (neg_cycle) {
        printf("Negative cycle: true\n");
    } else {
        printf("Vertex Distance\n");
        for (int i = 0; i < g.vertices; i++) {
            if (dist[i] >= INF_DISPLAY) {
                printf("%-6d INF\n", i);
            } else {
                printf("%-6d %lld\n", i, dist[i]);
            }
        }
        printf("Negative cycle: none\n");
    }
    printf("Execution time: %.4f ms\n", elapsed_ms);

    free(dist);
    freeCSRGraph(&g);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    return run_bellman_ford(argv[1]);
}