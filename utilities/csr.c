#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "csr.h"

static void failWithFormatError(const char *message) {
    fprintf(stderr, "Error: Invalid input file format - %s\n", message);
    exit(1);
}

CSRGraph readAdjacencyListAsCSR(FILE *filename, int isWeighted, int isDirected) {
    CSRGraph graph;

    char headerLine[256];
    if (fgets(headerLine, sizeof(headerLine), filename) == NULL)
        failWithFormatError("file is empty — expected 'V E' on the first line.");

    if (sscanf(headerLine, "%d %d", &graph.vertices, &graph.edges) != 2)
        failWithFormatError("expected 'V E' on the first line, but couldn't read two integers from it.");

    if (graph.vertices <= 0)
        failWithFormatError("Number of vertices (V) must be a positive integer");

    if (graph.edges < 0)
        failWithFormatError("Number of edges (E) cannot be negative.");

    
    int maxPossibleEntries = isDirected ? graph.edges : 2 * graph.edges;
    printf("Vertices: %d, Edges: %d\n", graph.vertices, graph.edges);

    graph.rowPtr = malloc((graph.vertices + 1) * sizeof(int));
    graph.colIndex = malloc(maxPossibleEntries * sizeof(int));
    graph.edgeWeights = isWeighted ? malloc(maxPossibleEntries * sizeof(int)) : NULL;

    graph.rowPtr[0] = 0;
    int nextFreeSlot = 0;   // tracks where to write the next neighbor in col_idx

    for (int v = 0; v < graph.vertices; v++) {
        int vertexId, degree;

        if (fscanf(filename, "%d %d", &vertexId, &degree) != 2)
            failWithFormatError("expected a vertex line (id and degree), but the file ended early or contained non-integer data.");

        if (vertexId != v)
            failWithFormatError("vertices must be listed in order 0 to V-1; expected vertex line for a different vertex than the one found.");

        if (degree < 0)
            failWithFormatError("a vertex's degree cannot be negative.");

        // add the neighbors of vertex v to the colIndex array
        for (int i = 0; i < degree; i++) {
            if (fscanf(filename, "%d", &graph.colIndex[nextFreeSlot]) != 1)
                failWithFormatError("Expected a neighbor vertex id, but the file ended early or contained non-integer data");

            if (graph.colIndex[nextFreeSlot] < 0 || graph.colIndex[nextFreeSlot] >= graph.vertices)
                failWithFormatError("a neighbor id is out of range (must be between 0 and V-1)");

            if (isWeighted) {
                if (fscanf(filename, "%d", &graph.edgeWeights[nextFreeSlot]) != 1)
                    failWithFormatError("expected an edge weight after the neighbor id, but the file ended early or contained non-integer data.");

                
                if (!isDirected && graph.edgeWeights[nextFreeSlot] < 0)
                    failWithFormatError("negative edge weights are not allowed on undirected edges.");
            }
            nextFreeSlot++;
        }

       
        graph.rowPtr[v + 1] = nextFreeSlot;
    }

    return graph;
}

int readSourceVertex(FILE *filename) {
    char keyword[16];
    int source;
    if (fscanf(filename, "%s %d", keyword, &source) != 2)
        failWithFormatError("expected a 'SOURCE s' line at the end of the file");

    if (strcasecmp(keyword, "source") != 0)
        failWithFormatError("expected the keyword 'SOURCE' before the source vertex id");

    return source;
}

void freeCSRGraph(CSRGraph *graph) {
    free(graph->rowPtr);
    free(graph->colIndex);
    if (graph->edgeWeights != NULL)
        free(graph->edgeWeights);
}

void printCSRGraph(CSRGraph *graph) {
    printf("Number of vertices: %d\n", graph->vertices);
    printf("Number of edges: %d\n", graph->edges);

    printf("row_ptr: ");
    for (int i = 0; i <= graph->vertices; i++)
        printf("%d ", graph->rowPtr[i]);
    printf("\n");

    printf("col_idx: ");
    for (int i = 0; i < graph->rowPtr[graph->vertices]; i++)
        printf("%d ", graph->colIndex[i]);
    printf("\n");

    if (graph->edgeWeights != NULL) {
        printf("edgeWeights:  ");
        for (int i = 0; i < graph->rowPtr[graph->vertices]; i++)
            printf("%d ", graph->edgeWeights[i]);
        printf("\n");
    }
}