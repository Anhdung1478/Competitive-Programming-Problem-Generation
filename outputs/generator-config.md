# Generator design contract

## 1. Input schema

The input contains exactly one test case.

| Field | Type | Scope | Legal range | Relations | Repeated |
|---|---|---|---|---|---|
| `n` | 32-bit integer | whole test | $1 \le n \le 10^5$ | array length | once |
| `q` | 32-bit integer | whole test | $1 \le q \le 10^5$ | number of following operations | once |
| `a_i` | 32-bit integer | position $i$ | $1 \le a_i \le 10^4$ | every value remains at most $10^4$ | $n$ times |
| operation | string | operation | `add` or `count` | case-sensitive | $q$ times |
| `l, r` | 32-bit integers | operation | $1 \le l \le r \le n$ | inclusive interval | $q$ times |
| `d` | 32-bit integer | `add` only | $1 \le d \le 10^4$ | range maximum plus $d$ must respect the subtask value cap | per `add` |

There is no test-count field and no cross-test sum constraint.

## 2. Structural invariants

- Print `n q`, exactly $n$ initial values, then exactly $q$ operations.
- Values are positive and updates only increase them.
- After every `add`, every array value is at most the selected subtask cap.
- Every interval is nonempty and inclusive.
- Every generated test contains at least one `count`, so the expected output is nonempty.
- Repeated values, repeated intervals, and repeated query answers are legal.

Maintain the current range maximum with an internal lazy range-maximum structure while generating operations. Choose `d` only from $[1,\text{cap}-\max(a_l,\ldots,a_r)]$. If this interval is empty, emit `count` or select another range instead.

## 3. Subtasks

| Subtask | Score | Restrictions | Generator enforcement |
|---|---:|---|---|
| 1 | 20% | $n,q\le1000$ | Clamp both size ceilings to 1000; use the full value cap $10^4$. |
| 2 | 24% | no `add` | Emit only `count`; use the full size and value bounds. |
| 3 | 26% | all values always at most 100 | Set the value cap to 100 and validate every update against it. |
| 4 | 30% | no extra restriction | Use the full bounds. |

The scores sum to 100%.

## 4. Interpretation of `rate`

Require `0.70 <= rate <= 1.00`.

For ordinary profiles, let `maxN` and `maxQ` be the selected subtask limits and set:

- `nCeil = max(1, floor(maxN * rate))`;
- `qCeil = max(1, floor(maxQ * rate))`.

Use the shared `genValue` distribution to choose `n` and `q` in `[1,nCeil]` and `[1,qCeil]`, normally biased toward their ceilings. High-load profiles choose exactly or very near these ceilings. `min-case` and `wrong-killer` deliberately use tiny legal instances regardless of `rate`; their purpose is semantic edge coverage, not load scaling.

Scale the density of large intervals and the requested number of operations with `rate`. Never use `rate` to relax the value cap, subtask restrictions, interval validity, or the requirement that at least one `count` exists.

## 5. Generator options

- Positional seed: first argument after `gentest`, consumed by `registerGen`; never expose `--seed`.
- `--subtask <S>`: required integer in `[1,4]`.
- `--rate <R>`: required real in `[0.70,1.00]`.
- `--profile <name>`: required semantic profile.

Allowed general profiles:

- `min-case`, `random`, `threshold-hit`, `threshold-cross`, `multi-threshold`;
- `staggered-thresholds`, `mixed-duplicates`, `point-ops`, `full-range`;
- `nested-ranges`, `alternating-ranges`, `query-heavy`, `update-heavy`;
- `all-lucky`, `no-lucky`, `boundary-values`, `wrong-killer`, `max-work`.

Allowed static Subtask 2 profiles:

- `static-random`, `static-all-lucky`, `static-no-lucky`, `static-mixed`;
- `static-duplicates`, `static-point`, `static-full`, `static-nested`;
- `static-boundary`.

Allowed Subtask 3 profiles:

- `small-random`, `small-all-lucky`, `small-no-lucky`;
- `small-threshold-hit`, `small-threshold-cross`, `small-point`;
- `small-full-range`, `small-nested`, `small-query-heavy`;
- `small-update-heavy`, `small-duplicates`, `small-staggered`, `small-boundary`.

