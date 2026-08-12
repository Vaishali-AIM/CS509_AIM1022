#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "csr.h"
#include "bellman_ford.h"
#include "floyd_warshall.h"

#define INF_SENTINEL ((long long)1e15)

static void failWithFormatError(const char *message) {
    fprintf(stderr, "Error: Invalid input file format - %s\n", message);
    exit(1);
}

/* Same inline matrix reader as fw_driver.c, duplicated here since we
 * decided matrix-reading stays inline per-file rather than shared. */
static DenseGraph readMatrixFromFile(FILE *fp) {
    DenseGraph g;
    if (fscanf(fp, "%d", &g.vertices) != 1)
        failWithFormatError("expected V on the first line.");

    g.dist = (long long **)malloc(g.vertices * sizeof(long long *));
    for (int i = 0; i < g.vertices; i++) {
        g.dist[i] = (long long *)malloc(g.vertices * sizeof(long long));
    }

    char token[64];
    for (int i = 0; i < g.vertices; i++) {
        for (int j = 0; j < g.vertices; j++) {
            if (fscanf(fp, "%63s", token) != 1)
                failWithFormatError("file ended early.");
            if (strcasecmp(token, "INF") == 0) {
                g.dist[i][j] = INF_SENTINEL;
            } else {
                g.dist[i][j] = strtoll(token, NULL, 10);
            }
        }
    }
    return g;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <bf_file> <fw_file>\n", argv[0]);
        return 1;
    }

    /* --- Run Bellman-Ford from every vertex as source --- */
    FILE *bfFp = fopen(argv[1], "r");
    if (!bfFp) { fprintf(stderr, "Error: could not open '%s'\n", argv[1]); return 1; }
    CSRGraph g = readAdjacencyListAsCSR(bfFp, /*isWeighted=*/1, /*isDirected=*/1);
    readSourceVertex(bfFp); /* file's own SOURCE line is irrelevant here — we test every source */
    fclose(bfFp);

    int V = g.vertices;
    long long **bfMatrix = (long long **)malloc(V * sizeof(long long *));
    for (int src = 0; src < V; src++) {
        bfMatrix[src] = (long long *)malloc(V * sizeof(long long));
        bool negCycle;
        bellman_ford(&g, src, bfMatrix[src], &negCycle);
        if (negCycle) {
            fprintf(stderr, "Error: negative cycle detected from source %d — cannot cross-check.\n", src);
            return 1;
        }
    }

    /* --- Run Floyd-Warshall once on the same graph (matrix form) --- */
    FILE *fwFp = fopen(argv[2], "r");
    if (!fwFp) { fprintf(stderr, "Error: could not open '%s'\n", argv[2]); return 1; }
    DenseGraph dg = readMatrixFromFile(fwFp);
    fclose(fwFp);

    if (dg.vertices != V) {
        fprintf(stderr, "Error: vertex count mismatch (bf=%d, fw=%d) — files aren't the same graph.\n",
                V, dg.vertices);
        return 1;
    }

    bool fwNegCycle;
    floyd_warshall(&dg, &fwNegCycle);
    if (fwNegCycle) {
        fprintf(stderr, "Error: Floyd-Warshall detected a negative cycle — cannot cross-check.\n");
        return 1;
    }

    /* --- Compare every entry --- */
    int mismatches = 0;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (bfMatrix[i][j] != dg.dist[i][j]) {
                mismatches++;
                if (mismatches <= 10) {
                    fprintf(stderr, "  mismatch at (%d,%d): BF=%lld FW=%lld\n",
                            i, j, bfMatrix[i][j], dg.dist[i][j]);
                }
            }
        }
    }

    if (mismatches == 0) {
        printf("Cross-check PASS: all %d x %d = %d entries match between Bellman-Ford (run from every source) and Floyd-Warshall.\n",
               V, V, V * V);
    } else {
        printf("Cross-check FAIL: %d / %d entries mismatched.\n", mismatches, V * V);
    }

    for (int i = 0; i < V; i++) { free(bfMatrix[i]); free(dg.dist[i]); }
    free(bfMatrix); free(dg.dist);
    freeCSRGraph(&g);

    return mismatches == 0 ? 0 : 1;
}
