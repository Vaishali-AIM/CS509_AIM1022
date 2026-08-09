#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "read_matrix.h"

#define INF_SENTINEL ((long long)1e15)

static void failWithFormatError(const char *message) {
    fprintf(stderr, "Error: Invalid input file format - %s\n", message);
    exit(1);
}

DenseGraph readAdjacencyMatrix(FILE *filename) {
    DenseGraph g;

    if (fscanf(filename, "%d", &g.vertices) != 1)
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
            if (fscanf(filename, "%63s", token) != 1)
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

void freeDenseGraph(DenseGraph *g) {
    for (int i = 0; i < g->vertices; i++) {
        free(g->dist[i]);
    }
    free(g->dist);
    g->dist = NULL;
}