# Solution suite manifest

## Authoritative limit

The authoritative time limit is **1 second**, from `source/problem-context.md`. All candidates use GNU C++17 and preserve the guarded `tunnel.inp` / `tunnel.out` local-file behavior of `source/solution.cpp`, with standard input/output as fallback.

## Candidates

### `ac-full-maximin-then-dijkstra.cpp` — AC

- **Scope:** full constraints (subtask 4 and every earlier subtask).
- **Algorithm:** a max-priority traversal computes, for each city, the maximum bottleneck height reachable from `X`. A second Dijkstra run keeps only roads whose height is at least the optimum for `Y`, minimizes total length, and stores parents.
- **Complexity:** `O((N + M) log N)` time and `O(N + M)` memory.
- **Material difference from the official source:** it computes the bottleneck directly with a maximin Dijkstra traversal instead of binary-searching the height and running Dijkstra for every feasibility check.
- **Validation evidence:** compiled with GNU C++17 and strict warnings; matched the official solution semantically on 1,200 random simple graphs, including disconnected graphs and `X = Y`. Step 1 independently checked the official solution against a brute oracle on 2,504 cases.
- **Supported domain:** all valid inputs, including `M = 0`, unreachable `Y`, and `X = Y`.

### `ac-subtask-1-tree-dfs.cpp` — AC

- **Scope:** exactly subtask 1 (the graph is a connected tree).
- **Algorithm:** iterative DFS records parents; the unique path from `X` to `Y` is reconstructed from the parent array. Since there is only one simple path, it is automatically optimal under both criteria.
- **Complexity:** `O(N + M)` time and `O(N + M)` memory.
- **Material difference from the official source:** it uses only the unique-path property and performs no threshold search or shortest-path computation.
- **Validation evidence:** compiled with GNU C++17 and strict warnings; matched the official solution semantically on 500 random trees, including `X = Y`.
- **Supported domain:** subtask 1 only; it must not be used as an oracle for other subtasks.

### `wa-shortest-path-first.cpp` — WA

- **Scope claimed by the candidate:** full constraints.
- **Attempt:** one Dijkstra run minimizes total length over every road and reconstructs that path.
- **Complexity:** `O((N + M) log N)` time and `O(N + M)` memory.
- **Precise defect:** it optimizes length before height, reversing the required lexicographic priority. It ignores every road height.
- **Counterexample:**

  ```text
  4 1 4
  4
  1 2 5 100
  2 4 5 100
  1 3 4 1
  3 4 4 1
  ```

  The candidate prints `1 3 4`, whose `(height, length)` is `(4, 2)`. The required path is `1 2 4`, whose objective is `(5, 200)` because height has priority.
- **Validation evidence:** the counterexample was executed and rejected by `outputs/checker.cpp`.
- **Generation target:** profile `competing-paths`, especially a short low tunnel path versus a longer high tunnel path. This profile is valid in subtasks 2–4 and can also be embedded in a tree only when the two routes are not both present.

### `tle-enumerate-thresholds.cpp` — TLE

- **Scope:** logically correct for full constraints.
- **Algorithm:** sort all distinct road heights descending and run Dijkstra independently for each threshold until `Y` becomes reachable.
- **Complexity:** `O(U (N + M) log N)` time and `O(N + M)` memory, where `U` is the number of distinct heights (`U <= 10,000`).
- **Bottleneck:** thousands of nearly full Dijkstra runs when many height values occur and `Y` becomes reachable only at a low threshold.
- **Classification limit:** the authoritative 1-second limit.
- **Validation evidence:** compiled with GNU C++17 and strict warnings; matched the official solution semantically on 150 random small graphs. A valid `N = 100,000`, `M = 99,999` long-chain instance with 10,000 height levels did not finish within 3 seconds.
- **Generation target:** profile `threshold-chain`, using a long path, many distinct/repeated height levels, and a final height-1 edge. Apply near the subtask-4 limit; small versions are also suitable for subtask 2 correctness coverage.

## Oracle routing

| Scope | Differential oracle |
|---|---|
| Subtask 1 | `ac-subtask-1-tree-dfs.cpp` and `ac-full-maximin-then-dijkstra.cpp` |
| Subtasks 2–4 | `ac-full-maximin-then-dijkstra.cpp` |

All AC comparisons are semantic because multiple optimal paths may exist. `outputs/checker.cpp` independently recomputes the optimum and validates the submitted witness rather than comparing it structurally with the jury path.
