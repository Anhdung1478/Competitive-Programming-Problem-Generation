# Solution suite: Truy tìm kho báu

Authoritative time limit: **2.25 seconds**. Every source uses guarded courses.inp/courses.out redirection and falls back to standard input/output when courses.inp is absent.

Only one AC solution is included. Any full solution within the constraints must resolve the ordering of up to 17 required monochromatic blocks; the natural efficient formulations all reduce to the same subset DP over used letters. A second implementation would therefore differ mainly in preprocessing details rather than algorithmic substance.

## ac-full-earliest-block-subset-dp.cpp — AC

- Intended verdict: **AC**.
- Claimed scope: full constraints and all five subtasks.
- Independently chosen algorithm: binary-search the answer len. For every letter and starting position, precompute the earliest compatible length-len block. A subset DP stores the smallest ending position after placing one nonoverlapping block for every letter in the mask, in any order.
- Complexity: O((nk + k·2^k) log n) time and O(nk + 2^k) memory.
- Material difference from source/solution.cpp: the candidate stores prefix counts of positions incompatible with each letter and records earliest valid block starts. The official source counts the requested letter together with wildcards and stores valid block ends. Both use the same necessary subset-ordering state.
- Validation evidence: compiled under GNU C++17 with warnings enabled; matched the official solution on 400 randomized suite cases and on maximum n=200000, k=17 all-wildcard, all-a, and periodic fixed-string profiles. Local times for those maximum profiles were approximately 312 ms, 194 ms, and 196 ms, all within 2.25 seconds.
- Supported domain: every valid n, k, and S from the source specification.
- Generation properties for future work: k=1, k>n, all wildcards, no wildcards, missing letters, reverse block order, alternating fixed letters, long forced runs, and n=200000, k=17.

## wa-fixed-letter-order.cpp — WA

- Intended verdict: **WA**.
- Claimed scope: full constraints.
- Algorithm: binary-search the answer and greedily place the earliest compatible block for a, then b, and so on.
- Complexity: O(nk log n) time and O(n) input memory.
- Precise defect: a feasible collection of blocks may occur in a different left-to-right letter order. Greedy earliest placement is valid only after an order has been fixed; fixing alphabetical order loses valid schedules.
- Valid counterexample:

      2 2
      ba

  The correct answer is 1, using the block for b before the block for a; this candidate prints 0.
- Validation evidence: compiles and follows the exact input/output format. The counterexample was executed: official 1, full AC 1, WA 0.
- Generation target: fixed strings whose required letter blocks appear in a nonalphabetical order, especially reverse order with few or no wildcards.

## tle-enumerate-letter-orders.cpp — TLE

- Intended verdict: **TLE**.
- Claimed scope: semantically correct for the full constraints.
- Algorithm: for each binary-search value, enumerate every permutation of the k letters and greedily place the earliest compatible block in that order.
- Complexity: O((nk + k·k!) log n) time and O(nk) memory.
- Correctness rationale: if a feasible completion exists, choose one qualifying run for each letter and sort these disjoint runs by position. For that permutation, earliest compatible placement never ends later than the chosen runs, so the enumeration finds a solution.
- Bottleneck and authoritative limit: an adversarial infeasible case forces all permutations. A local k=11 case already took about 206 ms; increasing from 11! to 17! multiplies the permutation count by 12·13·14·15·16·17, far beyond the **2.25-second** limit.
- Validation evidence: compiles, matched the official solution on 400 randomized small cases, succeeds on weak/small inputs, and returns the correct result on the WA counterexample.
- Generation target: k=17 with fixed characters making every candidate block length infeasible only after testing many order prefixes; avoid all-wildcard instances because the first permutation succeeds immediately.

## Oracle and kill summary for later generation work

| Candidate | Failure mechanism | Target profile | Relevant subtasks | Differential oracle |
|---|---|---|---|---|
| wa-fixed-letter-order.cpp | Assumes alphabetical left-to-right block order | reverse-order, fixed or nearly fixed | 1, 2, 3, 4, 5 | official solution or full AC |
| tle-enumerate-letter-orders.cpp | Enumerates up to k! orders | permutation-exhaustion, k=17 | 4, 5 | official solution or full AC |
