#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INF_SENTINEL ((long long)1e15)
#define NEGATIVE_EDGE_FRACTION 0.03   /* ~3% of edges get a negative weight */
#define MAX_WEIGHT 20                 /* magnitude range: 1..MAX_WEIGHT */

typedef struct {
    int u, v;
    int w;
} Edge;

/* --- Simple open-addressing hash set for (u,v) pair de-duplication --- */
typedef struct {
    long long *keys;
    int capacity;
} PairSet;

static PairSet pairset_create(int expectedEntries) {
    PairSet s;
    s.capacity = expectedEntries * 4 + 16;
    s.keys = (long long *)malloc(s.capacity * sizeof(long long));
    for (int i = 0; i < s.capacity; i++) s.keys[i] = -1;
    return s;
}

static int pairset_add_if_new(PairSet *s, long long key) {
    unsigned long long h = (unsigned long long)key * 2654435761ULL;
    int idx = (int)(h % (unsigned long long)s->capacity);
    while (s->keys[idx] != -1) {
        if (s->keys[idx] == key) return 0;
        idx = (idx + 1) % s->capacity;
    }
    s->keys[idx] = key;
    return 1;
}

static void pairset_free(PairSet *s) {
    free(s->keys);
}

static Edge *generate_random_edges(int V, int E) {
    Edge *edges = (Edge *)malloc(E * sizeof(Edge));
    PairSet seen = pairset_create(E);

    int count = 0;
    while (count < E) {
        int u = rand() % V;
        int v = rand() % V;
        if (u == v) continue;

        long long key = (long long)u * V + v;
        if (!pairset_add_if_new(&seen, key)) continue;

        int magnitude = 1 + rand() % MAX_WEIGHT;
        int isNegative = ((double)rand() / RAND_MAX) < NEGATIVE_EDGE_FRACTION;

        edges[count].u = u;
        edges[count].v = v;
        edges[count].w = isNegative ? -magnitude : magnitude;
        count++;
    }

    pairset_free(&seen);
    return edges;
}

static void fix_negative_cycles(int V, int E, Edge *edges) {
    long long *dist = (long long *)malloc(V * sizeof(long long));
    int changed = 1;
    int rounds = 0;

    while (changed) {
        changed = 0;
        rounds++;

        for (int i = 0; i < V; i++) dist[i] = 0;

        for (int pass = 0; pass < V - 1; pass++) {
            int any = 0;
            for (int e = 0; e < E; e++) {
                long long cand = dist[edges[e].u] + edges[e].w;
                if (cand < dist[edges[e].v]) {
                    dist[edges[e].v] = cand;
                    any = 1;
                }
            }
            if (!any) break;
        }

        for (int e = 0; e < E; e++) {
            if (dist[edges[e].u] + edges[e].w < dist[edges[e].v]) {
                int oldAbs = edges[e].w < 0 ? -edges[e].w : edges[e].w;
                edges[e].w = oldAbs + 1;
                changed = 1;
            }
        }
    }

    fprintf(stderr, "[generator] negative-cycle fix pass count: %d\n", rounds);
    free(dist);
}

static void write_bf_file(const char *path, int V, int E, Edge *edges, int source) {
    FILE *fp = fopen(path, "w");
    if (!fp) { fprintf(stderr, "Error: could not open '%s' for writing\n", path); exit(1); }

    int *degree = (int *)calloc(V, sizeof(int));
    for (int e = 0; e < E; e++) degree[edges[e].u]++;

    int *offset = (int *)malloc((V + 1) * sizeof(int));
    offset[0] = 0;
    for (int i = 0; i < V; i++) offset[i + 1] = offset[i] + degree[i];

    int *bucketNeighbor = (int *)malloc(E * sizeof(int));
    int *bucketWeight = (int *)malloc(E * sizeof(int));
    int *cursor = (int *)malloc(V * sizeof(int));
    memcpy(cursor, offset, V * sizeof(int));

    for (int e = 0; e < E; e++) {
        int u = edges[e].u;
        bucketNeighbor[cursor[u]] = edges[e].v;
        bucketWeight[cursor[u]] = edges[e].w;
        cursor[u]++;
    }

    fprintf(fp, "%d %d\n", V, E);
    for (int u = 0; u < V; u++) {
        fprintf(fp, "%d %d", u, degree[u]);
        for (int k = offset[u]; k < offset[u + 1]; k++) {
            fprintf(fp, " %d %d", bucketNeighbor[k], bucketWeight[k]);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "SOURCE %d\n", source);

    fclose(fp);
    free(degree); free(offset); free(bucketNeighbor); free(bucketWeight); free(cursor);
}

static void write_fw_file(const char *path, int V, int E, Edge *edges) {
    FILE *fp = fopen(path, "w");
    if (!fp) { fprintf(stderr, "Error: could not open '%s' for writing\n", path); exit(1); }

    long long **mat = (long long **)malloc(V * sizeof(long long *));
    for (int i = 0; i < V; i++) {
        mat[i] = (long long *)malloc(V * sizeof(long long));
        for (int j = 0; j < V; j++) mat[i][j] = (i == j) ? 0 : INF_SENTINEL;
    }

    for (int e = 0; e < E; e++) {
        mat[edges[e].u][edges[e].v] = edges[e].w;
    }

    fprintf(fp, "%d\n", V);
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (j > 0) fprintf(fp, " ");
            if (mat[i][j] >= INF_SENTINEL) fprintf(fp, "INF");
            else fprintf(fp, "%lld", mat[i][j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    for (int i = 0; i < V; i++) free(mat[i]);
    free(mat);
}

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s --bf|--fw V E outputFile [seed]\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc < 5 || argc > 6) {
        print_usage(argv[0]);
        return 1;
    }

    int isBF;
    if (strcmp(argv[1], "--bf") == 0) isBF = 1;
    else if (strcmp(argv[1], "--fw") == 0) isBF = 0;
    else { print_usage(argv[0]); return 1; }

    int V = atoi(argv[2]);
    int E = atoi(argv[3]);
    const char *outPath = argv[4];
    unsigned int seed = (argc == 6) ? (unsigned int)atoi(argv[5]) : (unsigned int)time(NULL);

    if (V <= 0 || E <= 0) {
        fprintf(stderr, "Error: V and E must be positive integers.\n");
        return 1;
    }
    long long maxPossibleEdges = (long long)V * (V - 1);
    if (E > maxPossibleEdges) {
        fprintf(stderr, "Error: E=%d exceeds max possible distinct directed edges (%lld) for V=%d.\n",
                E, maxPossibleEdges, V);
        return 1;
    }

    srand(seed);
    fprintf(stderr, "[generator] mode=%s V=%d E=%d seed=%u\n", isBF ? "bf" : "fw", V, E, seed);

    Edge *edges = generate_random_edges(V, E);
    fix_negative_cycles(V, E, edges);

    if (isBF) {
        write_bf_file(outPath, V, E, edges, /*source=*/0);
    } else {
        write_fw_file(outPath, V, E, edges);
    }

    fprintf(stderr, "[generator] wrote %s\n", outPath);
    free(edges);
    return 0;
}