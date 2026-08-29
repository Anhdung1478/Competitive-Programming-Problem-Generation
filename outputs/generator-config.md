# Generator design contract — Mật thư của Dũng

Single test case. Count `x ∈ [L, R]` with `x % 15 == 0` and digit-sum `== K`; output the count
and the smallest such `x` (or `-1`). Authoritative limits from `source/problem-context.md`
(`L ≤ R ≤ 10^18`, `K ≤ 10^9`) and the confirmed lower bounds `L ≥ 1`, `K ≥ 1`.

## 1. Input schema

| field | type | repeats | legal range | relation |
|---|---|---|---|---|
| `L` | integer (`long long`) | no | `1 ≤ L ≤ 10^18` | `L ≤ R` |
| `R` | integer (`long long`) | no | `1 ≤ R ≤ 10^18` | `R ≥ L` |
| `K` | integer (`int`) | no | `1 ≤ K ≤ 10^9` | independent of `L, R` |

One line: `L R K`, space-separated. No multi-test wrapper.

Reachability facts (drives the value profiles): a number with digit-sum `K` can be divisible by
15 only if `K ≡ 0 (mod 3)` (digit-sum rule for divisibility by 3) **and** it ends in `0` or `5`.
The maximum digit-sum of any `x ≤ 10^18` is `9·18 = 162`, and the maximum digit-sum of an actual
multiple of 15 that is `≤ 10^18` is `156` (e.g. `799999999999999995`). Hence `K > 162 ⇒ count = 0`
unconditionally, and `K ≢ 0 (mod 3) ⇒ count = 0` unconditionally.

## 2. Structural invariants

- `1 ≤ L ≤ R ≤ 10^18`.
- `1 ≤ K ≤ 10^9`.
- Subtask ceilings on `R` (see §3).
- For profile `zero`: the correct answer must be `0 / -1` (enforced by construction).
- For profile `nonzero`: the correct answer must be `count ≥ 1` (a planted multiple of 15).

## 3. Subtask table

| # | score | limit | differs from full by | generation logic |
|---|---|---|---|---|
| 1 | 20% | `R < 10^6` | `R` ceiling `999999` (`D=6`) | draw `R` as a `≤6`-digit value `≤ 999999` |
| 2 | 20% | `R < 10^11` | `R` ceiling `99999999999` (`D=11`) | draw `R` as a `≤11`-digit value `≤ 99999999999` |
| 3 | 20% | `L = 1` and `R = 10^x` | `L` fixed to 1; `R` must be a power of 10 | `L = 1`; `R = 10^x`, `x = clamp(round((rate−0.70)/0.30·18), 0, 18)` |
| 4 | 20% | `R < 10^15` | `R` ceiling `999999999999999` (`D=15`) | draw `R` as a `≤15`-digit value `≤ 999999999999999` |
| 5 | 20% | none | full | draw `R ≤ 10^18` (`D=18`); may emit exact `10^18` |

`D` is the digit-count ceiling used by `rate` (see §4). Subtask 3 is the only one not obtainable by
merely shrinking `R`; it fixes `L = 1` and constrains `R` to a power of 10.

## 4. `rate` interpretation

`rate ∈ [0.70, 1.00]` controls the **magnitude of `R`** via its digit count, so higher `rate` means
larger `R` (and thus a harder instance for brute/multiples candidates). Concretely, for a subtask
with digit-count ceiling `D`:

- target digits `d = clamp(round((rate − 0.70)/0.30 · D), 1, D)`, so `rate = 0.70 ⇒ d = 1`
  (single-digit `R`) and `rate = 1.00 ⇒ d = D` (max digits) — the full size spectrum is covered;
- `R` is a uniform random `d`-digit integer (`10^(d-1) ≤ R < 10^d`), clamped to the subtask ceiling;
- when `d = D` and the ceiling is reachable (subtask 5), the profile may also emit the exact
  boundary `R = 10^18`;
