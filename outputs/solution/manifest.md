# Solution suite — Ếch ham ăn (`jumping-frog`)

**Time limit:** 2 s — authoritative, from `source/problem-context.md`. No provisional limit is used
anywhere in this suite.
**Memory limit:** 512 MB — confirmed by the user during Step 2b; `source/problem-context.md` does
not state one.

All sources are self-contained GNU C++17, use `cin`/`cout` with
`ios_base::sync_with_stdio(false); cin.tie(nullptr);`, and carry the same guarded local-file block as
`source/solution.cpp` (`TASK "jumping-frog"`, `.inp`/`.out`, falling back to standard streams when
`jumping-frog.inp` is absent).

**Local build note:** testlib programs and these binaries must be built with `-static` on this
machine. Git Bash's `libstdc++-6.dll` shadows the MSYS2 one and makes any `testlib.h` program
segfault inside `registerTestlibCmd`. This is a local toolchain artefact only; it says nothing about
Polygon.

**Reference oracle** for every cross-check below: `source/solution.cpp`, validated in Step 1
(1400 randomised small cases against an independent brute force; max-constraint runs at 0.36 s).

---

## `ac-full-treap.cpp` — AC, full constraints

- **Scope:** all constraints (N, Q <= 2e5, positions up to 1e9). Serves as the differential oracle
  for every subtask.
- **Algorithm:** the fly multiset lives in a randomized balanced BST (treap) keyed by position, each
  node carrying subtree count and subtree sum. A type-2 query at x takes the prefix aggregate
  (sum, count) of flies at position <= the frog's current position via one iterative root-to-leaf
  descent, and repeats while the count grows. Each round eats a fly that sat beyond the previous
  round's position, so the position more than doubles per round, giving at most ~48 rounds.
- **Complexity:** O((N + Q) log N) for the updates plus O(log(answer) * log N) per type-2 query.
  Memory O(N + Q) nodes, about 10 MB.
- **Material difference from `source/solution.cpp`:** the source is *offline* — it reads every query
  up front to coordinate-compress all values, then uses a Fenwick tree over ranks. This one is fully
  *online*: no compression, no pre-read of the query list, a randomized BST instead of a Fenwick
  tree, and prefix aggregates obtained by tree descent rather than by low-bit decomposition. Same
  asymptotic class — the doubling loop is inherent to the problem and no materially different
  *asymptotics* exists — but a different data structure, a different input model, and independent
  arithmetic.
- **Validation:** identical output to `source/solution.cpp` on all 900 randomised small cases
  (value ranges 2, 3, 5, 10, 1000, 1e9; N <= 8, Q <= 10; heavy duplicate/collision density) and on
  both max-constraint tests: doubling-chain 683 ms, random-position 888 ms (limit 2 s). Answers up
  to ~1.0e14, all in `long long`. Re-verified in Step 7 against the full 100-test generated set:
  accepted by `outputs/checker.cpp` on all 100, slowest test 1.02 s against the 2 s limit — the
  treap's constant factor is roughly 4x the official Fenwick solution's 0.26 s peak, which is
  comfortable but is the reason it is the cross-check and not the jury solution.

## `ac-subtask-1-simulate.cpp` — AC, Subtask 1 only

- **Scope:** exactly Subtask 1 (N, Q <= 200). Not claimed anywhere else.
- **Algorithm:** literal statement simulation — per type-2 query, rescan all flies for the uneaten
  one with the largest position <= the frog's position, eat it, jump, repeat. No data structure.
- **Complexity:** O(Q * N^2) worst case (one fly per round, a full rescan per round), memory O(N).
- **Material difference:** shares nothing with either the source solution or `ac-full-treap` — no
  compression, no aggregate structure, no doubling argument; it enumerates the process the statement
  describes. Its independence from the doubling argument is the point: it validates the *semantics*
  rather than the optimisation.
- **Validation:** identical output to `source/solution.cpp` on all 900 randomised small cases. At the
  boundary of its declared domain, N = Q = 200 with every query eating every fly: 0.14 s. Immediately
  outside the domain (N = Q = 2000) it exceeds 12 s, which is why the scope is Subtask 1 and not
  Subtask 2.

---

## `wa-single-round.cpp` — WA

- **Defect:** one round of eating instead of the chain reaction. It eats every fly at position <= x,
  adds their positions, prints — missing that the jump can bring further flies within reach.
  Localized to a single deleted loop; the rest (offline compression + Fenwick of (count, sum)) is the
  correct efficient structure, so it is fast enough for the full constraints and fails on answer
  value, never on time.
