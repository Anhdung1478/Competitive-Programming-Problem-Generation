# Generator design contract

## 1. Input schema

The input contains exactly one test case.

| Field | Type | Scope | Legal range | Relations | Repeated |
|---|---|---|---|---|---|
| $N$ | 32-bit integer | whole test | $1 \le N \le 10^9$ | Subtask 1 also requires $N\cdot M\le 10^6$ | once |
| $M$ | 64-bit integer | whole test | $1 \le M \le 10^{18}$ | Subtask 2 fixes $M=2$; subtask 4 requires $M\le10$ | once |
| $K$ | 32-bit integer | whole test | $0 \le K \le \min(M,10^4)$ | Subtask 3 fixes $K=0$ | once |
| $A_i$ | 64-bit integer | prescribed day $i$ | $1 \le A_i \le M$ | $A_1<A_2<\ldots<A_K$ | $K$ times |
| $B_i$ | 32-bit integer | prescribed candy $i$ | $1 \le B_i \le N$ | Candy values may repeat | $K$ times |

There is no test-count field and therefore no cross-test sum constraint.

## 2. Structural invariants

- Print `N M K` first, followed by exactly $K$ lines `A_i B_i`.
- The prescribed days are distinct and strictly increasing.
- Every prescribed day lies in $[1,M]`; every prescribed candy lies in $[1,N]$.
- Repeated candy values are legal, including on adjacent prescribed days. Such cases may deliberately have answer zero.
- $K=0$ produces no subsequent lines.
- All subtask restrictions are enforced in addition to the full constraints.

## 3. Subtasks

| Subtask | Score | Additional restriction | Generator enforcement |
|---|---:|---|---|
| 1 | 20% | $N\cdot M\le10^6$ | Choose the coupled pair $(N,M)$ under a scaled product cap and verify the product using 64-bit arithmetic. |
| 2 | 20% | $M=2$ | Set $M$ to exactly 2; select $N$ and $K\le2$ according to the profile. |
| 3 | 20% | $K=0$ | Force $K=0$ and ignore all fixed-day structure profiles. |
| 4 | 20% | $M\le10$ | Choose $M$ in $[1,10]$ and then enforce $K\le M$. |
| 5 | 20% | no additional restriction | Use the full bounds. |

The percentages sum to 100%.

## 4. Interpretation of `rate`

`rate` must be in $[0.70,1.00]$. For a normal variable with legal interval $[L,U]$, define the rate ceiling

`ceilRate(L,U) = L + floor((U-L) * rate)`

using `long double`, followed by clamping to $[L,U]$. Random, sparse, dense, and endpoint profiles select values no larger than this ceiling, usually with a distribution biased toward the ceiling. Explicit structural profiles may choose small legal values when their purpose is a degenerate or fully prescribed case.

- Subtasks 2 and 3 keep their exact restrictions regardless of `rate`.
- In subtask 1, first set `productCap = max(1, floor(10^6 * rate))`, then choose $N$ and $M$ so that $N\cdot M\le productCap$. Profiles may favor either a large $N$, a large $M$, or balanced factors.
- In subtasks 4 and 5, apply the rate ceiling separately to $N$ and $M$; subtask 4 clamps the $M$ ceiling to 10.
- After $M$ is known, define `kCap = min(M,10000)`. Sparse profiles choose at most 10 prescribed days, random profiles choose across $[0,kCap]$, and dense/max profiles choose near `kCap`. Profiles that require exactly two endpoints ensure $M\ge2$.
- `rate` never relaxes a lower bound, strict ordering, the product bound, or an exact subtask restriction.

## 5. Generator options

- Positional `seed`: the first argument after `gentest`; consumed by `registerGen`. There is no named `--seed` option.
- `--subtask <S>`: required integer in `[1,5]`.
- `--rate <R>`: required real number in `[0.70,1.00]`.
- `--profile <name>`: required semantic profile. Allowed values are:
  - `random`, `sparse`, `dense`, `no-fixed`;
  - `all-fixed-valid`, `all-fixed-conflict`, `edge-days`;
  - `n-one`, `n-two`;
  - `same-endpoints`, `different-endpoints`;
  - `max-values`, `long-gap-same`, `long-gap-different`;
  - `free-random`, `free-n-one`, `free-n-two`, `free-max`.

The `free-*` profiles always set $K=0$ and are intended for subtask 3. All options used by `test-script.txt` are listed above.

## 6. Generation profiles

- `random`: rate-scaled $N,M$, a broad random $K$, random distinct prescribed days, and random candies.
- `sparse`: few prescribed days spread through the interval; catches logic around long unconstrained segments.
- `dense`: $K$ near its legal cap, with a mixture of repeated and changing candies.
- `no-fixed` / `free-random`: $K=0$; validates the closed-form-like unconstrained behavior.
- `all-fixed-valid`: choose $M\le10^4$, set $K=M$, and generate a valid alternating/cyclic candy schedule. If $N=1$, use $M=1$.
- `all-fixed-conflict`: choose $2\le M\le10^4$, set $K=M$, and force at least one adjacent pair to use the same candy, so the answer is zero.
- `edge-days`: prescribe day 1 and day $M$ when $M>1$, plus optional internal days; covers free-prefix/free-suffix boundaries.
- `n-one`: set $N=1$ and vary $M,K$ and prescriptions, including both the sole feasible one-day case and impossible longer schedules.
- `n-two`: set $N=2$; stresses deterministic alternation and parity between fixed endpoints.
- `same-endpoints` / `different-endpoints`: prescribe two separated days with equal/different candies and vary the gap parity.
- `long-gap-same` / `long-gap-different`: full-range variants with $A_1=1$, $A_2=M$, and very large $M$ whenever the subtask permits.
- `max-values`: choose $N,M,K$ near their applicable maxima and include candy values near 1 and $N$.
- `free-n-one`, `free-n-two`, `free-max`: $K=0$ with respectively $N=1$, $N=2$, or rate-scaled near-maximum $N,M$.

## 7. Subtask-specific generation

- Subtask 1 cannot be produced by independently scaling $N$ and $M$; generate one first and cap the other by `productCap / first`.
- Subtask 2 always sets $M=2$, including for low-rate tests.
- Subtask 3 always overrides $K$ to zero. Its script uses only `free-*` profiles.
- Subtask 4 caps $M$ at 10 before selecting $K$.
- Profiles requiring $K=M$ first cap $M$ at 10,000 so the global $K$ bound remains valid.
- Profiles requiring distinct candies ensure $N\ge2`; if a selected subtask/profile combination permits $N=1$, raise $N$ within that subtask's legal bounds.