- `L` is then drawn in `[1, R]` per the active profile; `K` is drawn per the value profile and does
  **not** depend on `rate`.

`rate` never breaks `L ≤ R` or the subtask ceiling, and never affects `K`.

## 5. Generator options (consumed by `outputs/gentest.cpp`)

| option | kind | values | effect |
|---|---|---|---|
| `seed` | positional | any string | first argument; feeds `rnd` via `registerGen` |
| `--subtask` | named int | `1..5` | selects the ceiling/rule from §3 |
| `--rate` | named double | `0.70..1.00` | scales `R`'s digit count per §4 |
| `--profile` | named string | `random` (default), `full`, `single`, `narrow`, `maxR`, `zero`, `nonzero`, `kill-factor5`, `kill-offbyone`, `kill-tle` | semantic generation profile (§6) |

No `--seed`: the seed is the positional argument immediately after the generator name.

## 6. Generation profiles

- `random` (default): random `L ∈ [1, R]`, random width, `K` in the "meaningful" range
  `[1, min(162, 9·d)]` (mostly `≡ 0 mod 3` to keep answers non-trivial). General coverage.
- `full`: `L = 1` (whole prefix `[1, R]`). Exercises prefix counting and the `count(L−1)=count(0)` path.
- `single`: `L = R`. Single-point range; min equals the point iff it is lucky. Exposes off-by-one.
- `narrow`: `L = R − w`, `w ∈ [1, 1000]` (or `R − R % something`). Tightens the min search.
- `maxR`: `R` at the subtask ceiling (`10^18` for subtask 5); `L` small or random. Boundary DP.
- `zero`: guarantees answer `0 / -1` — either `K ≢ 0 (mod 3)`, or `K > 9·d` (digit-sum impossible),
  or `K ∈ (162, 10^9]` (beyond any 18-digit sum). Exercises the `-1` output path.
- `nonzero`: guarantees `count ≥ 1` by planting a multiple of 15: pick `m = 15·t` in the working
  range, set `K = digitSum(m)`, and choose `L ≤ m ≤ R`. Exercises count and min on a known answer.
- `kill-factor5`: `K ≡ 0 (mod 3)`, `L` small, `R` large enough to include a small multiple of 3
  (not 5) with digit-sum `K` — e.g. `L = 1`, `K ∈ {3,6,9,12,…}`, `R ≥ K`. Exposes `wa-ignore-factor5`.
- `kill-offbyone`: `L` itself is a lucky number (a multiple of 15 with digit-sum `K`), so
  `count(L)` differs from `count(L−1)`. Exposes `wa-offbyone`.
- `kill-tle`: `L = 1`, `R` at the ceiling (`10^18` for subtask 5). Exposes `tle-multiples`
  (`O(R/15)` loop). Valid and contest-plausible (a max-size test).

## 7. Subtask-specific generation

Only subtask 3 needs non-shrinking logic: enforce `L = 1` and `R = 10^x` with
`x = round(rate·18)`, `0 ≤ x ≤ 18` (so `R ∈ {1, 10, …, 10^18}`). All other subtasks differ only by
the `R` ceiling and digit-count ceiling `D`; `L`, `K`, and width profiles apply unchanged.

## 8. Coverage plan (for the default 100-test script)

| subtask | share | profiles to hit |
|---|---|---|
| 1 (`R<1e6`) | ~15 | `random`, `full`, `single`, `narrow`, `zero`, `nonzero`, `kill-factor5`, `kill-offbyone` |
| 2 (`R<1e11`) | ~15 | `random`, `full`, `narrow`, `zero`, `nonzero`, `kill-factor5` |
| 3 (`L=1,R=10^x`) | ~10 | `full` (power-of-10 `R`), `zero` (K ≢ 0 mod 3), `nonzero` |
| 4 (`R<1e15`) | ~20 | `random`, `full`, `narrow`, `maxR`, `zero`, `nonzero`, `kill-factor5`, `kill-offbyone` |
| 5 (full) | ~40 | `random`, `full`, `single`, `narrow`, `maxR`, `zero`, `nonzero`, `kill-factor5`, `kill-offbyone`, `kill-tle` |

