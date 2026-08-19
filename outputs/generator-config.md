# Generator configuration — Các đường hầm

This file is the design contract for `test-script.txt` and `gentest.cpp`.

## 1. Normalized input schema

There is exactly one test case.

| Field | Type | Scope and order | Legal range | Relations |
|---|---|---|---|---|
| `N` | integer | first token of line 1 | `1..100000` | number of vertices |
| `X`, `Y` | integer | remaining tokens of line 1 | `1..N` | equality is allowed |
| `M` | integer | only token of line 2 | `0..100000` | also `M <= N(N-1)/2` because the graph is simple |
| `i`, `j` | integer | first two tokens of each of the next `M` lines | `1..N` | `i != j`; unordered pairs are unique |
| `H` | integer | third road token | `1..10000` | tunnel height limit |
| `D` | integer | fourth road token | `1..10000` | positive road length |

## 2. Structural invariants

- The graph is undirected and simple: no self-loop and no repeated unordered pair.
- The graph need not be connected; in particular, `Y` may be unreachable from `X`.
- Vertex labels are exactly in `1..N`.
- Exactly `M` road records are printed.
- There is no test-count field and no cross-test sum constraint.
- `X = Y` is legal and its answer is the one-vertex path.

## 3. Subtasks

| ID | Score | Limits/restrictions | Generator enforcement |
|---|---:|---|---|
| 1 | 15 | connected tree, `M=N-1` | construct a tree; never use disconnected or two-route profiles |
| 2 | 20 | `N,M <= 200` | cap both quantities at 200; all global invariants remain active |
| 3 | 25 | every `D=1` | overwrite/generated road lengths with 1 |
| 4 | 40 | full constraints | no additional restriction |

## 4. Interpretation of `rate`

`rate` is in `[0.70,1.00]` and determines the size envelope within the selected subtask.

- `scaledN = clamp(round(Nmax * rate), 1, Nmax)` and `scaledM = clamp(round(Mmax * rate), 0, Mmax)`.
- Size-oriented profiles (`random-sparse`, `near-limit`, `threshold-chain`) choose sizes close to these envelopes.
- `tree-*` profiles set `M=N-1` after choosing `N`; the equality is never scaled independently.
- `dense` first chooses `M` near `scaledM`, then chooses the smallest useful `N` whose simple-graph capacity can hold that many edges. Thus density is not destroyed by blindly scaling `N`.
- `minimum`, `x-equals-y`, and small handcrafted adversarial cores may deliberately choose below the envelope; `rate` remains reproducibility metadata and controls any filler appended around the core.
- All derived sizes are clamped again to the selected subtask and to `N(N-1)/2`.

## 5. Generator options

Invocation shape:

```text
gentest <positional-seed> --subtask <S> --rate <R> --profile <P>
```

- `subtask`: one of `1`, `2`, `3`, `4`.
- `rate`: real value in `[0.70,1.00]`.
- `profile`: one of the profiles below. The generator rejects a profile/subtask combination that cannot satisfy that subtask.
- The positional seed is consumed by `registerGen(argc, argv, 1)` and is never exposed as `--seed`.

No literal `N`, `M`, `H`, or `D` option is allowed in the script.

## 6. Generation profiles

| Profile | Legal subtasks | Construction and purpose |
|---|---|---|
| `minimum` | 1–4 | smallest legal cases, including `N=1,M=0`, two isolated vertices, and one-road graphs when the selected subtask permits them |
| `x-equals-y` | 1–4 | choose `X=Y`; surround it with a valid graph of profile-appropriate size |
| `disconnected` | 2–4 | place `X` and `Y` in distinct components; include isolated and two-large-component variants |
| `tree-random` | 1,4 | random recursive tree with shuffled labels/edge order |
| `tree-path` | 1,4 | long path, stressing reconstruction and distance accumulation |
| `tree-star` | 1,4 | high-degree hub with varied boundary weights |
| `tree-balanced` | 1,4 | binary-parent tree, covering shallow balanced depth |
| `tree-broom` | 1,4 | long path feeding a high-degree hub, mixing depth and degree stress |
| `random-sparse` | 2–4 | random simple graph with `M=O(N)` where limits permit |
| `dense` | 2–4 | choose enough vertices for `M` near its envelope, enumerate unique candidate pairs, and sample many of them |
| `height-ties` | 1–4 | use a small set dominated by `H=1` and `H=10000`, creating many equal bottlenecks |
| `distance-ties` | 1–4 | many `D=1` or repeated lengths; in subtask 3 every length is 1 |
| `competing-paths` | 2–4 | embed a short low-height route and a longer high-height route between `X` and `Y`; filler must not create a route with a better objective |
| `threshold-chain` | 1–4 | make the only `X-Y` route a long chain; cycle through all height values `1..10000` and force its final edge to height 1 |
| `near-limit` | 1–4 | sizes near the rate envelope, mixed boundary/random weights, valid simple structure |

