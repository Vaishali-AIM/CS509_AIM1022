#include "floyd_warshall.h"
#define INF_SENTINEL ((long long)1e15)

void floyd_warshall(DenseGraph *g, bool *has_negative_cycle) {
    int V = g->vertices;
    long long **dist = g->dist;

    /* For every intermediate vertex k, try routing i -> k -> j and keep
     * the shorter of that or the existing i -> j distance. */
    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            if (dist[i][k] >= INF_SENTINEL) continue; /* i can't reach k */

            for (int j = 0; j < V; j++) {
                if (dist[k][j] >= INF_SENTINEL) continue; /* k can't reach j */

                long long through_k = dist[i][k] + dist[k][j];
                if (through_k < dist[i][j]) {
                    dist[i][j] = through_k;
                }
            }
        }
    }

    /* Negative-cycle check */
    *has_negative_cycle = false;
    for (int i = 0; i < V; i++) {
        if (dist[i][i] < 0) {
            *has_negative_cycle = true;
            break;
        }
    }
}