# Generator design contract — Mật mã lăng mộ cổ

Design contract for `outputs/test-script.txt` and `outputs/gentest.cpp`. Everything the
generator does must be traceable to a section here.

## 1. Input schema

| Field | Type | Scope | Legal range | Relation | Repeats |
|---|---|---|---|---|---|
| `T` | integer | first line | `1 <= T <= 20` | — | once |
| `L` | decimal string, no leading zero | line `i+1`, token 1 | `1 <= L <= 10^10000` | `L <= R` | per query |
| `R` | decimal string, no leading zero | line `i+1`, token 2 | `1 <= R <= 10^10000` | `R >= L` | per query |

There is **no** sum-of-lengths constraint across the `T` queries: every query may carry two
10001-digit numbers, so the maximal input is ~400 KB. This is the intended worst case, not an
accident, and the generator must be able to produce it.

`|X|` denotes the digit count of `X`. `10^10000` has 10001 digits; every other legal value has
at most 10000.

## 2. Structural invariants

Enforced before printing, for every query:

- `L` and `R` consist only of characters `0`-`9`;
- neither has a leading zero (`X[0] != '0'`), and neither is empty;
- `L <= R` compared numerically (length first, then lexicographically);
- `R` is within the active subtask's cap (Section 3);
- exactly two tokens per line, separated by one space, one `'\n'` per line, no trailing blank line;
- `1 <= T <= 20` and the first line holds `T` alone.

## 3. Subtask table

| # | Score | Limit | Differs from full | Generator logic |
|---|---|---|---|---|
| 1 | 20% | `R <= 10^5` | numeric cap, tiny | pick `|R| in [1,6]`; if `|R| = 6` the only legal value is `100000` |
| 2 | 20% | `R <= 10^11` | numeric cap, fits in 64 bit | pick `|R| in [1,12]`; if `|R| = 12` the only legal value is `100000000000` |
| 3 | 20% | `L = 1`, `R = 10^x`, `0 <= x <= 10^4` | **shape-constrained**, cannot be produced by shrinking | emit `L = "1"`, `R = "1" + x` zeros; profile options are ignored, only `x` varies |
| 4 | 20% | `|R| <= 500` | digit-length cap | pick `|R| in [1,500]`; no special top value (`10^500` is out of scope, `499` nines is not) |
| 5 | 20% | full | — | pick `|R| in [1,10001]`; if `|R| = 10001` the only legal value is `10^10000` |

Subtask 3 is the only subtask that is *not* a restriction of the general random shape; see
Section 7.

## 4. `rate` interpretation

`rate in [0.70, 1.00]` is a single knob for "how close to the subtask ceiling". It is mapped
onto the **digit length** of `R` (never onto the value directly, because the value range is
astronomically wide) and onto `T`:

```
span(maxLen) = clamp(round(maxLen * (rate - 0.60) / 0.40), 1, maxLen)
```

so `rate = 0.70 -> 25%` of the ceiling, `0.80 -> 50%`, `0.90 -> 75%`, `1.00 -> 100%`.

| Quantity | Mapping | Clamp |
|---|---|---|
| `|R|` (subtasks 1, 2, 4, 5) | `span(maxLen)` for that subtask's `maxLen` = 6 / 12 / 500 / 10001 | `>= 1`; the top length is legal only for the exact power of ten |
| `x` (subtask 3) | `clamp(round(10000 * (rate - 0.60) / 0.40), 0, 10000)` | `[0, 10000]` |
| `T` (default) | `clamp(round(20 * (rate - 0.50) / 0.50), 1, 20)` -> `0.70 -> 8`, `0.90 -> 16`, `1.00 -> 20` | `[1, 20]` |

`rate` never breaks a lower bound: after scaling, `|R|` is clamped to `>= 1`, `|L|` is chosen
in `[1, |R|]`, and the numeric `L <= R` check is re-applied and repaired (Section 11) rather
than trusted.

## 5. Generator options