Every name used by `test-script.txt` is included above. `query-heavy`, `wrong-killer`, and `max-work` also have subtask-aware implementations.

## 6. Generation profiles

- `min-case`: $n=q=1$, initial value 4, and `count 1 1`.
- `random` / `small-random`: mixed valid values, intervals, updates, and counts.
- `all-lucky` / `small-all-lucky`: initialize from lucky numbers allowed by the cap; mix point and range counts with safe updates.
- `no-lucky` / `small-no-lucky`: initialize away from lucky numbers and include updates that remain non-lucky when possible.
- `threshold-hit`: place values immediately below lucky numbers and add exactly the missing distance.
- `threshold-cross`: place values near lucky numbers and add enough to pass the next threshold.
- `multi-threshold`: use large safe additions that pass several lucky numbers at once.
- `staggered-thresholds`: distribute distances $0,1,2,\ldots$ to nearby lucky numbers so different positions cross at different operations.
- `mixed-duplicates` / `small-duplicates`: long runs of equal lucky and non-lucky values.
- `point-ops` / `small-point`: mostly single-position updates and counts, including $l=r$ at both array ends.
- `full-range` / `small-full-range`: mostly `[1,n]` operations.
- `nested-ranges` / `small-nested`: shrinking and expanding nested intervals.
- `alternating-ranges`: alternate disjoint left/right ranges and full-range queries.
- `query-heavy` / `small-query-heavy`: at least 90% `count` operations.
- `update-heavy` / `small-update-heavy`: emit as many valid updates as the value budget permits, while reserving counts at the beginning, between update phases, and at the end.
- `boundary-values` / `small-boundary`: emphasize 1, the cap, lucky numbers, and immediate lucky neighbors.
- `static-*`: the corresponding value/interval pattern with only `count` operations.
- `wrong-killer`: emit a tiny case whose correct output is nonempty (at minimum `a_1=4`, `count 1 1`). In Polygon's standard-input execution, `source/wrong-solution.cpp` unconditionally redirects to `luckynumber.inp` and therefore produces no participant tokens; the checker rejects it.
- `max-work`: choose near-maximum `n,q`. For Subtasks 1, 3, and 4, use broad unit updates that repeatedly cross lucky thresholds plus many full-range counts. For Subtask 2, use near-maximum static queries. This checks the intended solution against the 2-second limit.

## 7. Subtask-specific generation

- Subtask 1 uses all general profiles but clamps $n,q$ to 1000.
- Subtask 2 rejects every profile that requires an update. Static profiles, `query-heavy`, `wrong-killer`, `max-work`, and `min-case` emit only `count`.
- Subtask 3 uses cap 100. Its lucky set is `{4,7,44,47,74,77}`; every update is chosen using the current range maximum so no value exceeds 100.
- Subtask 4 uses the full cap 10000 and all general profiles.
- `multi-threshold` is used only where the chosen cap permits crossing multiple lucky values.

## 8. Coverage plan

`test-script.txt` has exactly 100 tests allocated by score: 20/24/26/30 tests for Subtasks 1/2/3/4. Rate distribution is exactly:

- 15 tests in `[0.70,0.80)`;
- 35 tests in `[0.80,0.90)`;
- 50 tests in `[0.90,1.00]`.

Each subtask contains minimum, random or mixed, boundary, duplicate, point/full/nested interval, high-load, and explicit wrong-solution-killing coverage where applicable. Every test has at least one `count`; therefore every test has a nonempty jury output and rejects the redirected empty output of `wrong-solution.cpp`.

## 9. Specialized generator skills

Use `generator-array` with `polygon-gentest`. Array value distributions, duplicates, lucky-number neighbors, runs, and adversarial interval patterns are central. No tree, graph, string, or number-theory generator skill is needed.

## 10. Generator assertions

Before printing, enforce:

- `1 <= n,q <=` the selected subtask limits;
- exactly $n$ initial values and exactly $q$ operations;
- every initial and current value lies in `[1,valueCap]`;
- every operation has `1 <= l <= r <= n`;
- every `add` has `1 <= d <= 10000` and preserves the value cap;
- Subtask 2 has zero `add` operations;
- Subtask 3 has maximum value at most 100 initially and after every update;
- at least one `count` operation exists;
- all profile-specific promises hold;
- `wrong-killer` has a nonempty correct output.
