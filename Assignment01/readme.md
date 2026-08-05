# CS509 Laboratory Repository

## Repository Overview

This repository contains the **individual** assignments for CS509 (First-Year M.Tech CSE, 2026).
Assignment 1's individual task is **GEMM (General Matrix Multiplication)** — implemented as both
a Simple (naive nested-loop) version and a Blocking (tiled) version, along with a CSR graph
conversion helper used by the buddy tasks.

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
| Compiler |  g++ (GCC) 13.2.0 |

| OS / Machine | <e.g. Ubuntu 24.04, x86_64> |
| Timing Method | <e.g. std::chrono::high_resolution_clock> |

## Directory Structure

```
CS509_<EntryNumber>/
|-- README.md                     <- this file
|-- common_wrapper/
|   `-- wrapper.cpp
|-- assignment_01/
|   |-- src/                      <- GEMM (simple + blocking) and CSR helper implementation
|   |-- driver/                   <- driver program for assignment 1
|   |-- tests/
|   |   |-- gemm_test_perf_200x200x200.txt
|   |   |-- gemm_test_perf_350x350x350.txt
|   |   |-- gemm_test_perf_500x500x500.txt
|   |   |-- gemm_test_perf_700x700x700.txt
|   |   `-- malformed/            <- invalid-input test cases (error-handling)
|   `-- outputs/                  <- captured run logs (optional)
`-- ...
```

## Common Wrapper: Build and Usage

```bash
cd common_wrapper
g++ -O2 -std=c++17 -o wrapper wrapper.cpp
./wrapper
```

The wrapper presents a menu to select an assignment, compile it, and run either a single test
file or all test files for that assignment. See `common_wrapper/wrapper.cpp` for details.

## Assignment 01 - GEMM (Simple + Blocking) and CSR Graph Helper

### Assignment Mode

Individual.

### Objective

Implement General Matrix Multiplication (C = A x B) two ways — a direct/simple nested-loop
version and a cache-blocked (tiled) version — and compare their execution times on the same
inputs. Also implement a helper function that converts an adjacency list into CSR
(Compressed Sparse Row) format, for use by the buddy graph tasks (BFS/DFS/SSSP).

### Algorithm / Approach

- **Simple GEMM:** direct triple nested loop over i (rows of A), j (columns of B), k (shared
  dimension), accumulating `C[i][j] += A[i][k] * B[k][j]`.
- **Blocking GEMM:** matrices are divided into `BLOCK_SIZE x BLOCK_SIZE` tiles; the same
  accumulation is performed tile-by-tile so that data reused within a tile stays resident in
  cache, reducing memory traffic for larger matrices.
- **CSR conversion:** adjacency list -> `row_ptr`, `col_idx`, and (for weighted graphs) `values`
  arrays. Conversion time is excluded from any reported algorithm timing (not applicable to GEMM
  itself, but shared code is used by the buddy tasks).

Both GEMM implementations are required to produce identical result matrices for every test case.

### Input Format

```
M K N
A row 0 values
A row 1 values
...
A row M-1 values
B row 0 values
B row 1 values
...
B row K-1 values
```

A is M x K, B is K x N, result C is M x N. All values are space-separated integers.

### File Structure

| File | Purpose |
|---|---|
| `src/gemm_simple.cpp` | Simple GEMM implementation |
| `src/gemm_blocking.cpp` | Blocking GEMM implementation |
| `src/csr_helper.cpp` | Adjacency-list to CSR conversion helper |
| `driver/driver.cpp` | Reads input file, calls both GEMM versions, times and prints results |
| `tests/*.txt` | Test input files (see result table below) |

### Compilation

```bash
cd assignment_01
g++ -O2 -std=c++17 -o driver driver/driver.cpp src/gemm_simple.cpp src/gemm_blocking.cpp -Isrc
```

### Execution

```bash
# run a single test file
./driver gemm tests/gemm_test_perf_200x200x200.txt

# run all test files in the tests/ directory
./driver gemm --all tests/
```

*(Adjust the above to match your actual driver's argument format.)*

### Test Cases and Result Table

**Large-scale performance comparison tests**

| Test File | Size (M=K=N) | Multiply-adds | Simple Time (ms) | Blocking Time (ms) | Block Size | Speedup | Status |
|---|---|---|---|---|---|---|---|
| gemm_test_perf_200x200x200.txt | 200 | 8,000,000 | | | | | |
| gemm_test_perf_350x350x350.txt | 350 | 42,875,000 | | | | | |
| gemm_test_perf_500x500x500.txt | 500 | 125,000,000 | | | | | |
| gemm_test_perf_700x700x700.txt | 700 | 343,000,000 | | | | | |

**Malformed / invalid-input tests (error-handling)**

| Test File | What's Wrong | Expected Driver Behaviour | Actual Behaviour | Status |
|---|---|---|---|---|
| gemm_test_21_empty_file.txt | File is completely empty | Clear error, no crash | | |
| gemm_test_22_missing_rows.txt | B matrix rows missing | Clear parsing/EOF error | | |
| gemm_test_23_row_too_short.txt | A row has fewer values than K | Clear format error | | |
| gemm_test_24_row_too_long.txt | A row has more values than K | Clear format error | | |
| gemm_test_25_non_numeric_token.txt | Non-numeric token in place of integer | Clear parse error | | |
| gemm_test_26_zero_dimension.txt | M declared as 0 | Documented graceful handling or clear rejection | | |
| gemm_test_27_negative_dimension.txt | M declared as -2 | Clear rejection, no invalid memory allocation | | |

### Complexity

| Algorithm | Time Complexity | Space Complexity |
|---|---|---|
| Simple GEMM | O(M x K x N) | O(M x N) for output (plus O(M x K + K x N) input) |
| Blocking GEMM | O(M x K x N) (same asymptotic work, better constant factor via cache reuse) | O(M x N) for output |

### References

- <fill in any references used, e.g. course slides, textbook sections, blog posts on cache blocking>