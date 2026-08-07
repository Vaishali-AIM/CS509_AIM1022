#include <stdlib.h>
#include "bellman_ford.h"

/* Large sentinel instead of a true "max" value, so dist[u] + weight never
 * overflows while u is still unreached. */
#define INF ((long long)1e15)

void bellman_ford(const CSRGraph *g, int source, long long *dist,
                   bool *has_negative_cycle) {
    int V = g->vertices;

    for (int i = 0; i < V; i++) {
        dist[i] = INF;
    }
    dist[source] = 0;

    /* Pass 1 .. V-1: relax every edge */
    for (int pass = 0; pass < V - 1; pass++) {
        bool changed = false;

        for (int u = 0; u < V; u++) {
            if (dist[u] == INF) continue; /* nothing to relax through u yet */

            for (int k = g->rowPtr[u]; k < g->rowPtr[u + 1]; k++) {
                int neighbor = g->colIndex[k];
                int weight = g->edgeWeights[k];

                if (dist[u] + weight < dist[neighbor]) {
                    dist[neighbor] = dist[u] + weight;
                    changed = true;
                }
            }
        }

        /* If nothing changed this pass, distances have already converged —
         * further passes are guaranteed no-ops. */
        if (!changed) break;
    }

    /* Pass V: negative-cycle check. If anything can still be relaxed,
     * a negative-weight cycle is reachable from source. */
    *has_negative_cycle = false;

    for (int u = 0; u < V; u++) {
        if (dist[u] == INF) continue;

        for (int k = g->rowPtr[u]; k < g->rowPtr[u + 1]; k++) {
            int neighbor = g->colIndex[k];
            int weight = g->edgeWeights[k];

            if (dist[u] + weight < dist[neighbor]) {
                *has_negative_cycle = true;
                return;
            }
        }
    }
}