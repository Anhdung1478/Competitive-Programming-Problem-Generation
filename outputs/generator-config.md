# Generator design contract — Ếch ham ăn (`jumping-frog`)

Authoritative sources: `source/problem-context.md`, `source/subtask.md`, `outputs/statement.txt`.
Candidate objectives: `outputs/solution/manifest.md`. Time limit 2 s, memory 512 MB.

## 1. Input schema

One test per file. **No multi-test wrapper, no `T`, no sum-over-tests constraints.**

| Field | Type | Scope | Legal range | Relation | Repeats |
|---|---|---|---|---|---|
| `N` | int | number of flies | `1 <= N <= 2e5` | — | once, line 1 |
| `Q` | int | number of queries | `1 <= Q <= 2e5` | independent of `N` | once, line 1 |
| `x_i` | int | initial position of fly `i` | `1 <= x_i <= 1e9` | duplicates allowed | `N` values, line 2 |
| `type` | int | query kind | `1` or `2` | — | once per query line |
| `u` | int | fly index moved (type 1) | `1 <= u <= N` | repeats allowed across queries | type-1 lines |
| `y` | int | new position of fly `u` (type 1) | `1 <= y <= 1e9` | may equal the old position | type-1 lines |
| `x` | int | frog's hypothetical position (type 2) | `1 <= x <= 1e9` | unrelated to any `x_i` | type-2 lines |

Output layout: `N Q` on line 1; `N` space-separated integers on line 2; then exactly `Q` query lines,
each `1 u y` or `2 x`. Single `'\n'` line endings, no trailing blank line, no surplus spaces.

## 2. Structural invariants

Every generated file must satisfy, before printing:

- line 2 holds exactly `N` integers;
- exactly `Q` query lines follow;
- every query's first token is `1` or `2`;
- every type-1 line has exactly 3 tokens, every type-2 line exactly 2;
- `1 <= u <= N` for the `N` actually printed on line 1;
- all of `x_i`, `y`, `x` within their subtask's coordinate cap (§3);
- **no ordering, distinctness, or connectivity requirement of any kind** — positions may repeat,
  may be unsorted, and a type-1 query may move a fly onto an occupied position or onto its own
  current position. All of that is legal input and must appear in the test set.

Nothing else is invariant: there is no guarantee about the number of type-2 queries (zero is legal),
and the answer to a type-2 query may be as small as `x` itself.

## 3. Subtask table

`Nmax(S)`, `Qmax(S)` = query/fly caps; `C(S)` = coordinate cap applied to **fly** positions.

| S | Score | `Nmax` | `Qmax` | `C(S)` (fly positions) | type-2 `x` cap | Differs from full by | Generator logic |
|---|---|---|---|---|---|---|---|
| 1 | 20% | 200 | 200 | 1e9 | 1e9 | size only | shrink `N`, `Q` |
| 2 | 10% | 2000 | 2000 | 1e9 | 1e9 | size only | shrink `N`, `Q` |
| 3 | 15% | 2e5 | 2e5 | **100** | 1e9 | coordinate cap on flies | draw every `x_i` and every `y` from `[1, 100]` |
| 4 | 15% | 2e5 | 2e5 | **1e5** | 1e9 | coordinate cap on flies | draw every `x_i` and every `y` from `[1, 1e5]` |
| 5 | 15% | 50000 | 50000 | 1e9 | 1e9 | size only | shrink `N`, `Q` |
| 6 | 25% | 2e5 | 2e5 | 1e9 | 1e9 | nothing | full range |

**Reading of Subtasks 3 and 4, stated explicitly because it changes the generator.** `subtask.md`
bounds *"vị trí của các con ruồi"* — the flies' positions — before and after every query. That is
`x_i` and `y`, and it says nothing about the frog's hypothetical position `x` in a type-2 query. So
`x` stays in `[1, 1e9]` in these subtasks. The generator therefore caps flies but not `x`, and the
profile mix for S3/S4 deliberately includes both `x <= C(S)` (the interesting case, where the frog
interacts with the flies) and `x` far above `C(S)` (legal, and the case where the frog eats
everything in one round). Subtasks 3 and 4 keep the **full** `N, Q` caps — they are not small
subtasks, which is exactly why `tle-sort-per-query` must not survive them.

