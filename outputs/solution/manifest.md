# Solution suite — Mật mã lăng mộ cổ

Time limit used for classification: **1 s** (authoritative, from `source/problem-context.md`).
Memory limit: 512 MB. All sources compile clean with `g++ -std=gnu++17 -O2 -Wall -Wextra -Wshadow`
and use the guarded `TASK "lis"` `.inp`/`.out` block plus `cin`/`cout`, matching `source/solution.cpp`.

Notation: `n = |R|` (number of digits of `R`, up to 10001), `T <= 20`.

## `ac-full-forward-digitdp.cpp` — AC, full constraints

- **Algorithm.** Forward (prefix) digit DP with a rolling layer. State after consuming a
  prefix is `(last, nonz, tightLow, tightHigh)`: `last` = last digit taken into the
  non-decreasing subsequence (0 also encodes "nothing taken yet", which is safe because
  every digit is `>= 0`), `nonz` = a nonzero digit has appeared, and the two tight flags
  pin the prefix to `L`/`R`. The value is the pair `(length so far, number of
  (prefix, chosen index set) pairs reaching that length)`, merged by max-then-sum.
  `L` is left-padded with zeros to `|R|`, so shorter numbers are covered.
- **Complexity.** `O(T * n * 10 * 8 * 10 * 2)` time, `O(1)` extra memory (two 80-entry layers).
- **Difference from `source/solution.cpp`.** The source is a *backward* (suffix) top-down
  memoised recursion over `dp[id][last][nonz][biggerL][smallerR]` holding a 6.4 MB table and
  recursing 10001 frames deep. This candidate reverses the DP direction, accumulates the
  length in the state value instead of on the way out of the recursion, is fully iterative
  (no stack-depth exposure), and uses O(1) memory.
- **Validation.** Identical output to `source/solution.cpp` and to an independent
  `O((R-L+1) * k^2)` brute force on 309 randomised/edge ranges within `[1, 20000]`
  (single points, `L = R`, ranges spanning digit-length boundaries, `1 1`, `1 9`, `1 10`,
  `9 11`, `1 20000`, `999 1001`, `11111 11111`). Identical to the official solution on three
  worst-shape 20-query 10000-digit batches. Also verified `1 10^10000` (10001-digit `R`,
  Subtask 3 maximum) against the official solution.
- **Measured worst case.** 356 ms (20 queries, 10000 random digits each) versus 996 ms for
  `source/solution.cpp` on the same machine and input.
- **Oracle role.** Differential oracle for every subtask scope.

Only one correct solution is shipped. Every correct approach to this problem is the same
digit DP over `(position, last digit taken, leading-zero flag, two tightness flags)` carrying
`(max length, count)`; the counting requirement rules out a value-only or greedy/patience
formulation, because states with equal flags but different prefixes cannot be merged unless
`last` is part of the state. A second file would therefore differ only cosmetically, so per
AGENTS.md this decision is recorded here instead.

## `wa-prefix-subtract.cpp` — WA, full constraints

- **Intended idea.** Compute `g(x) = (maxPower, purity)` over `[1, x]` with a correct
  one-sided digit DP (`(last, nonz, tightHigh)`), then answer `[L, R]` as `g(R)` combined
  with `g(L-1)`: keep `g(R)` when its power is strictly larger, otherwise subtract the two
  counts. Big-integer decrement of `L` is implemented correctly.
- **Defect.** The subtraction is invalid exactly when every number attaining
  `maxPower(1..R)` lies strictly below `L`. The real answer then has a *smaller* power, but
  the candidate still reports `maxPower(1..R)` with the difference of the counts, which is
  `0`. This is the trap the editorial calls out.
- **Counterexample (minimal).** `T = 1`, `L = R = 21`. Candidate prints `2 0`; correct is
  `1 2` (the digits `2, 1` give power 1, and each single digit is one witness).
  Other confirmed kills: `30 30` -> `2 0` vs `1 2`, and 4 of the 309 stress ranges.
- **Test target.** Any test whose interval sits entirely above the last number attaining the
  global maximum power — narrow high ranges such as `[21, 21]`, `[30, 32]`, `[98, 99]`, and
  their many-digit analogues (`L` and `R` sharing a long non-increasing prefix).

## `tle-enumerate.cpp` — TLE, semantically correct on full scope

- **Algorithm.** Enumerate every integer in `[L, R]` by decimal-string increment and run the
  textbook `O(k^2)` counting LIS on its digits.
- **Complexity.** `O(T * (R - L + 1) * k^2)` — linear in the *value* of the range, so it is
  unbounded on the real constraints.
- **Bottleneck / classification.** Measured on the 1 s limit: the maximal Subtask 1 batch
  (20 queries of `1 100000`) finishes in 460 ms, i.e. it genuinely earns Subtask 1. A single
  Subtask 2 query `1 100000000000` was still running after 5 s (killed), and any 10000-digit
  test never terminates.
- **Test target.** Already killed by the smallest Subtask 2 test; every subtask above 1
  contains such tests by construction.