Rate distribution (Step 6 default): ≈50% of tests `rate ≥ 0.90`, with meaningful coverage in
`[0.70, 0.80)` and `[0.80, 0.90)`. Boundary `K` values (`1`, `156`, `162`, `163`, `171`, `10^9`)
and boundary `R` (`1`, `999999`, `10^18`, `10^18−1`) are included.

## 9. Solution-kill matrix

| candidate | failure mechanism | smallest counterexample | legal subtasks | killing profile | expected signal |
|---|---|---|---|---|---|
| `wa-ignore-factor5.cpp` | counts numbers divisible by 3 (drops the ×5 factor) | `1 15 6` → prints `2\n6`, correct `1\n15` | 1,2,4,5 | `kill-factor5` (`K ≡ 0 mod 3`, small `L`, `R ≥ K`) | checker rejects (wrong count/min) |
| `wa-offbyone.cpp` | `count(L)` instead of `count(L−1)` | `15 15 6` → prints `0\n-1`, correct `1\n15` | 1,2,4,5 | `kill-offbyone` (`L` is a lucky multiple of 15) | checker rejects (wrong count/min) |
| `tle-multiples.cpp` | `O(R/15)` iteration | `1 1000000000000000000 156` | 5 (also 4) | `kill-tle` (`L=1`, `R=10^18`) | timeout under 1s |

AC oracles for differential validation:

- subtask 1: `ac-subtask-1-brute.cpp` (scope `R < 1e6`) and `ac-full-digitdp.cpp`;
- subtasks 2,4,5: `ac-full-digitdp.cpp` (full scope);
- subtask 3: `ac-full-digitdp.cpp` (full scope) — no scoped-only AC needed.

The official `source/solution.cpp` (full digit DP) is the primary jury oracle everywhere.

**Validated killing seeds (Step 7):** every profile reliably kills its target across subtasks —
e.g. `kill-factor5`: seed `0VECDJBC07` → `1 9 9` (`wa` prints `1/9`, correct `0/-1`),
`0VECDJBC0D` → `1 69 6` (`7/6` vs `2/15`), `0VECDJBC1C` → `1 278 3` (`9/3` vs `3/30`);
`kill-offbyone`: seed `0VECDJBC08` → `30 58 3` (`0/-1` vs `1/30`); `kill-tle`: seed `KTLE1` →
`1 1000000000000000000 2` (`tle-multiples` times out >30s while the DP answer is instant).
All 100 script tests were regenerated and differentially validated against the official solution
(0 disagreements), and generator output is deterministic per seed.

## 10. Specialized generator skills

None of the structural skills (`generator-tree/graph/array/string`) apply — the input is three
integers with no array/graph/tree/string payload. `generator-number-theory` is marginally relevant
for the divisibility/multiples reasoning; load it for the `nonzero` (planted multiple of 15) and
`kill-factor5` profiles. Primary numeric sampling uses the shared `genValue` distribution strategy.

## 11. Generator assertions (`ensure` before printing)

- `1 <= L && L <= R && R <= 1000000000000000000LL`.
- `1 <= K && K <= 1000000000`.
- subtask ceiling respected (`R < 10^6` for 1, `< 10^11` for 2, `L==1 && R==10^x` for 3,
  `< 10^15` for 4, `≤ 10^18` for 5).
- `rate` within `[0.70, 1.00]`.
- profile `zero`: `K % 3 != 0 || K > 162` (guarantees answer `0 / -1`).
- profile `nonzero`: the planted `m` satisfies `L ≤ m ≤ R`, `m % 15 == 0`, `digitSum(m) == K`.