## 4. `rate` interpretation

`rate` in `[0.70, 1.00]` scales instance magnitude within the chosen subtask. It never changes
legality and never touches the coordinate cap.

- `N = clamp(round(Nmax(S) * rate), 1, Nmax(S))`
- `Q = clamp(round(Qmax(S) * rate), 1, Qmax(S))`
- `N` and `Q` get **independent** jitter of `[0.95, 1.00]` applied on top of `rate`, so a
  `rate = 1.00` test is not the only way to reach the cap and the two are not always equal — the
  problem couples nothing between them, and a test set where `N == Q` always would miss the
  `N << Q` and `N >> Q` shapes.
- `rate` does **not** scale coordinates. Coordinate magnitude is the `profile` option's job (§5),
  because "big coordinates" and "big instance" are independent axes here: a 2e5-fly test with all
  positions in `[1, 3]` is the interesting stress for the compression step, and a 5-fly test with
  positions near 1e9 is the interesting stress for `long long` width.
- Lower bounds are structural, not scaled: `N >= 1`, `Q >= 1` always hold after clamping. With
  `rate >= 0.70` and `Nmax >= 200` the clamp is never actually reached — it is a safety net.

Degenerate minimum instances (`N = 1`, `Q = 1`) are **not** produced by low `rate`; they are their
own profile (`profile=minimum`, §6), because `rate` is defined as "fraction of the subtask cap" and
`rate = 0.005` is outside the mandated `[0.70, 1.00]` band.

## 5. Generator options

Invocation shape: `gentest <SEED> --subtask <S> --rate <R> [--profile <P>] [--qmix <M>] [--xprofile <X>]`

The seed is the **positional** first argument, consumed by `registerGen(argc, argv, 1)`. There is no
`--seed` option.

| Option | Values | Default | Effect |
|---|---|---|---|
| `subtask` | `1..6` | required | selects `Nmax`, `Qmax`, `C(S)` per §3 |
| `rate` | `0.70..1.00` | required | instance magnitude per §4 |
| `profile` | `random`, `chain`, `collide`, `dup`, `maxvalue`, `minvalue`, `cluster`, `minimum` | `random` | shape of the fly-position multiset (§6) |
| `qmix` | `balanced`, `q2heavy`, `q1heavy`, `q2only`, `q1only`, `updates-then-query` | `balanced` | proportion and ordering of query types (§6) |
| `xprofile` | `random`, `small`, `onfly`, `large`, `justabove` | `random` | distribution of the type-2 `x` (§6) |

No option ever carries a literal final input value: `profile` says *what shape*, `rate` says *how
big*, and the generator picks every concrete number. That is why there is no `--n`, no `--maxpos`,
and no `--numq2`.

## 6. Generation profiles

### `profile` — fly position multiset

| Value | Construction | Why it matters |
|---|---|---|
| `random` | each `x_i` drawn by `genValue` over `[1, C(S)]` | baseline distribution; the honest average case |
| `chain` | a reachability chain: `p_1` small, then `p_{k+1}` in `(S_k, 2*S_k]` where `S_k` is the running prefix sum, so fly `k+1` is reachable **only after** fly `k` is eaten. Padded with far-away flies once `C(S)` is reached. | the core mechanic of the problem, and the only shape that forces the maximum number of doubling rounds. Kills `wa-single-round`. |
| `collide` | positions chosen so that prefix sums land exactly on later positions: pick `p_1`, then set some `p_{k+1} = x + sum(p_1..p_k)` exactly | makes the frog land exactly on a fly, the `<=` vs `<` boundary. Kills `wa-strict-left`. |
| `dup` | 1–5 distinct values, each repeated many times | duplicate-heavy: stresses coordinate compression, the multiset semantics, and the "many flies eaten in one round" path |
| `maxvalue` | every `x_i` within `[C(S) - 10, C(S)]` | drives the answer to its maximum (~1e14 at S6), the 32-bit overflow boundary |
| `minvalue` | every `x_i` in `[1, 3]` | maximum number of flies eaten per round; the answer is dominated by count, not magnitude |
| `cluster` | a few tight clusters at random scales, gaps between them | mixed density; the frog crosses gaps only via accumulated sums |
| `minimum` | `N = 1`, `Q = 1` (overrides `rate`) | degenerate boundary; the smallest legal file |

