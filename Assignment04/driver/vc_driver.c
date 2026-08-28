#define _POSIX_C_SOURCE 200809L /* for getline() and clock_gettime()/CLOCK_MONOTONIC */

#include "vc_driver.h"
#include "vc.h"
#include "csr.h" 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>



typedef struct {
    int u;
    int v;
} EdgeEntry;

static ssize_t nextNonBlankLine(FILE *fp, char **line, size_t *cap) {
    ssize_t len;
    while ((len = getline(line, cap, fp)) != -1) {
        char *p = *line;
        while (*p && isspace((unsigned char)*p)) p++;
        if (*p != '\0') return len;
    }
    return -1;
}

static int compareEdgeEntry(const void *a, const void *b) {
    const EdgeEntry *ea = (const EdgeEntry *)a;
    const EdgeEntry *eb = (const EdgeEntry *)b;
    if (ea->u != eb->u) return ea->u - eb->u;
    return ea->v - eb->v;
}

static int compareInt(const void *a, const void *b) {
    return *(const int *)a - *(const int *)b;
}


static int mirrorEdgeExists(const EdgeEntry *sorted, int count, int u, int v) {
    EdgeEntry key;
    key.u = v;
    key.v = u;
    return bsearch(&key, sorted, (size_t)count, sizeof(EdgeEntry), compareEdgeEntry) != NULL;
}


static int validateVertexColoringFile(const char *path, char *errBuf, size_t errBufSize) {
    FILE *fp;
    char *line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen;
    int V = 0, E = 0;
    int row;
    int *seen = NULL;
    int *rowNbrs = NULL;
    int rowNbrsCap = 0;
    EdgeEntry *edges = NULL;
    int edgeCount = 0, edgeCap = 0;
    long degreeSum = 0;
    int ok = 1;
    int e;

    fp = fopen(path, "r");
    if (!fp) {
        snprintf(errBuf, errBufSize, "Could not open input file: %s", path);
        return 0;
    }

    lineLen = nextNonBlankLine(fp, &line, &lineCap);
    if (lineLen < 0) {
        snprintf(errBuf, errBufSize, "Missing header line: expected 'V E'.");
        fclose(fp);
        free(line);
        return 0;
    }
    if (sscanf(line, "%d %d", &V, &E) != 2) {
        snprintf(errBuf, errBufSize, "Malformed header: expected 'V E' on the first line.");
        fclose(fp);
        free(line);
        return 0;
    }
    if (V <= 0) {
        snprintf(errBuf, errBufSize, "Number of vertices (V) must be a positive integer.");
        fclose(fp);
        free(line);
        return 0;
    }
    if (E < 0) {
        snprintf(errBuf, errBufSize, "Number of edges (E) cannot be negative.");
        fclose(fp);
        free(line);
        return 0;
    }

    seen = calloc((size_t)V, sizeof(int));
    edgeCap = (E > 0) ? 2 * E : 16;
    edges = malloc((size_t)edgeCap * sizeof(EdgeEntry));

    for (row = 0; row < V && ok; row++) {
        char *ptr, *endptr;
        long u, degree;
        int i;

        lineLen = nextNonBlankLine(fp, &line, &lineCap);
        if (lineLen < 0) {
            snprintf(errBuf, errBufSize,
                     "Missing adjacency row %d: file ended early (expected %d rows).", row, V);
            ok = 0;
            break;
        }

        ptr = line;
        u = strtol(ptr, &endptr, 10);
        if (endptr == ptr) {
            snprintf(errBuf, errBufSize,
                     "Malformed adjacency row %d: expected 'vertex_id degree neighbours...'.", row);
            ok = 0;
            break;
        }
        ptr = endptr;
        degree = strtol(ptr, &endptr, 10);
        if (endptr == ptr) {
            snprintf(errBuf, errBufSize,
                     "Malformed adjacency row %d: missing degree field.", row);
            ok = 0;
            break;
        }
        ptr = endptr;

        if (u != row) {
            snprintf(errBuf, errBufSize,
                     "Vertices must be listed in order 0..V-1: expected vertex %d, found %ld.", row, u);
            ok = 0;
            break;
        }
        if (seen[u]) {
            snprintf(errBuf, errBufSize, "Duplicate adjacency row for vertex %ld.", u);
            ok = 0;
            break;
        }
        seen[u] = 1;
        if (degree < 0) {
            snprintf(errBuf, errBufSize, "Negative degree for vertex %ld.", u);
            ok = 0;
            break;
        }

        if (degree > rowNbrsCap) {
            rowNbrsCap = (int)degree;
            rowNbrs = realloc(rowNbrs, (size_t)rowNbrsCap * sizeof(int));
        }

        for (i = 0; i < degree; i++) {
            long nbr = strtol(ptr, &endptr, 10);
            if (endptr == ptr) {
                snprintf(errBuf, errBufSize,
                         "Mismatched degree/neighbour count for vertex %ld: declared degree %ld "
                         "but only %d neighbour(s) were given on that line.", u, degree, i);
                ok = 0;
                break;
            }
            ptr = endptr;
            if (nbr < 0 || nbr >= V) {
                snprintf(errBuf, errBufSize,
                         "Out-of-range neighbour id %ld for vertex %ld.", nbr, u);
                ok = 0;
                break;
            }
            if (nbr == u) {
                snprintf(errBuf, errBufSize,
                         "Self-loop detected at vertex %ld (self-loops are not permitted).", u);
                ok = 0;
                break;
            }
            rowNbrs[i] = (int)nbr;

            if (edgeCount >= edgeCap) {
                edgeCap *= 2;
                edges = realloc(edges, (size_t)edgeCap * sizeof(EdgeEntry));
            }
            edges[edgeCount].u = (int)u;
            edges[edgeCount].v = (int)nbr;
            edgeCount++;
        }
        if (!ok) break;


        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (*ptr != '\0') {
            snprintf(errBuf, errBufSize,
                     "Mismatched degree/neighbour count for vertex %ld: declared degree %ld "
                     "but more neighbours were given on that line.", u, degree);
            ok = 0;
            break;
        }

       
        if (degree > 1) {
            int *sortedRow = malloc((size_t)degree * sizeof(int));
            memcpy(sortedRow, rowNbrs, (size_t)degree * sizeof(int));
            qsort(sortedRow, (size_t)degree, sizeof(int), compareInt);
            for (i = 1; i < degree; i++) {
                if (sortedRow[i] == sortedRow[i - 1]) {
                    snprintf(errBuf, errBufSize,
                             "Parallel edge: vertex %ld lists neighbour %d more than once.",
                             u, sortedRow[i]);
                    ok = 0;
                    break;
                }
            }
            free(sortedRow);
            if (!ok) break;
        }

        degreeSum += degree;
    }

    if (ok) {
        for (row = 0; row < V; row++) {
            if (!seen[row]) {
                snprintf(errBuf, errBufSize, "Vertex %d is missing an adjacency row.", row);
                ok = 0;
                break;
            }
        }
    }

    if (ok && degreeSum != 2L * E) {
        snprintf(errBuf, errBufSize,
                 "Sum of degrees (%ld) does not equal 2*E (2*%d = %ld); the file's row degrees "
                 "are inconsistent with its declared edge count.", degreeSum, E, 2L * E);
        ok = 0;
    }

    if (ok && edgeCount > 0) {
        EdgeEntry *sorted = malloc((size_t)edgeCount * sizeof(EdgeEntry));
        memcpy(sorted, edges, (size_t)edgeCount * sizeof(EdgeEntry));
        qsort(sorted, (size_t)edgeCount, sizeof(EdgeEntry), compareEdgeEntry);
        for (e = 0; e < edgeCount && ok; e++) {
            if (!mirrorEdgeExists(sorted, edgeCount, edges[e].u, edges[e].v)) {
                snprintf(errBuf, errBufSize,
                         "Edge %d-%d is not symmetric: it must also appear in vertex %d's "
                         "adjacency list.", edges[e].u, edges[e].v, edges[e].v);
                ok = 0;
            }
        }
        free(sorted);
    }

    free(seen);
    free(rowNbrs);
    free(edges);
    free(line);
    fclose(fp);
    return ok;
}