| Option | Form | Values | Effect |
|---|---|---|---|
| seed | positional, immediately after `gentest` | any token | `registerGen` reproducibility seed. Never written as `--seed`. |
| `subtask` | `--subtask S` | `1..5` | selects the row of Section 3 |
| `rate` | `--rate R` | `0.70..1.00` | Section 4 |
| `profile` | `--profile P` | see Section 6 | digit-content shape of each query |
| `numtest` | `--numtest N` | `1..20`, optional | overrides the `T` derived from `rate`; used only for the `T = 1` degenerate tests |

## 6. Generation profiles

Each profile describes the *digit content* of one query. `--profile mixed` draws a fresh
profile per query from the list below.

| Profile | Shape | Why it matters |
|---|---|---|
| `random` | `L`, `R` independent uniform digit strings, reordered so `L <= R`; `\|R\|` uses the largest *non-reserved* length so this stays the heaviest shape | baseline and the actual worst case for the intended DP; both tight flags fall away immediately, exercising the free part in bulk |
| `equal` | `L = R`, uniform digits | single number: answer is exactly that number's power/purity, so any range-decomposition bug shows |
| `prefix` | `L` and `R` share a random common prefix of ~half the length, then `L` continues with small digits and `R` with large ones | keeps `tightLow`/`tightHigh` alive deep into the number — the only shape that populates the tight DP states in bulk |
| `wa-trap` | `L = R = '9' + '1' * (k-1)`, and digit variants `'d' + 'c' * (k-1)` with `d > c` | power is `k-1` while the maximum power over `[1, R]` is `k`; see Section 9 |
| `flat` | all digits equal (`ddd...d`) | power `= k`, purity `= 1`: the maximum-length / minimum-count extreme |
| `decreasing` | non-increasing digit runs, e.g. blocks of `9`s then `8`s ... | drives the power far below `k` while the counts stay large |
| `zeros` | numbers of the form `d * 10^m` and digit strings that are mostly `0` | the leading-zero / `nonz` logic and the "long run of zeros is itself the best subsequence" case |
| `boundary` | subtask extremes, cycled deterministically over the queries of one test: `L = R = 1`, `L = 1` with `R` at the cap, `L = R` at the cap. This is the only profile that emits a number of maximal digit length (the exact power of ten). | exact edges of every bound, with all three shapes guaranteed to appear |

## 7. Subtask-specific generation

**Subtask 3 (`L = 1`, `R = 10^x`)** cannot be produced by shrinking a random shape. The
generator must special-case it: emit the literal `1`, then `1` followed by `x` zeros, with `x`
from Section 4. `--profile` is ignored for this subtask (the shape is fully determined), and
`x = 0` (`R = 1`) must appear at least once.

**Top-of-range values.** For subtasks 1, 2 and 5 the ceiling is a power of ten, so a number of
maximal digit length is legal *only* if it equals that power exactly. The generator picks
`|R| = maxLen` only when it intends to emit that exact power of ten; otherwise it uses
`maxLen - 1` digits.

**Subtask 4** caps digits, not value, so `499`/`500` nines are legal and are the real edge.

## 8. Coverage plan

100 generated tests, 20 per subtask. Per subtask:

- 2 `boundary` (one of them `--numtest 1`);
- 3 `equal`;
- 3 `prefix`;
- 2 `random`;
- 2 `mixed`;
- 2 `wa-trap`;
- 2 `flat`;
- 2 `decreasing`;
- 2 `zeros`.

Within each subtask the two `rate = 1.00` slots are given to `random` and `prefix`, so the
largest test of every subtask is also its most adversarial one.

Rate distribution across all 100 tests: exactly 50 tests at `rate >= 0.90` (so the near-limit
shapes dominate), 35 in `[0.80, 0.90)`, 15 in `[0.70, 0.80)`, with representative values at
`0.70`, `0.80`, `0.90`, `0.95`, `0.99` and `1.00`. Subtask 3 ignores the profile column above
(its shape is fully determined by `x`); its two `boundary` tests cycle `x` over
`0, 1, 2, 10000, 9999`, which is what guarantees the `x = 0` edge, and the remaining tests
spread `x` across the same rate buckets.

