#include "mst.h"
#include <vector>
#include <queue>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <climits>

// ================= Shared helper =================
void freeMSTResult(MSTResult *result) {
    free(result->edges);
    result->edges = nullptr;
    result->edgeCount = 0;
}

// ================= Kruskal =================
namespace {
    struct DSU {
        std::vector<int> parent, rank_;
        DSU(int n) : parent(n), rank_(n, 0) {
            for (int i = 0; i < n; i++) parent[i] = i;
        }
        int find(int x) {
            while (parent[x] != x) {
                parent[x] = parent[parent[x]];
                x = parent[x];
            }
            return x;
        }
        bool unite(int a, int b) {
            int ra = find(a), rb = find(b);
            if (ra == rb) return false;
            if (rank_[ra] < rank_[rb]) std::swap(ra, rb);
            parent[rb] = ra;
            if (rank_[ra] == rank_[rb]) rank_[ra]++;
            return true;
        }
    };
}

MSTResult kruskalMST(const CSRGraph *graph) {
    MSTResult result;
    result.edges = (MSTEdge *)malloc((graph->vertices - 1) * sizeof(MSTEdge));
    result.edgeCount = 0;
    result.totalWeight = 0;

    auto start = std::chrono::high_resolution_clock::now();

    // Step 1: extract unique undirected edges from CSR (neighbor > v avoids duplicates)
    std::vector<MSTEdge> edgeList;
    edgeList.reserve(graph->rowPtr[graph->vertices] / 2);
    for (int v = 0; v < graph->vertices; v++) {
        for (int idx = graph->rowPtr[v]; idx < graph->rowPtr[v + 1]; idx++) {
            int neighbor = graph->colIndex[idx];
            if (neighbor > v) {
                edgeList.push_back({v, neighbor, graph->edgeWeights[idx]});
            }
        }
    }

    // Step 2: sort by weight
    std::sort(edgeList.begin(), edgeList.end(),
              [](const MSTEdge &a, const MSTEdge &b) { return a.weight < b.weight; });

    // Step 3: DSU-based edge selection
    DSU dsu(graph->vertices);
    for (const auto &e : edgeList) {
        if (result.edgeCount == graph->vertices - 1) break;
        if (dsu.unite(e.u, e.v)) {
            result.edges[result.edgeCount++] = e;
            result.totalWeight += e.weight;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    result.executionTimeMs = std::chrono::duration<double, std::milli>(end - start).count();

    return result;
}

// ================= Prim =================
MSTResult primMST(const CSRGraph *graph) {
    MSTResult result;
    result.edges = (MSTEdge *)malloc((graph->vertices - 1) * sizeof(MSTEdge));
    result.edgeCount = 0;
    result.totalWeight = 0;

    auto start = std::chrono::high_resolution_clock::now();

    int V = graph->vertices;
    std::vector<int> key(V, INT_MAX);
    std::vector<int> parent(V, -1);
    std::vector<bool> inMST(V, false);

    // min-heap of (key, vertex)
    std::priority_queue<std::pair<int, int>,
                         std::vector<std::pair<int, int>>,
                         std::greater<std::pair<int, int>>> pq;

    int startVertex = 0; // recommended start, per assignment
    key[startVertex] = 0;
    pq.push({0, startVertex});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (inMST[u]) continue;
        inMST[u] = true;

        if (parent[u] != -1) {
            result.edges[result.edgeCount++] = {parent[u], u, key[u]};
            result.totalWeight += key[u];
        }

        for (int idx = graph->rowPtr[u]; idx < graph->rowPtr[u + 1]; idx++) {
            int v = graph->colIndex[idx];
            int w = graph->edgeWeights[idx];
            if (!inMST[v] && w < key[v]) {
                key[v] = w;
                parent[v] = u;
                pq.push({w, v});
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    result.executionTimeMs = std::chrono::duration<double, std::milli>(end - start).count();

    return result;
}