int runVertexColoring(const char *inputFile) {
    char errBuf[512];
    FILE *fp;
    CSRGraph graph;
    int *colors;
    int numColorsUsed;
    int valid;
    struct timespec startTime, endTime;
    double elapsedMs;
    int v;

    if (!validateVertexColoringFile(inputFile, errBuf, sizeof(errBuf))) {
        fprintf(stderr, "Error: %s\n", errBuf);
        return 1;
    }

    fp = fopen(inputFile, "r");
    if (!fp) {
        fprintf(stderr, "Error: Could not open input file: %s\n", inputFile);
        return 1;
    }

   
    graph = readAdjacencyListAsCSR(fp, 0, 0);
    fclose(fp);

    clock_gettime(CLOCK_MONOTONIC, &startTime);
    colors = vertexColoring(graph.vertices, graph.rowPtr, graph.colIndex, &numColorsUsed);
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    elapsedMs = (double)(endTime.tv_sec - startTime.tv_sec) * 1000.0 +
                (double)(endTime.tv_nsec - startTime.tv_nsec) / 1.0e6;
    

    valid = isValidColoring(graph.vertices, graph.rowPtr, graph.colIndex, colors);

    
    printf("Algorithm: Greedy Vertex Coloring\n");
    printf("Vertex colors:\n");
    for (v = 0; v < graph.vertices; v++) {
        printf("%d %d\n", v, colors[v]);
    }
    printf("Colors used: %d\n", numColorsUsed);
    printf("Valid: %s\n", valid ? "true" : "false");
    printf("Execution time: %f ms\n", elapsedMs);

    free(colors);
    freeCSRGraph(&graph);

    return 0;
}

#ifndef VC_NO_MAIN

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <adjacency_list_input_file>\n", argv[0]);
        return 1;
    }
    return runVertexColoring(argv[1]);
}
#endif