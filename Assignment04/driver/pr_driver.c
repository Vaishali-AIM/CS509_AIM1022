#define _POSIX_C_SOURCE 199309L

#include "pr_driver.h"
#include "pr.h"
#include "csr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


static double elapsedMillis(struct timespec start, struct timespec end) {
    double seconds = (double)(end.tv_sec - start.tv_sec);
    double nanos = (double)(end.tv_nsec - start.tv_nsec);
    return seconds * 1000.0 + nanos / 1e6;
}


static int readTaggedDouble(FILE *fp, const char *expectedTag, double *outValue) {
    char tag[64];
    if (fscanf(fp, "%63s", tag) != 1) {
        return 0;
    }
    if (strcmp(tag, expectedTag) != 0) {
        return 0;
    }
    if (fscanf(fp, "%lf", outValue) != 1) {
        return 0;
    }
    return 1;
}

int runPageRankDriver(const char *inputFilePath) {
    if (inputFilePath == NULL) {
        fprintf(stderr, "Error: no input file path provided.\n");
        return 1;
    }

    FILE *fp = fopen(inputFilePath, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open input file '%s'.\n", inputFilePath);
        return 1;
    }

    
    CSRGraph graph = readAdjacencyListAsCSR(fp, 0, 1);

    if (graph.vertices <= 0 || graph.rowPtr == NULL || graph.colIndex == NULL) {
        fprintf(stderr, "Error: failed to parse a valid CSR graph from '%s'.\n", inputFilePath);
        fclose(fp);
        freeCSRGraph(&graph);
        return 1;
    }

    double damping = 0.0, tolerance = 0.0, maxIterationsD = 0.0;

    if (!readTaggedDouble(fp, "DAMPING", &damping)) {
        fprintf(stderr, "Error: missing or malformed DAMPING line in '%s'.\n", inputFilePath);
        fclose(fp);
        freeCSRGraph(&graph);
        return 1;
    }
    if (!readTaggedDouble(fp, "TOLERANCE", &tolerance)) {
        fprintf(stderr, "Error: missing or malformed TOLERANCE line in '%s'.\n", inputFilePath);
        fclose(fp);
        freeCSRGraph(&graph);
        return 1;
    }
    if (!readTaggedDouble(fp, "MAX_ITERATIONS", &maxIterationsD)) {
        fprintf(stderr, "Error: missing or malformed MAX_ITERATIONS line in '%s'.\n", inputFilePath);
        fclose(fp);
        freeCSRGraph(&graph);
        return 1;
    }

    fclose(fp);

    int maxIterations = (int)maxIterationsD;

    if (damping <= 0.0 || damping >= 1.0) {
        fprintf(stderr, "Error: DAMPING must satisfy 0 < damping < 1 (got %g).\n", damping);
        freeCSRGraph(&graph);
        return 1;
    }
    if (tolerance <= 0.0) {
        fprintf(stderr, "Error: TOLERANCE must be positive (got %g).\n", tolerance);
        freeCSRGraph(&graph);
        return 1;
    }
    if (maxIterations <= 0) {
        fprintf(stderr, "Error: MAX_ITERATIONS must be positive (got %d).\n", maxIterations);
        freeCSRGraph(&graph);
        return 1;
    }

    struct timespec startTime, endTime;
    clock_gettime(CLOCK_MONOTONIC, &startTime);

    PageRankResult result = runPageRank(&graph, damping, tolerance, maxIterations);

    clock_gettime(CLOCK_MONOTONIC, &endTime);
    double execMs = elapsedMillis(startTime, endTime);

    if (result.ranks == NULL) {
        fprintf(stderr, "Error: PageRank failed to allocate working memory.\n");
        freeCSRGraph(&graph);
        return 1;
    }

    printPageRankResult(&result, damping);
    printf("Execution time: %.3f ms\n", execMs);

    freePageRankResult(&result);
    freeCSRGraph(&graph);

    return 0;
}

#ifndef PR_NO_MAIN
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pagerank_input_file.txt>\n", argv[0]);
        return 1;
    }
    return runPageRankDriver(argv[1]);
}
#endif