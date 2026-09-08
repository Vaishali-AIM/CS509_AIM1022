#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "mst.h"

void freeMSTResult(MSTResult *result) {
    free(result->edges);
    result->edges = NULL;
    result->edgeCount = 0;
}

/* ---- Disjoint Set Union (for Kruskal's) ---- */
typedef struct {
    int *parent;
    int *rank;
} DSU;

static void dsu_init(DSU *dsu, int n) {
    dsu->parent = (int *)malloc(n * sizeof(int));
    dsu->rank = (int *)calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) dsu->parent[i] = i;
}

static int dsu_find(DSU *dsu, int x) {
    while (dsu->parent[x] != x) {
        dsu->parent[x] = dsu->parent[dsu->parent[x]];
        x = dsu->parent[x];
    }
    return x;
}

static int dsu_unite(DSU *dsu, int a, int b) {
    int ra = dsu_find(dsu, a), rb = dsu_find(dsu, b);
    if (ra == rb) return 0;
    if (dsu->rank[ra] < dsu->rank[rb]) { int tmp = ra; ra = rb; rb = tmp; }
    dsu->parent[rb] = ra;
    if (dsu->rank[ra] == dsu->rank[rb]) dsu->rank[ra]++;
    return 1;
}

static void dsu_free(DSU *dsu) {
    free(dsu->parent);
    free(dsu->rank);
}

static int compareEdgesByWeight(const void *a, const void *b) {
    const MSTEdge *ea = (const MSTEdge *)a;
    const MSTEdge *eb = (const MSTEdge *)b;
    return ea->weight - eb->weight;
}

MSTResult kruskalMST(const CSRGraph *graph) {
    MSTResult result;
    result.edges = (MSTEdge *)malloc((graph->vertices - 1) * sizeof(MSTEdge));
    result.edgeCount = 0;
    result.totalWeight = 0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int maxEdges = graph->rowPtr[graph->vertices] / 2 + 1;
    MSTEdge *edgeList = (MSTEdge *)malloc(maxEdges * sizeof(MSTEdge));
    int edgeCount = 0;

    for (int v = 0; v < graph->vertices; v++) {
        for (int idx = graph->rowPtr[v]; idx < graph->rowPtr[v + 1]; idx++) {
            int neighbor = graph->colIndex[idx];
            if (neighbor > v) {
                edgeList[edgeCount].u = v;
                edgeList[edgeCount].v = neighbor;
                edgeList[edgeCount].weight = graph->edgeWeights[idx];
                edgeCount++;
            }
        }
    }

    qsort(edgeList, edgeCount, sizeof(MSTEdge), compareEdgesByWeight);

    DSU dsu;
    dsu_init(&dsu, graph->vertices);

    for (int i = 0; i < edgeCount && result.edgeCount < graph->vertices - 1; i++) {
        if (dsu_unite(&dsu, edgeList[i].u, edgeList[i].v)) {
            result.edges[result.edgeCount++] = edgeList[i];
            result.totalWeight += edgeList[i].weight;
        }
    }

    dsu_free(&dsu);
    free(edgeList);

    clock_gettime(CLOCK_MONOTONIC, &end);
    result.executionTimeMs = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1e6;

    return result;
}

/* ---- Binary min-heap (for Prim's), same pattern as SSSP's Dijkstra ---- */
typedef struct { int key; int vertex; } HeapNode;
typedef struct { HeapNode *data; int size; } MinHeap;

static MinHeap heap_create(int capacity) {
    MinHeap h;
    h.data = (HeapNode *)malloc(capacity * sizeof(HeapNode));
    h.size = 0;
    return h;
}

static void heap_push(MinHeap *h, HeapNode node) {
    int i = h->size++;
    h->data[i] = node;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->data[parent].key <= h->data[i].key) break;
        HeapNode tmp = h->data[parent]; h->data[parent] = h->data[i]; h->data[i] = tmp;
        i = parent;
    }
}

static HeapNode heap_pop(MinHeap *h) {
    HeapNode top = h->data[0];
    h->data[0] = h->data[--h->size];
    int i = 0;
    while (1) {
        int l = 2 * i + 1, r = 2 * i + 2, smallest = i;
        if (l < h->size && h->data[l].key < h->data[smallest].key) smallest = l;
        if (r < h->size && h->data[r].key < h->data[smallest].key) smallest = r;
        if (smallest == i) break;
        HeapNode tmp = h->data[smallest]; h->data[smallest] = h->data[i]; h->data[i] = tmp;
        i = smallest;
    }
    return top;
}

MSTResult primMST(const CSRGraph *graph) {
    MSTResult result;
    result.edges = (MSTEdge *)malloc((graph->vertices - 1) * sizeof(MSTEdge));
    result.edgeCount = 0;
    result.totalWeight = 0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int V = graph->vertices;
    int *key = (int *)malloc(V * sizeof(int));
    int *parent = (int *)malloc(V * sizeof(int));
    int *inMST = (int *)calloc(V, sizeof(int));
    for (int i = 0; i < V; i++) { key[i] = INT_MAX; parent[i] = -1; }

    MinHeap pq = heap_create(graph->rowPtr[V] + V);

    int startVertex = 0;
    key[startVertex] = 0;
    heap_push(&pq, (HeapNode){0, startVertex});

    while (pq.size > 0) {
        HeapNode top = heap_pop(&pq);
        int u = top.vertex;

        if (inMST[u]) continue;
        inMST[u] = 1;

        if (parent[u] != -1) {
            result.edges[result.edgeCount].u = parent[u];
            result.edges[result.edgeCount].v = u;
            result.edges[result.edgeCount].weight = key[u];
            result.edgeCount++;
            result.totalWeight += key[u];
        }

        for (int idx = graph->rowPtr[u]; idx < graph->rowPtr[u + 1]; idx++) {
            int v = graph->colIndex[idx];
            int w = graph->edgeWeights[idx];
            if (!inMST[v] && w < key[v]) {
                key[v] = w;
                parent[v] = u;
                heap_push(&pq, (HeapNode){w, v});
            }
        }
    }

    free(pq.data);
    free(key);
    free(parent);
    free(inMST);

    clock_gettime(CLOCK_MONOTONIC, &end);
    result.executionTimeMs = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1e6;

    return result;
}