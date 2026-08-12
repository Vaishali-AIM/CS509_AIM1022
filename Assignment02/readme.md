# Assignment 2 — Bellman-Ford & Floyd-Warshall (individual task)

## Repository Overview

This repository contains the **individual** assignments for CS509 (First-Year M.Tech CSE, 2026).
Assignment 2's individual task is **Shortest path algorithm** — implemented through two different algorithms, bellman ford and floyd warshall 

## Student Details

| Field | Value |
|---|---|
| Name | VAISHALI SINGH |
| Entry Number | 2026AIM1022 |
| Repository | `CS509_AIM1022` |

## Language and Environment

| Field | Value |
|---|---|
| Language | C |
| Compiler | GCC — (Ubuntu 15.2.0-16ubuntu1) 15.2.0 |
| OS / Machine | Ubuntu-WSL |

## Build & Run

```bash
cd Assignment02
make            # builds both ./bellman_ford and ./floyd_warshall
./bellman_ford tests/bf_10.txt
./floyd_warshall tests/fw_10.txt
```

## Timing methodology

Timer starts immediately before the call to `bellman_ford()` / `floyd_warshall()`
and stops immediately after it returns — this includes the negative-cycle
check for both algorithms, since that's part of each algorithm's defined
procedure. File reading, parsing, and (for Bellman-Ford) adjacency-list-to-CSR
conversion all happen before the timer starts and are excluded, per the
assignment's timing rule.

## CSR conversion

CSR conversion is pulled from `../utilities/csr.c` — the single canonical
copy used across all assignments, not duplicated here, per the assignment
instruction to reuse the Assignment 1 CSR function rather than re-implement
it.

## Test data generation

Test graphs (other than the small worked examples) were produced with
`tools/generate_graph.c`, a random graph generator that guarantees no
negative-weight cycle exists anywhere in the generated graph (verified via
a Bellman-Ford-style detect-and-fix pass using a virtual-source technique,
so the check isn't limited to cycles reachable from a single vertex).

```bash
cd tools
gcc -Wall -Wextra -O2 -o generate_graph generate_graph.c
./generate_graph --bf V E outputFile [seed]
./generate_graph --fw V E outputFile [seed]
```

## 11.1 Bellman-Ford / Floyd-Warshall Results Table

| Algorithm | Test File | Vertices | Edges | Source | Negative Cycle | Status | Time |
|---|---|---|---|---|---|---|---|
| Bellman-Ford | bf_10.txt | 10 | 15 | 0 | No | Pass | 0.0004 ms |
| Bellman-Ford | bf_100.txt | 100 | 300 | 0 | No | Pass | 0.0100 ms |
| Bellman-Ford | bf_10000.txt | 10,000 | 30,000 | 0 | No | Pass | 1.7901 ms |
| Bellman-Ford | bf_50000.txt | 50,000 | — | — | — | **Not generated — see note below** | — |
| Bellman-Ford | bf_100000.txt | 100,000 | — | — | — | **Not generated — see note below** | — |
| Floyd-Warshall | fw_10.txt | 10 | 15 | N/A | No | Pass | 0.0020 ms |
| Floyd-Warshall | fw_100.txt | 100 | 300 | N/A | No | Pass | 0.5157 ms |
| Floyd-Warshall | fw_500.txt | 500 | 1,200 | N/A | No | Pass | 30.8559 ms |
| Floyd-Warshall | fw_1000.txt | 1,000 | 2,500 | N/A | No | Pass | 261.7339 ms |
| Floyd-Warshall | fw_2000.txt | 2,000 | 5,000 | N/A | No | Pass | 1979.2154 ms |

"Expected Output" for randomly generated graphs is "valid finite distances
with no negative cycle" (there's no hand-computable exact answer to check
against at this scale); "Actual Output" for each run