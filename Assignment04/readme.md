# Assignment 4 — Vertex Coloring & PageRank (individual task)

## Repository Overview

This repository contains the **individual** assignments for CS509 (First-Year M.Tech CSE, 2026).
Assignment 4's individual task covers two algorithms operating on separate CSR graph inputs —
**Vertex Coloring** (greedy, Welsh-Powell ordering) and **PageRank** (iterative rank propagation)
— each implemented and timed independently.

## Student Details

| Field | Value |
|---|---|
| Name | VAISHALI SINGH |
| Entry Number | 2026AIM1022 |
| Repository | https://github.com/Vaishali-AIM/CS509_AIM1022 |


## Algorithm / Approach

**Vertex Coloring** — Welsh-Powell greedy heuristic. Vertex degrees are computed
from the CSR representation, vertices are ordered by non-increasing degree, then
each is assigned the smallest color index not already used by a colored neighbor.
`driver/vc_driver.c` also runs a hand-written input validator ahead of CSR
conversion — rejecting self-loops, parallel edges, mismatched degree/neighbor
counts, out-of-range vertex IDs, and non-symmetric adjacency lists.

**PageRank** — Iterative, all vertices simultaneously updated each round using
the previous iteration's values: `PR(v) = (1-d)/N + d * sum(PR(u)/outdegree(u))`
over incoming edges. Dangling vertices (outdegree 0) have their rank
redistributed evenly across all vertices to avoid divide-by-zero. Stops when
total absolute rank change <= `TOLERANCE`, or `MAX_ITERATIONS` is reached.

**CSR Graph Conversion (reused, not reimplemented)** — Both drivers read their
input via `readAdjacencyListAsCSR()` from `../utilities/csr.c`, the single
canonical copy carried over from Assignment 1, per the assignment's instruction
to reuse rather than duplicate.

## Input Format, Assumptions, and Constraints

**Vertex Coloring** — undirected, unweighted:
```
V E
u0 degree neighbor1 neighbor2 ...
...
```
Each edge appears in both endpoints' adjacency lists; no self-loops.

**PageRank** — directed, unweighted, with trailing parameters:
```
V E
u0 outdegree neighbor1 neighbor2 ...
...
DAMPING d
TOLERANCE epsilon
MAX_ITERATIONS n
```
Only outgoing edges are listed per vertex.

## Source Files and Test Files

- **`utilities/csr.c` / `csr.h`** — reused CSR reader (Assignment 1).
- **`src/vc.c` / `vc.h`** — greedy vertex coloring (`vertexColoring`, `isValidColoring`).
- **`driver/vc_driver.c` / `vc_driver.h`** — input validation, CSR conversion,
  timed coloring call, formatted output.
- **`src/pr.c` / `pr.h`** — PageRank algorithm.
- **`driver/pr_driver.c` / `pr_driver.h`** — CSR conversion, timed PageRank call, formatted output.
- **`tests/`** — required-size graphs (`color_10.txt` … `color_100000.txt`,
  `pagerank_10.txt` … `pagerank_50000.txt`).

## Compilation and Execution

```bash
cd Assignment04
make vc         # builds driver/vc_driver
make pagerank   # builds driver/pr_driver
./driver/vc_driver tests/color_10.txt
./driver/pr_driver tests/pagerank_10.txt
```

## Timing Methodology

Timer starts immediately before the call to the algorithm (`vertexColoring()`
for VC, the PageRank iteration loop for PageRank) and stops immediately after
it returns. File reading, input validation, and CSR conversion all happen
before the timer starts and are excluded, per the assignment's timing rule.
For PageRank, all iterations of the rank-update loop are included in the
timed section, as required.

## Vertex Coloring Results Table

| File | V | E | Colors Used | Valid? | Time | Status |
|---|---|---|---|---|---|---|
| color_10.txt | 10 | 30 | 5 | true | 0.013957 ms | Completed Successfully |
| color_100.txt | 100 | 300 | 5 | true | 0.012836 ms | Completed Successfully |
| color_10000.txt | 10,000 | 30,000 | 6 | true | 1.995894 ms | Completed Successfully |
| color_50000.txt | 50,000 | 150,000 | 6 | true | 6.627531 ms | Completed Successfully |
| color_100000.txt | 100,000 | 300,000 | 6 | true | 13.420136 ms | Completed Successfully |

## PageRank Results Table

| File | V | E | Damping | Sum of Ranks | Iterations | Time | Status |
|---|---|---|---|---|---|---|---|
| pagerank_10.txt | 10 | 30 | 0.85 | ~1.000 | 11 | 0.002 ms | Completed Successfully |
| pagerank_100.txt | 100 | 300 | 0.85 | ~1.000 | 14 | 0.019 ms | Completed Successfully |
| pagerank_1000.txt | 1,000 | 3,000 | 0.85 | ~1.000 | 15 | 0.111 ms | Completed Successfully |
| pagerank_10000.txt | 10,000 | 30,000 | 0.85 | ~1.000 | 15 | 1.701 ms | Completed Successfully |
| pagerank_50000.txt | 50,000 | 150,000 | 0.85 | ~1.000 | 15 | 12.261 ms | Completed Successfully |

*(100,000-vertex tests are optional per the spec; if attempted and it fails —
core-dump, OOM, excessive runtime — document that outcome here instead of
omitting the row.)*

## Known Limitations

- **VC connectivity/format assumptions:** `validateVertexColoringFile` in
  `vc_driver.c` rejects self-loops and non-symmetric edges, but does not
  separately check whether the graph is connected — disconnected VC inputs
  are still valid per the spec (isolated vertices just get colored
  independently), so this is expected behavior, not a bug.
- **PageRank dangling-vertex handling:** rank from outdegree-0 vertices is
  redistributed evenly across *all* vertices each iteration; this matches one
  of the two spec-permitted strategies (the alternative being "treat as
  linking to all vertices," which is mathematically equivalent here).

## Time and Space Complexity

**Vertex Coloring**
- Time: O(V log V) for the degree sort, O(V + E) for the coloring pass —
  overall O(V log V + E).
- Space: O(V + E) — CSR arrays, color array, per-vertex `used[]` scratch array.

**PageRank**
- Time: O((V + E) × iterations).
- Space: O(V + E) — CSR arrays, two rank arrays (current/previous iteration).

## References
- Welsh-Powell heuristic: https://graphstream-project.org/doc/Algorithms/Welsh-Powell
- PageRank: https://web.stanford.edu/class/cs106m/meetings/03-pagerank