## 8. Coverage plan

`test-script.txt` contains exactly 100 tests, 20 per subtask. Each 20-test group has 3 rates in `[0.70,0.80)`, 7 in `[0.80,0.90)`, and 10 in `[0.90,1.00]`. Totals are therefore 15 low, 35 medium, and 50 high-rate tests.

Subtasks 1, 4, and 5 mix random, sparse, dense, free, fully prescribed, endpoint, and small-$N$ profiles. Subtask 2 emphasizes all possible two-day constraint shapes. Subtask 3 rotates the four unconstrained profiles. Subtask 5 additionally includes maximum-value and extremely long same/different-endpoint gaps.

## 9. Specialized generator skills

Use `generator-array` with `polygon-gentest`. The input contains the ordered arrays of prescribed days and candy values, and adversarial placement, repetitions, endpoints, and alternating value patterns are central. No graph, tree, string, or number-theory generator skill applies.

## 10. Generator assertions

Before printing, `gentest.cpp` must ensure:

- `1 <= N && N <= 1e9`;
- `1 <= M && M <= 1e18`;
- `0 <= K && K <= min(M,10000)`;
- the vector lengths equal $K$;
- every $A_i$ is in `[1,M]` and every $B_i$ is in `[1,N]`;
- prescribed days are strictly increasing;
- subtask 1 has `N*M <= 1e6` without overflow;
- subtask 2 has `M == 2`;
- subtask 3 has `K == 0`;
- subtask 4 has `M <= 10`;
- the selected profile's promised properties hold (for example, a forced conflict really exists).
