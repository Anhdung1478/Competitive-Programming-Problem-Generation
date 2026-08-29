# Solution suite — Mật thư của Dũng

Time limit (authoritative, from `source/problem-context.md`): **1s**.
Jury/oracle: `source/solution.cpp` (digit DP, validated in Step 1 after fixing a
`dp[20][172][15][2]` out-of-bounds fix). Independent reference used during
validation: `ref.cpp` (tight-flag digit DP) plus brute force for small domains.

## Files

### ac-full-digitdp.cpp — verdict AC — full constraints

- **Scope:** full constraints `1 ≤ L ≤ R ≤ 1e18`, `1 ≤ K ≤ 1e9`.
- **Algorithm:** digit DP over states `(position, remaining digit-sum, value mod 15, tight)`,
  memoized, counting `x ∈ [0, bound]` with digit-sum `K` and `x ≡ 0 (mod 15)`; the answer for
  `[L, R]` is `count(R) − count(L−1)`. The smallest lucky number is found by binary search on the
  monotone `count(·)`. `K > 162` short-circuits to `0` (max digit-sum of any `x ≤ 1e18` is `9·18`).
- **Complexity:** `O(19 · 162 · 15 · 2 · 10)` ≈ `9·10^5` per `count` call, `× ~60` binary-search
  calls ≈ `5·10^7` ops. **Memory** `O(20·172·15·2·8)` ≈ 825 KB.
- **Difference vs source/solution.cpp:** same algorithm family (digit DP); independently derived
  with a `tight` flag and `(pos,sum,rem,tight)` memo layout versus the source's `smaller` flag and
  `(id,sum,mod15,smaller)` layout. No materially different second full algorithm exists — a digit
  DP is the canonical approach for "digit-sum = K and value ≡ 0 mod 15"; recorded per AGENTS.md.
- **Validation:** compiled `gnu++17 -O2 -Wall -Wextra -Wshadow -Wconversion` (0 warnings); 295
  differential cases vs the official solution (0 mismatches), including `R = 1e18` and boundary `K`
  in `{1,2,3,9,15,150,153,156,157,160,162,163,171,172,1e9}`. Max-case runtime 0.08s.

### ac-subtask-1-brute.cpp — verdict AC — Subtask 1 only (`R < 1e6`)

- **Scope:** exactly Subtask 1 (`R < 10^6`).
- **Algorithm:** enumerate every `x ∈ [L, R]`, keep `x % 15 == 0 && digitSum(x) == K`.
- **Complexity:** `O(R − L + 1)` ≤ `10^6` within scope; **memory** `O(1)`.
- **Difference:** brute force vs digit DP.
- **Validation:** 804 differential cases vs official within scope (0 mismatches), incl. boundary
  `R = 999999`.

### wa-ignore-factor5.cpp — verdict WA

- **Defect:** misreads "divisible by 15" as "divisible by 3". The DP tracks the value modulo 3
  (the digit-sum test) and drops the divisibility-by-5 requirement (`15 = 3 · 5`, last digit 0/5).
- **Counterexample:** `L=1 R=15 K=6` → prints `2` / `6`; correct is `1` / `15` (it wrongly counts
  `6`, whose digit-sum is 6 and which is divisible by 3 but `6 % 15 ≠ 0`).
- **Kill profile:** any `K ≡ 0 (mod 3)` with `L` small enough that a multiple of 3 (not 5) with
  digit-sum `K` falls in range — e.g. `K=3/6/9`, `L=1`, `R ≥ K`.

### wa-offbyone.cpp — verdict WA

- **Defect:** range subtraction uses `countUpTo(L)` instead of `countUpTo(L−1)`, dropping `L`
  itself (and shifting the minimum baseline identically).
- **Counterexample:** `L=15 R=15 K=6` → prints `0` / `-1`; correct is `1` / `15`.
- **Kill profile:** any test where `L` itself is a lucky number (e.g. `L` a multiple of 15 with
  digit-sum `K`), since `countUpTo(L)` already includes `L`.

### tle-multiples.cpp — verdict TLE (logically correct)

- **Scope:** correct for the full semantic problem, too slow for large `R`.
- **Bottleneck:** iterates every multiple of 15 in `[L, R]` → `O(R/15)` iterations.
- **Limit:** 1s. **Adversarial profile:** `L=1`, `R ≈ 1e18` → `~6.7·10^16` iterations (impossible);
  already `R = 1e10` measured 9.2s, `R = 1e9` 0.84s.
- **Passes weak:** `R < 1e6` → `~6.7·10^4` iterations, instant. Verified 500/500 small cases match
  the official solution.
