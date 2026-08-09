#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "fw_driver.h"
#include "floyd_warshall.h"

#define INF_SENTINEL ((long long)1e15)

static void failWithFormatError(const char *message) {
    fprintf(stderr, "Error: Invalid input file format - %s\n", message);
    exit(1);
}

static DenseGraph readMatrixFromFile(FILE *fp) {
    DenseGraph g;

    if (fscanf(fp, "%d", &g.vertices) != 1)
        failWithFormatError("expected V (number of vertices) on the first line.");

    if (g.vertices <= 0)
        failWithFormatError("Number of vertices (V) must be a positive integer.");

    g.dist = (long long **)malloc(g.vertices * sizeof(long long *));
    for (int i = 0; i < g.vertices; i++) {
        g.dist[i] = (long long *)malloc(g.vertices * sizeof(long long));
    }

    char token[64];
    for (int i = 0; i < g.vertices; i++) {
        for (int j = 0; j < g.vertices; j++) {
            if (fscanf(fp, "%63s", token) != 1)
                failWithFormatError("file ended early or a row has fewer than V entries.");

            if (strcasecmp(token, "INF") == 0) {
                g.dist[i][j] = INF_SENTINEL;
            } else {
                char *endptr;
                long long value = strtoll(token, &endptr, 10);
                if (*endptr != '\0')
                    failWithFormatError("expected an integer or the token 'INF' in the matrix.");
                g.dist[i][j] = value;
            }
        }
    }

    for (int i = 0; i < g.vertices; i++) {
        if (g.dist[i][i] != 0)
            failWithFormatError("diagonal entry dist[i][i] must be 0.");
    }

    return g;
}

static void freeDenseGraph(DenseGraph *g) {
    for (int i = 0; i < g->vertices; i++) {
        free(g->dist[i]);
    }
    free(g->dist);
    g->dist = NULL;
}

int run_floyd_warshall(const char *filepath) {
    /* ---- (NOT timed) ---- */
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "Error: could not open input file '%s'\n", filepath);
        return 1;
    }

    DenseGraph g = readMatrixFromFile(fp);
    fclose(fp);

    bool neg_cycle;

    /* ---- Timed phase ---- */
    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    floyd_warshall(&g, &neg_cycle);

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double elapsed_ms = (t_end.tv_sec - t_start.tv_sec) * 1000.0 +
                         (t_end.tv_nsec - t_start.tv_nsec) / 1e6;

    
    printf("Algorithm: Floyd-Warshall\n");

    if (neg_cycle) {
        printf("Negative cycle: true\n");
    } else {
        printf("Distance matrix:\n");
        for (int i = 0; i < g.vertices; i++) {
            for (int j = 0; j < g.vertices; j++) {
                if (g.dist[i][j] >= INF_SENTINEL) {
                    printf("INF");
                } else {
                    printf("%lld", g.dist[i][j]);
                }
                if (j < g.vertices - 1) printf(" ");
            }
            printf("\n");
        }
        printf("Negative cycle: none\n");
    }
    printf("Execution time: %.4f ms\n", elapsed_ms);

    freeDenseGraph(&g);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    return run_floyd_warshall(argv[1]);
}