### `qmix` — query composition

| Value | Composition | Why |
|---|---|---|
| `balanced` | ~50% type 1, ~50% type 2, interleaved | baseline |
| `q2heavy` | ~90% type 2 | maximum query-side work; the TLE shape |
| `q1heavy` | ~90% type 1 | maximum update churn; stresses erase/insert and compression size |
| `q2only` | all type 2 | pure query load, static multiset |
| `q1only` | all type 1 | **no output at all** — a legal file the checker must handle (zero answer lines) |
| `updates-then-query` | all type 1, then one final type 2 | the one shape that `wa-persistent-eat` survives, and a natural contestant blind spot |

### `xprofile` — the type-2 `x`

| Value | Draw | Why |
|---|---|---|
| `random` | `genValue` over `[1, 1e9]` | baseline |
| `small` | `[1, min(10, C(S))]` | the frog starts below almost everything: long chains, small answers |
| `onfly` | exactly the current position of a randomly chosen live fly | the `<=` boundary again, from the query side. Kills `wa-strict-left`. |
| `large` | `[1e9 - 10, 1e9]` | the frog starts above everything: one round, maximum answer |
| `justabove` | one above a live fly's position (`pos + 1`) | off-by-one probe from the other side |

## 7. Subtask-specific generation

Only two subtasks need more than shrinking:

- **Subtask 3 (`C = 100`) and Subtask 4 (`C = 1e5`).** The cap applies to fly positions at *all*
  times, so it must be enforced on both `x_i` and every type-1 `y`, not just on line 2. With
  `C = 100` and `N` up to 2e5 the positions are necessarily duplicate-heavy — the `dup` and
  `minvalue` profiles are the natural ones there, and `chain` degenerates after about 7 links
  (`1, 2, 4, 8, 16, 32, 64`) so the generator must pad the remainder with in-range values rather
  than exceeding the cap. `chain` at S3 is still worth generating: 7 real links plus padding is the
  longest chain the subtask permits, and it is what a chain-blind solution fails on there.
- **`profile=minimum`** overrides `rate` and produces `N = Q = 1`. Legal in every subtask.

No subtask restricts the query mix, so every `qmix` is legal everywhere.

## 8. Coverage plan (for the 100-test default)

| Group | Tests | Notes |
|---|---|---|
| S1 | 10 | includes 2 `minimum`, both `q1only` and `updates-then-query` once |
| S2 | 8 | includes `chain`, `collide` |
| S3 (`C=100`) | 16 | duplicate-heavy by construction; both `x <= 100` and `x` near 1e9 |
| S4 (`C=1e5`) | 16 | same shape one scale up |
| S5 | 14 | mid-size, all profiles represented |
| S6 | 36 | the scoring group and the TLE/overflow group; every profile × the adversarial `qmix` values |

Rate distribution across all 100 (the mandated shape): ~50 tests at `rate >= 0.90`, meaningful
coverage in `[0.80, 0.90)` and `[0.70, 0.80)`. Boundary tests (`profile=minimum`, `maxvalue`,
`minvalue`, `q1only`) appear in at least two subtasks each. Every subtask carries at least one
`chain`, one `collide`, and one `q2heavy` test.

## 9. Solution-kill matrix

Concrete seeds are recorded after generator validation in Step 7; the row's profile is the contract,
the seed is the receipt.

