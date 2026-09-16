# Anchors

Real Codeforces problems with their true ratings. Pass C places the problem being rated
against these, in the window `[floor, floor+600]`.

Ratings are labels, not estimates — they come from the Codeforces API. Do not adjust one
because a problem "feels" harder.

| id | rating | div | prereq | intended solution |
|---|---|---|---|---|
| 1497C1 | 1200 | Div2 | sorting + greedy | Case-split on n mod 4 (and parity) to construct three explicit summands whose divisibility keeps their LCM at or below n/2. |
| 1381A1 | 1300 | Div1 | two pointers | Simulate from the last unmatched character backward with two pointers over the remaining prefix, spending at most two flips per position to fix each target bit. |
| 1882C | 1500 | Div2 | prefix sums | Scan left to right keeping the best score for each parity of how many cards have been kept so far, adding a card's value only when discarding it lands on an even kept-count. |
| 1904D2 | 1800 | Div2 | segment tree / BIT | Group b into maximal equal-value runs and use range-maximum queries over a, with two-pointer expansion into adjacent compatible runs, to check each run's value is producible on top of the pointwise a_i <= b_i requirement. |
| 1017D | 1900 | Div1+2 | bitmask DP over subsets | Since n <= 12, represent every string as a bitmask and, for each possible query mask, precompute counts by weighted agreement over the full mask space so each query answers with a prefix-sum lookup. |
| 1120C | 2100 | Div1 | standard DP over one dimension | DP over prefix length where dp[i] is the minimum cost to encode the first i characters, using a precomputed all-pairs longest-common-prefix table to test in O(1) whether a candidate piece already occurred earlier. |
| 1042F | 2400 | Div2 | heavy-light decomposition | Root the tree and merge each node's children's leaf-distance multisets via small-to-large union, greedily closing off any leaf whose distance to the current farthest leaf would exceed k. |
| 2150E1 | 2600 | Div1 | binary search on answer | Interactively bisect the remaining candidate positions with existence queries, exploiting that every paired value's two occurrences land together while the unpaired value's does not, to isolate it in roughly 4n + O(log n) queries. |

**Provisional set — 8 anchors, one per band.** Task 13 replaces this table with 56 anchors drawn from the frozen corpus. The format does not change.
