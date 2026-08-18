# Generator configuration — Đếm đường đi

## 1. Authoritative input schema

There is exactly one test case.

```text
r c
grid[1]
...
grid[r]
```

- `r` and `c` are integers with `1 <= r, c <= 2087`.
- Each `grid[i]` is a token of exactly `c` characters from the alphabet `.` and `#`.
- `grid[1][1] = grid[r][c] = '.'`.
- There are no spaces inside a grid row and no fields after the last row.
- A move lands at displacement `(0,1)`, `(1,0)`, `(1,3)`, or `(3,1)`. Only the landing cell must be open; long moves do not inspect intermediate cells.

## 2. Subtask limits

| `subtask` | Score | Row limit | Column limit |
|---:|---:|---:|---:|
| 1 | 50 | 8 | 7 |
| 2 | 50 | 2087 | 2087 |

Every generated test must satisfy the selected row and column limits. Subtask 2 is the full constraint set, not a disjoint group.

## 3. Command-line parameters

Canonical invocation:

```text
gentest SEED --subtask S --rate R --profile P
```

- `SEED`: required first positional argument; controls `testlib` randomness and reproducibility. There is no named `--seed` option.
- `subtask`: required integer, either `1` or `2`.
- `rate`: required real number in `[0.70, 1.00]`.
- `profile`: required string from the profile table below.

No option specifies literal `r`, `c`, a grid row, or an obstacle count.

## 4. Interpretation of `rate`

For a selected subtask with limits `Rmax` and `Cmax`, define:

```text
Rscaled = max(1, round(Rmax * rate))
Cscaled = max(1, round(Cmax * rate))
```

Large profiles choose dimensions close to these scaled maxima. `boundary` chooses the scaled maxima exactly. `open`, `near-open`, `random`, `dense`, and `checkerboard` normally choose each dimension from roughly 85% through 100% of its scaled maximum, with occasional narrow dimensions to exercise boundary recurrences.

Structural micro-profiles (`minimum` and `long-only`) intentionally override scale: their purpose is to isolate semantic edge cases, so `rate` remains a reproducibility/test-plan classification rather than forcing a large board. `zero-answer` uses dimensions at least `4 x 4` and otherwise follows the scaled limits.

## 5. Profiles

| Profile | Construction and purpose |
|---|---|
| `minimum` | Internally choose dimensions in `[1, min(4, scaled limit)]`, including `1x1`, one-row, and one-column grids. Mix all-open grids and legal blocked cells. Covers initialization, unreachable narrow boards, and endpoint overlap. |
| `open` | All cells are `.`; dimensions are near the scaled maximum. Maximizes the number of paths and kills exponential enumeration. |
| `near-open` | Near-scaled dimensions with independently blocked cells at a low rate of about 2%–10%; endpoints are reopened. Preserves large branching while perturbing uniform structure. |
| `random` | Near-scaled dimensions with obstacle probability selected in about 15%–55%; endpoints are reopened. General coverage. |
| `dense` | Near-scaled dimensions with about 70%–92% blocked cells; endpoints are reopened. Covers sparse reachability, many zero DP states, and accidental assumptions about connectivity. |
| `checkerboard` | Alternating/short-period `.` and `#` row strings, optionally with one phase disruption; endpoints are reopened. Exercises periodic structure rather than only uniform random strings. |
| `long-only` | Generate either `2x4` or `4x2`, mark only start and target open, and block every other cell. Exactly one long move reaches the target while ordinary right/down DP returns zero. |
| `zero-answer` | Use at least `4x4`; reopen endpoints, then block every legal successor of `(1,1)` that is inside the grid. The answer must be zero. |
| `boundary` | Dimensions equal the rate-scaled maxima. Choose an internal pattern among open, long runs, alternating rows, and random obstacles. At `rate = 1.00`, this reaches the exact subtask maxima. |

The grid is a rectangular family of strings over the exact alphabet `.#`. String-pattern generation therefore includes repeated characters, alternating strings, short periods, long runs, and one disruptive cell, not only independent uniform characters.

## 6. Numeric generation strategy

Except for fixed structural profiles, numeric choices use a shared `genValue(low, high, mode)` helper with modes biased toward minima, maxima, boundaries, or uniform values. Dimension generation must not rely on unexplained constants in `gentest.cpp`; the ranges and probabilities are defined above.

Obstacle percentages are selected once per generated grid and then applied per cell. Endpoints are always explicitly restored to `.` after pattern construction.

## 7. Adversarial coverage and candidate-kill matrix

| Risk/candidate | Required profiles | Expected observation |
|---|---|---|
| `wa-standard-moves-only.cpp` omits `(1,3)` and `(3,1)` | `long-only` is mandatory in both subtasks; also `open` | Correct answer differs; on `long-only`, official answer is `1` and candidate answer is `0`. |
| `tle-naive-dfs.cpp` has exponential recomputation | Full-subtask `open`, `near-open`, and `boundary`, preferably with `rate >= 0.95` | Timeout under the authoritative 0.5-second limit. |
| Off-by-one in long predecessors | Narrow grids, `minimum`, `long-only`, `checkerboard` | Differential mismatch near rows/columns 1, 2, 4. |
| Incorrect blocked-cell semantics | `dense`, `zero-answer`, disrupted periodic grids | Nonzero value through a blocked landing cell or incorrect reachability. |
| Overflow/no modulo | Large full-subtask `open` grids | Output diverges from the modulo oracle. |

At least half of all commands use `rate >= 0.90`; both subtasks receive all semantic micro-profiles. Large open/near-open full tests are mandatory, not probabilistic accidents.

## 8. Test-set distribution goals

- Exactly 100 commands.
- 35 commands for Subtask 1 and 65 for Subtask 2.
- Exactly 15 rates in `[0.70, 0.80)`, 35 in `[0.80, 0.90)`, and 50 in `[0.90, 1.00]`.
- Distinct positional seeds on every command.
- Mix profiles and subtasks throughout the file rather than placing one homogeneous family in a single block.
- Include multiple exact `rate = 1.00` `boundary`/`open` tests for maximum dimensions.

## 9. Required invariants and assertions

Before printing, `gentest.cpp` must assert:

- selected `subtask`, `rate`, and `profile` are valid;
- `1 <= r <= Rmax` and `1 <= c <= Cmax`;
- exactly `r` rows exist and every row has exactly `c` characters;
- every character belongs to `.#`;
- the first and last cells are `.`;
- profile-specific guarantees (`long-only` direct path, `zero-answer` blocked successors) hold.

## 10. Applicable generator skills

- Generic Polygon `testlib.h` generator rules.
- String generation for the rectangular `.#` row tokens, especially repeated, alternating, periodic, long-run, and disrupted patterns.
- No tree, general-graph, permutation/array, or number-theory generator is required.

## 11. Validated candidate kills

- All 12 scripted `long-only` tests make `source/solution.cpp` print `1` and `wa-standard-moves-only.cpp` print `0`.
- `PATH075X93925 --subtask 2 --rate 1.00 --profile open` exceeded the authoritative 0.5-second limit with `tle-naive-dfs.cpp` in local validation.
- The same seed and options regenerate byte-identical input.