| Candidate | Mechanism | Options that kill it | Kill rate | Signal | Verified seed |
|---|---|---|---|---|---|
| `wa-single-round.cpp` | eats only the first round; misses the chain reaction | `--profile chain --xprofile small`, and most non-degenerate tests | 77/100; every subtask 8/10, 7/8, 11/16, 8/16, 13/14, 30/36 | wrong answer (both numbers too small) | `gentest 0DW9X1B6A5 --subtask 1 --rate 0.95 --profile chain --qmix q2heavy --xprofile small` (test 003) |
| `wa-strict-left.cpp` | `lower_bound` for `<=`; never eats a fly on the frog's own square | `--profile collide`, `--xprofile onfly`, and every `--subtask 3` test (collisions are near-certain at `C = 100`) | 65/100; every subtask 5/10, 5/8, 10/16, 8/16, 11/14, 26/36 | wrong answer | `gentest 9WARXOVB65 --subtask 1 --rate 0.98 --profile collide --qmix q2heavy --xprofile onfly` (test 004) |
| `wa-persistent-eat.cpp` | really removes eaten flies; type-2 is not hypothetical | any `--qmix q2heavy` / `q2only` test with overlapping reachable sets | 83/100; every subtask 7/10, 7/8, 13/16, 13/16, 13/14, 30/36 | wrong answer from the 2nd type-2 line on | `gentest 0DW9X1B6A5 --subtask 1 --rate 0.95 --profile chain --qmix q2heavy --xprofile small` (test 003) |
| `tle-sort-per-query.cpp` | re-sorts and rebuilds prefix sums per type-2 query, `O(Q N log N)` | large `N` with type-2-heavy queries, at every coordinate scale | passes S1 (0.01 s) and S2 (0.03 s) as documented; **>6 s** (limit 2 s) on S3, S4, S5, S6 | timeout | S3 `gentest OV0BQKK78W --subtask 3 --rate 1.00 --profile random --qmix q2heavy --xprofile small` (test 019); S4 `M7ZWF41LY0` (035); S5 `N1IQRI4GNP` (051); S6 `WT5QVPRNKL` (065) |
| *(no shipped candidate)* | 32-bit position/sum overflow | `--profile maxvalue --xprofile large`, and any large S5/S6 test | largest jury answer over the whole 100-test set is **199 223 732 698 925** (~2.0e14), 92 800x past `2^31` | wrong answer for any `int`-width submission | n/a — no UB candidate is shipped, the coverage is what matters |

**Differential oracles.** `source/solution.cpp` is the jury answer for every test (`> $` in the
script). `outputs/solution/ac-full-treap.cpp` cross-checks every test in every subtask.
`outputs/solution/ac-subtask-1-simulate.cpp` cross-checks **Subtask 1 tests only** — outside
`N, Q <= 200` it exceeds the limit and must never be run there.

## 10. Specialized skills

- `generator-array` — the fly positions are a numeric multiset with adversarial value ordering,
  duplicates, and coordinate extremes; `genValue` and the duplicate/extreme profiles come from there.
- `generator-number-theory` — **not** applicable. No primality, divisibility, or gcd structure is
  involved; the doubling chain is a prefix-sum construction, not a number-theoretic one.
- `generator-tree`, `generator-graph`, `generator-string` — not applicable; the input has no graph,
  tree, or string.

## 11. Generator assertions

Before printing, `gentest.cpp` must `ensure(...)`:

- `1 <= n && n <= Nmax(S)` and `1 <= q && q <= Qmax(S)`;
- `1 <= x_i && x_i <= C(S)` for every initial position;
- for every query: `type == 1 || type == 2`;
- for type 1: `1 <= u && u <= n` and `1 <= y && y <= C(S)`;
- for type 2: `1 <= x && x <= 1e9`;
- the number of query lines emitted equals `q`, and the number of positions emitted equals `n`;
- `C(S)` matches the subtask table (`100` for S3, `1e5` for S4, `1e9` otherwise) — asserted from the
  subtask number, not from a duplicated literal;
- for `profile=chain`: every constructed link is `<= C(S)`, and the construction stops rather than
  clamping into an out-of-range value;
- for `profile=collide`: the collision target is `<= C(S)`, else fall back to a plain random value;
- `subtask` is one of `1..6` and `rate` is within `[0.70, 1.00]` — a script typo must fail loudly
  rather than silently generate an out-of-subtask test.