- **Minimal counterexample** (verified): `3 1 / 1 2 5 / 2 2` prints `5 2`, correct is `10 3`.
- **Passes:** any test where the first sweep is already the fixpoint — sparse flies with wide
  coordinates and a small x. Random 1e9-range tests with few flies hit this often, so weak tests do
  not kill it.
- **Kill profile for Steps 5–7:** flies whose positions form a growing chain (each fly reachable only
  after the previous one is eaten) with a type-2 query starting at or below the smallest fly.

## `wa-strict-left.cpp` — WA

- **Defect:** reads "con ruồi gần nhất ở bên trái" as *strictly* to the left and uses `lower_bound`
  where `upper_bound` is required, so a fly standing exactly on the frog's square is never eaten.
  This is the exact ambiguity that was resolved in `source/problem-context.md` during Step 1, which
  makes it the most likely real contestant misread. Otherwise the full correct algorithm.
- **Minimal counterexample** (verified): `1 1 / 7 / 2 7` prints `7 0`, correct is `14 1`.
- **Passes:** every test in which no fly ever sits exactly on a position the frog occupies. With
  positions drawn uniformly from [1, 1e9] that is almost always, so this candidate is invisible to
  random large tests and needs deliberate collisions.
- **Kill profile for Steps 5–7:** small coordinate ranges and duplicated positions (Subtask 3–4
  style), plus type-2 queries whose x equals a fly position, and chains built so a jump lands exactly
  on the next fly.

## `wa-persistent-eat.cpp` — WA

- **Defect:** misses the "giả sử" note — that a type-2 query is hypothetical — and really removes the
  flies the frog ate, so every type-2 query after the first can be wrong. A type-1 query on an
  already-eaten fly reinserts it, the natural reading once flies are being removed. Otherwise the
  full correct algorithm.
- **Minimal counterexample** (verified): `1 2 / 1 / 2 1 / 2 1` prints `2 1` then `1 0`, correct is
  `2 1` twice.
- **Passes:** any test with a single type-2 query, or with type-2 queries so small they eat nothing.
- **Kill profile for Steps 5–7:** at least two type-2 queries per test, with the earlier one eating
  flies the later one still needs. Every profile with a healthy type-2 density kills it, so this is
  the cheapest candidate to cover — but a test made only of type-1 queries plus one final type-2
  query would not.

## `tle-sort-per-query.cpp` — TLE

- **Semantics:** correct for the full problem — this is not a WA in disguise. It reproduced
  `source/solution.cpp` exactly on all 900 randomised cases.
- **Bottleneck:** it recomputes everything per query. On each type-2 query it copies the position
  array, sorts it, rebuilds prefix sums, then runs the correct doubling rounds by binary search.
  O(Q * N log N), about 4e10 steps at N = Q = 2e5.
- **Classification limit:** 2 s (authoritative). Measured on worst-case shapes (many type-2 queries,
  large N):
  - N = Q = 200 (Subtask 1): 0.12 s — **passes**
  - N = Q = 2000 (Subtask 2): 0.25 s — **passes**
  - N = Q = 50000 (Subtask 5): >12 s — **TLE**
  - N = Q = 2e5 (Subtask 6): >12 s — **TLE**

  Exactly the "passes the weak subtasks, dies on the big ones" shape a contestant submits.
- **Kill profile for Steps 5–7:** large N with a high type-2 query density. Note it is *not* saved by
  small coordinates, so Subtasks 3 and 4 at full N, Q must include high-type-2-density tests, or
  those subtasks would leak full marks to it.

---

## Guidance for Steps 5–7

- **Oracle:** run `ac-full-treap` on every generated test and require agreement with
  `source/solution.cpp`. Run `ac-subtask-1-simulate` only on Subtask 1 tests.
- **Properties every profile set must contain**, derived from the candidates above:
  1. reachability chains (fly i+1 only reachable after fly i is eaten) — kills `wa-single-round`;
  2. exact collisions between fly positions and the frog's positions, including duplicated fly
     positions and small coordinate ranges — kills `wa-strict-left`;
  3. multiple type-2 queries per test with overlapping fly sets — kills `wa-persistent-eat`;
  4. large N with type-2-heavy query mixes, at every coordinate scale — kills `tle-sort-per-query`.
- **Large answers:** at least one test must drive the answer past 2^31 (easy: many flies near 1e9
  with a large x; the max tests reach ~1e14). No 32-bit candidate is shipped in this suite — signed
  overflow would be undefined behaviour, which the suite contract forbids as an intentional defect —
  but it is the single cheapest defect in real submissions, so the test set must cover it regardless.