Numeric generation uses `minimum`, `uniform`, `logarit`, and `maximum` distributions from the shared `genValue` strategy. Boundary profiles explicitly include `1` and `10000` for both `H` and `D` (except subtask 3, where `D=1`).

## 7. Subtask-specific logic

- **Subtask 1:** construct exactly one of the tree shapes and guarantee connectedness. `minimum` uses the one-vertex tree. `x-equals-y`, `height-ties`, `distance-ties`, `threshold-chain`, and `near-limit` are implemented on top of a tree. Dedicated shapes cover path, star, balanced, broom, and random trees. `disconnected`, `dense`, `random-sparse`, and `competing-paths` are rejected for this subtask unless the named profile has a dedicated tree interpretation listed above.
- **Subtask 2:** both `N` and `M` remain at most 200 even for `rate=1.00`.
- **Subtask 3:** every construction assigns `D=1`, including adversarial cores and filler roads.
- **Subtask 4:** full profile set and full limits.

## 8. Coverage plan for 100 tests

Allocate exactly 15/20/25/40 tests to subtasks 1/2/3/4, matching their scores. Across all tests use 15 rates in `[0.70,0.80)`, 35 in `[0.80,0.90)`, and 50 in `[0.90,1.00]`.

- Boundary/degenerate (`minimum`, `x-equals-y`, `disconnected`): about 14 tests.
- Tree shapes and reconstruction: about 17 tests.
- Random sparse/dense graphs: about 30 tests.
- Height/distance tie patterns: about 14 tests.
- `competing-paths`: at least 10 tests spread over subtasks 2–4.
- `threshold-chain`: at least 8 tests, including near-limit subtask 1, 3, and 4 instances.
- Remaining near-limit tests mix boundary weights and adversarial degree distributions.

## 9. Solution-kill matrix

| Candidate | Failure mechanism | Adversarial profile | Legal subtasks | Planned coverage | Expected signal |
|---|---|---|---|---|---|
| `wa-shortest-path-first.cpp` | minimizes distance before height | `competing-paths`: direct/short low route versus longer high route | 2–4 | at least 10 script lines | checker rejection for nonmaximum bottleneck |
| `tle-enumerate-thresholds.cpp` | runs Dijkstra for up to 10,000 thresholds | `threshold-chain`: long unique path, many height levels, final edge `H=1` | 1–4 | at least 8 lines, with near-limit instances in 1,3,4 | timeout under the authoritative 1s limit |

Concrete Step 7 evidence:

- Every `competing-paths` script seed kills `wa-shortest-path-first.cpp`: `TUNNEL056X`, `TUNNEL060X`, `TUNNEL063X`, `TUNNEL066X`, `TUNNEL072X`, `TUNNEL075X`, `TUNNEL077X`, `TUNNEL078X`, `TUNNEL079X`, `TUNNEL082X`, `TUNNEL084X`, and `TUNNEL085X`.
- `TUNNEL086X --subtask 4 --rate 1.00 --profile threshold-chain` did not finish `tle-enumerate-thresholds.cpp` within 2 seconds, already twice the authoritative limit. The independent manual full-size chain also exceeded 3 seconds.

### Differential oracles

| Subtask | Oracle(s) |
|---|---|
| 1 | `ac-subtask-1-tree-dfs.cpp`, cross-checked with `ac-full-maximin-then-dijkstra.cpp` |
| 2–4 | `ac-full-maximin-then-dijkstra.cpp` |

The official source remains the jury oracle for all subtasks; comparisons are semantic through `checker.cpp` when paths differ structurally.

## 10. Specialized generator skill

Use both `generator-graph` and `generator-tree`. The final generator remains self-contained and adapts the relevant line, star, binary-balanced, random, and broom parent constructions from the tree reference. No array, string, or number-theory helper is needed.

## 11. Generator assertions

Before printing, `gentest.cpp` must ensure:

- valid subtask id, rate, and profile/subtask combination;
- `1 <= N <= Nmax`, `0 <= M <= Mmax`, and `M <= N(N-1)/2`;
- `X,Y` are in `1..N`;
- every endpoint is in range and differs from its mate;
- every unordered endpoint pair occurs once;
- every `H,D` lies in `1..10000`;
- subtask 1 has exactly `N-1` edges and is connected;
- subtask 2 has `N,M <= 200`;
- subtask 3 has `D=1` on every edge;
- the emitted road-vector size equals `M`.
