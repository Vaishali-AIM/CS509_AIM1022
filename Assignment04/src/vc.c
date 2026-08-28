#include "vc.h"
#include <stdlib.h>

typedef struct {
    int idx;
    int degree;
} VertexDegree;


static int compareVertexDegreeDesc(const void *a, const void *b) {
    const VertexDegree *va = (const VertexDegree *)a;
    const VertexDegree *vb = (const VertexDegree *)b;
    if (va->degree != vb->degree) return vb->degree - va->degree;
    return va->idx - vb->idx;
}

int *vertexColoring(int V, const int *rowPtr, const int *colIndex, int *numColorsUsed) {
    int *colors;
    VertexDegree *order;
    int *used;
    int usedCapacity;
    int maxColorUsed = -1;
    int v, i;

    *numColorsUsed = 0;
    if (V <= 0) return NULL;

    colors = malloc((size_t)V * sizeof(int));
    for (v = 0; v < V; v++) colors[v] = -1;

    order = malloc((size_t)V * sizeof(VertexDegree));
    for (v = 0; v < V; v++) {
        order[v].idx = v;
        order[v].degree = rowPtr[v + 1] - rowPtr[v];
    }
    qsort(order, (size_t)V, sizeof(VertexDegree), compareVertexDegreeDesc);

    
    usedCapacity = 1;
    used = malloc((size_t)usedCapacity * sizeof(int));
    used[0] = 0;

    for (i = 0; i < V; i++) {
        int u = order[i].idx;
        int e, c, chosen;

        if (maxColorUsed + 2 > usedCapacity) {
            int newCapacity = maxColorUsed + 2;
            int *grown = realloc(used, (size_t)newCapacity * sizeof(int));
            used = grown;
            usedCapacity = newCapacity;
        }
        for (c = 0; c < usedCapacity; c++) used[c] = 0;

        for (e = rowPtr[u]; e < rowPtr[u + 1]; e++) {
            int nbr = colIndex[e];
            int nc = colors[nbr];
            if (nc >= 0 && nc < usedCapacity) used[nc] = 1;
        }

        chosen = 0;
        while (chosen < usedCapacity && used[chosen]) chosen++;

        colors[u] = chosen;
        if (chosen > maxColorUsed) maxColorUsed = chosen;
    }

    free(order);
    free(used);

    *numColorsUsed = maxColorUsed + 1;
    return colors;
}

int isValidColoring(int V, const int *rowPtr, const int *colIndex, const int *colors) {
    int v, e;
    for (v = 0; v < V; v++) {
        if (colors[v] < 0) return 0;
        for (e = rowPtr[v]; e < rowPtr[v + 1]; e++) {
            int nbr = colIndex[e];
            if (colors[nbr] == colors[v]) return 0;
        }
    }
    return 1;
}