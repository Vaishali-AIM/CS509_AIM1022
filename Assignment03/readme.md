# Assignment 03 — Minimum Spanning Tree: Kruskal's Algorithm and Prim's Algorithm

**Type:** Individual

**Objective:** Implement both Kruskal's and Prim's algorithms to find a Minimum
Spanning Tree (MST) of a connected, weighted, undirected graph represented in
CSR format, run on the same input files so results and execution times can be
compared directly.

## Algorithm / Approach

**Kruskal's Algorithm** — Greedy. Extracts all undirected edges from the CSR
graph, sorts them by weight, then adds each edge in increasing order if its
two endpoints are in different components (checked via Disjoint Set Union
with path compression + union by rank, implemented inline in `kruskal.cpp`).
Stops once V−1 edges are selected.

**Prim's Algorithm** — Also greedy, but grows one tree outward from vertex 0.
Repeatedly pulls the cheapest edge crossing from the tree to a vertex outside
it, using a `std::priority_queue` min-heap (lazy Prim: stale/duplicate heap
entries are skipped on pop rather than removed eagerly).

**CSR Graph Conversion (reused, not reimplemented)** — Both algorithms read
their input via `readAdjacencyListAsCSR()` from `csr.h`/`csr.cpp` (carried
over from Assignment 1), not duplicated here.

## Input Format, Assumptions, and Constraints

```
V E
u0 degree neighbor1 weight1 neighbor2 weight2 ...
...
u(V-1) degree neighbor1 weight1 neighbor2 weight2 ...
```
- `V` vertices, `E` undirected edges (each edge appears once per endpoint's
  adjacency list, same weight both sides).
- No `SOURCE` line — MST considers the whole graph.
- Edge weights may be positive, zero, or negative integers per spec.
- Vertices are numbered 0 to V−1, listed in that order.
- The graph is assumed connected; **connectivity is not currently validated**
 

## Source Files and Test Files

- **`csr.h` / `csr.cpp`** — reused CSR reader/writer (Assignment 1).
- **`mst_common.h` / `mst_common.cpp`** — shared `MSTEdge`, `MSTResult`,
  `freeMSTResult()`, used by both algorithms so neither depends on the other.
- **`kruskal.h` / `kruskal.cpp`** — `kruskalMST(graph)`; DSU is a private
  struct inside this file, not a separate module.
- **`prim.h` / `prim.cpp`** — `primMST(graph)`, starting from vertex 0.
- **`tests/`** — required-size graphs (`mst_10.txt` … `mst_100000.txt`) plus
  hand-built edge cases (already-a-tree, star, complete graph, tied weights,
  negative/zero weights, disconnected, isolated vertex, malformed header,
  out-of-range neighbor).

`driver.cpp` (CLI dispatch to Kruskal/Prim by name, per-algorithm timing,
formatted output) is not yet built.

## Compilation and Execution (current state)

```bash
g++ -O2 -std=c++17 -o test_both csr.cpp mst_common.cpp kruskal.cpp prim.cpp test_kruskal_driver.cpp
./test_both tests/mst_10.txt
```

## MST Results Table

| File | V | E | Kruskal Wt. | Prim Wt. | Kruskal Time | Prim Time | Equal? | Status |
|---|---|---|---|---|---|---|---|---|
| mst_10.txt | 10 | 30 | 1439 | 1439 | 0.002504 ms | 0.0047 ms | Yes | Pass |
| mst_100.txt | 100 | 300 | 19375 | 19375 | 0.030760 ms | 0.023936 ms | Yes | Pass |
| mst_10000.txt | 10,000 | 30,000 | 1979187 | 1979187 | 2.6065 ms | 6.090810 ms | Yes | Pass |
| mst_50000.txt | 50,000 | 150,000 | 9778124 | 9778124 | 13.3007 ms | 31.0759 ms | Yes | Pass |
| mst_100000.txt | 100,000 | 300,000 | 19684915 | 19684915 | 23.4098 ms | 66.2026 ms | Yes | Pass |



## Known Limitations

- **Connectivity not validated:** a disconnected input currently produces a
  silent partial spanning forest (`edgeCount < V-1`) instead of an error.
  `mst_invalid_disconnected.txt` / `mst_invalid_isolated_vertex.txt` exist in
  `tests/` but aren't caught by any check yet — this needs to land in either
  `csr.cpp` or `driver.cpp`.

## Time and Space Complexity

**Kruskal's Algorithm**
- Time: O(E log E) — dominated by sorting the edge list.
- Space: O(V + E) — CSR arrays, extracted edge list, DSU parent/rank arrays.

**Prim's Algorithm**
- Time: O(E log V) — each edge relaxation may push/pop the heap, O(log V) each.
- Space: O(V + E) — CSR arrays, `inMST` array, heap (bounded by E under lazy deletion).