## 9. Solution-kill matrix

| Candidate | Mechanism | Smallest known counterexample | Legal in | Profile / options | Tests | Signal |
|---|---|---|---|---|---|---|
| `wa-prefix-subtract.cpp` | answers `[L,R]` as `g(R)` minus `g(L-1)`; when every number attaining `maxPower(1..R)` is below `L`, it prints that too-large power with count `0` | `T=1`, `L=R=21` -> prints `2 0`, correct `1 2` | subtasks 1, 2, 4, 5 | `--profile wa-trap`, also hit by `equal`, `decreasing`, `flat`, `prefix`, `random` | **48 of the 100 generated tests kill it** | wrong answer, rejected by `ncmp` |
| `tle-enumerate.cpp` | enumerates every integer in `[L,R]`; `O(T * (R-L+1) * k^2)` | `T=1`, `L=1`, `R=10^11` still running after 5 s | subtasks 2-5 (deliberately *not* subtask 1) | any profile whose range width exceeds ~`10^7` | passes all 20 subtask 1 tests (worst 289 ms), exceeds 3 s on tests 025, 041, 045, 069, 081, 091, 097 and every other wide-range test | timeout against the 1 s limit |

**Confirmed killing seeds** (from the generated set, after generator validation):

- `wa-prefix-subtract.cpp`: tests 001, 003, 006-008, 010, 011, 013, 014, 017, 018, 020, 021, 025-027, 029-032, 035, 037, 038, 040 (subtasks 1-2), 062-064, 069-073, 075-078 (subtask 4), 081, 083-087, 089, 091, 095, 096, 098, 100 (subtask 5).
- `wa-prefix-subtract.cpp` **cannot** be killed by any subtask 3 test, and this is structural rather than a coverage gap: subtask 3 fixes `L = 1`, so `g(L-1) = g(0) = (0, 0)` and the subtraction degenerates into the correct answer. Subtasks 1, 2, 4 and 5 cover it instead.
- `tle-enumerate.cpp`: any subtask 2-5 test whose range is wide; the two `boundary --numtest 1` tests (021, 061) are narrow by design and let it finish, which is harmless.

`tle-enumerate.cpp` is expected to **pass** subtask 1 (measured 460 ms on the maximal subtask 1
batch). Subtask 1 tests must therefore not be strengthened against it — earning subtask 1 is its
documented scope.

**Differential oracles.** `ac-full-forward-digitdp.cpp` is validated over the full domain and is
the oracle for every subtask; `source/solution.cpp` produces the jury answers (`> $`). No
subtask-scoped AC exists, so no scope restriction applies when cross-checking. The two agree
byte-for-byte on all 100 generated tests.

**Measured cost of the generated set** (local machine, `-O2`): `source/solution.cpp` peaks at
~1.02 s on test 097 (`--subtask 5 --rate 0.94 --profile random`) against the 1 s limit;
`ac-full-forward-digitdp.cpp` runs the same test in 0.34 s. See the risk note in the final
summary.

## 10. Specialized skills

`generator-string` — the payload is two decimal strings over the alphabet `0`-`9` with run,
repetition, periodicity and prefix-sharing structure. `generator-number-theory` is *not*
needed: the only arithmetic structure required is powers of ten, which Section 7 pins exactly.
No tree/graph/array skill applies.

## 11. Generator assertions

Before printing, `gentest.cpp` must `ensure(...)`:

- `1 <= T <= 20`;
- for every query: `L` and `R` are non-empty, all characters in `'0'..'9'`, `X[0] != '0'`;
- `|L| <= |R|`, and `L <= R` under (length, lexicographic) comparison;
- `|R| <= maxLen(subtask)`, and if `|R| == maxLen(subtask)` for subtasks 1/2/5 then `R` equals
  the exact power of ten (`"1"` followed by `maxLen-1` zeros);
- subtask 4: `|R| <= 500`;
- subtask 3: `L == "1"` and `R == "1" + x*'0'` with `0 <= x <= 10000`.

`L <= R` is **repaired by swapping**, not by resampling in a loop, so no profile can spin.
