# Anchors

Real Codeforces problems with their true ratings. Pass C places the problem being rated
against these, in the window `[floor, floor+600]`.

Ratings are labels, not estimates — they come from the Codeforces API. Do not adjust one
because a problem "feels" harder.

| id | rating | div | prereq | intended solution |
|---|---|---|---|---|
| 2245B | 1100 | Div1+2 | two pointers, prefix sums, sorting + greedy | Sort the values; each pairing sacrifices its smaller element, so choose p smallest elements to sacrifice maximizing c·p minus their prefix sum. |
| 1099C | 1200 | Div2 | two pointers, prefix sums, sorting + greedy | Classify each letter as mandatory, optional (may drop), or repeatable (may drop/keep/repeat), then greedily pad or trim to hit length k. |
| 1257C | 1200 | Div2 | none | Track each value's previous occurrence index; the shortest dominated subarray is one more than the minimum gap between two equal values. |
| 1541B | 1200 | Div2 | none | Bound a_i·a_j ≤ i+j ≤ 2n, then for each i enumerate candidate values of a_j and check the resulting index against a value-to-position map. |
| 1832C | 1200 | Div2 | two pointers, prefix sums, sorting + greedy | The contrast telescopes along monotonic runs, so keep only the array's local extrema (direction-change points); their count plus one is the answer. |
| 2019B | 1200 | Div2 | none | Derive a formula for segment coverage at each point/gap, aggregate equal coverage values in a hashmap, and answer each query by lookup. |
| 934B | 1200 | Div2 | none | Map each digit's loop count (8→2, 0/4/6/9→1, else 0) and construct floor(k/2) eights plus one extra digit if k is odd, else -1. |
| 1870C | 1300 | Div1+2 | none | Bucket indices by value; sweep from k down to 1 merging each value's positions into a running min/max range, giving each color's box. |
| 2152B | 1300 | Div1+2 | none | Transform to u=r+c, v=r-c coordinates turning the king pursuit into a closed-form Chebyshev/Manhattan case analysis for the capture turn. |
| 2160C | 1300 | Div2 | none | Note bit i and its mirrored bit of x XOR reverse(x) are forced equal, so n must satisfy a bit-palindrome condition, checked directly. |
| 1513B | 1400 | Div2 | none | Prove prefix and suffix AND must both equal the array's overall AND X; answer is c(c-1)(n-2)! for c = frequency of X, else 0. |
| 1989C | 1400 | Div2 | DSU, basic graph traversal with a twist, binary search on answer | Classify reviewers into nine outcome types, resolve the unambiguous ones directly, then binary-search the achievable minimum rating checking feasibility with the flexible ones. |
| 2127C | 1400 | Div1+2 | two pointers, prefix sums, sorting + greedy | Realize k rounds are irrelevant since repeating one pair suffices; sort interval endpoints to find the pair minimizing forced rearrangement cost. |
| 2245C | 1400 | Div1+2 | none | Exploit that prefix mex is non-decreasing; greedily interleave "next missing value" and "largest unused value" insertions to hit the target XOR k. |
| 1797C | 1500 | Div1+2 | none | Query three well-chosen cells (Chebyshev distances) and algebraically solve the resulting max() equations to pin down the king's row and column. |
| 1804C | 1500 | Div1+2 | none | Use that triangular numbers mod n repeat with period at most 2n, so brute-force check every force f from 1 to min(p, 2n). |
| 1924A | 1500 | Div1 | two pointers, prefix sums, sorting + greedy | Greedily partition s into consecutive blocks each covering all k letters; at least n full blocks means YES, else build a missing witness. |
| 2109C1 | 1500 | Div2 | none | Interactive: repeated digit-sum reduction shrinks x to a few candidates; feedback from add/mul/div then identifies and rebuilds x as n. |  <!-- unsure -->
| 2140C | 1500 | Div2 | two pointers, prefix sums, sorting + greedy | Prove only one swap ever happens (Bob then ends); pick the best single swap via prefix max/min scans over parity-based value swings. |
| 1455D | 1600 | Div2 | two pointers, prefix sums, sorting + greedy | Greedily scan left to right, swapping a_i with x whenever a_i > x and a_i breaks the sorted order built so far. |
| 980B | 1600 | Div2 | none | Ad hoc construction: exploit top-bottom mirror symmetry between the two paths, placing hotels symmetrically (or blocking a full row) to force equal path counts. |
| 1073D | 1700 | Div2 | DSU, basic graph traversal with a twist, binary search on answer | Binary search on the number of full laps around the circle; prefix sums give the spend per lap, then simulate the final partial lap. |
| 1228C | 1700 | Div2 | none | Factorize x by trial division, then for each prime sum floor(n/p^i) (Legendre-style) to get its exponent, and multiply modpow results. |
| 2018C | 1700 | Div1 | two pointers, prefix sums, sorting + greedy | DFS gets each node's depth and subtree height; a difference array over [depth, depth+height] finds the target depth maximizing kept nodes. |
| 1327E | 1800 | Div2 | none | Direct combinatorial formula per block length: count boundary vs interior placements with free-digit choices, using precomputed powers of 10 mod p. |
| 1758D | 1800 | Div2 | none | Constructive: fix D=max-min, pick n distinct integers clustered near D^2/n, then nudge two values so the sum equals D^2 exactly. |
| 1923D | 1800 | Div2 | DSU, basic graph traversal with a twist, binary search on answer | For each slime, binary search the minimal same-direction prefix-sum window exceeding its size, merging growth from both sides. |
| 933A | 1800 | Div1 | segment tree / BIT, standard DP over one dimension, shortest paths | Multi-phase one-dimensional DP (five states for pre-ones, the reversed twos run, and trailing ones) tracks the best achievable non-decreasing pattern. |
| 1063C | 1900 | Div1 | none | Place points via an adaptive convex-hull construction so any online adversarial coloring stays linearly separable at the end. |  <!-- unsure -->
| 1120A | 1900 | Div1 | two pointers, prefix sums, sorting + greedy | Two-pointer sliding window locates the shortest segment containing b's required multiset, then greedily trims flowers so it becomes an early workpiece. |
| 1717D | 1900 | Div2 | two pointers, prefix sums, sorting + greedy | Recognize the answer equals the prefix sum of C(n,i) for i=0..k, computed via precomputed factorials mod 1e9+7. |
| 1080D | 2000 | Div2 | two pointers, prefix sums, sorting + greedy | Greedy per-level consumption of a closed-form split capacity (4^i-1)/3 determines whether exactly k operations keep the diagonal path intact. |
| 1513D | 2000 | Div2 | segment tree / BIT, standard DP over one dimension, shortest paths | Process values ascending; binary-search each one's maximal gcd-equal range via sparse table, then DSU-merge that range at this weight, Kruskal-style. |
| 1943B | 2000 | Div1 | two pointers, prefix sums, sorting + greedy | Because overlapping equal-length palindromic substrings force periodicity, almost every k is good except the whole segment or its trim, verified via O(1) palindrome hashing. |
| 1265E | 2100 | Div2 | segment tree / BIT, standard DP over one dimension, shortest paths | Express expected days E via a backward linear recurrence per mirror, solve the self-referential equation in E, using modular inverse. |
| 1278D | 2100 | Div2 | segment tree / BIT, standard DP over one dimension, shortest paths | Sweep endpoints with a BIT-based stack to find each crossing-pair edge, then DSU-union them checking for exactly n-1 edges and no cycle. |
| 1695D1 | 2200 | Div2 | DSU, basic graph traversal with a twist, binary search on answer | Compute the tree's metric dimension via leaves minus exterior-major-vertices: walk each leaf up to its nearest degree>=3 ancestor and count distinct ones. |
| 1909F1 | 2200 | Div1+2 | none | Track d_i = i - a_i as the unmatched row/column defect; multiply per-step factors 1, 1+2d, or d^2 depending on a_i - a_{i-1}. |
| 932D | 2200 | Div1+2 | none | Build each node's chain to its nearest ancestor with weight >= its own via binary lifting, then binary-search that chain's prefix sum against X. |
| 979D | 2200 | Div2 | digit DP, bitmask DP over subsets, tree DP with rerooting | Insert u into a binary trie per divisor; walk bits maximizing XOR while a tight flag keeps v within s-x. |
| 980E | 2200 | Div2 | DSU, basic graph traversal with a twist, binary search on answer | Repeatedly pop the smallest-indexed leaf from a min-heap k times, since 2^i weighting means only leaf removals preserve the optimum. |
| 1513E | 2300 | Div2 | none | Split values into above/below/equal-average groups; require both non-equal groups contiguous (2 orderings) if size >= 2 each, else count all permutations via factorials. |
| 2002D2 | 2300 | Div1+2 | none | Reduce global DFS-order validity to a local, O(1)-checkable condition per adjacent permutation pair using ancestor/subtree-size relations, updated after each swap. |
| 1348E | 2400 | Div2 | segment tree / BIT, standard DP over one dimension, shortest paths | Limit mixed single-shrub baskets to shrubs with min(a_i,b_i) < k, then knapsack-DP over achievable red-berry totals mod basket size. |
| 1860E | 2400 | Div2 | segment tree / BIT, standard DP over one dimension, shortest paths | Compress positions by adjacent-letter-pair class, precompute nearest-class distances, run Floyd-Warshall over the <=676 classes, then combine per query. |
| 2005D | 2400 | Div2 | segment tree / BIT, standard DP over one dimension, shortest paths | Precompute prefix/suffix gcds; for each l, jump through the O(log) breakpoints where the swapped-range gcds change, maximizing the sum. |
| 2097C | 2400 | Div1 | none | Unfold the triangle via repeated reflections into a straight line, then use gcd/Euclidean analysis on velocity ratios to find escape vertex and bounce count. |
| 983C | 2400 | Div1 | digit DP, bitmask DP over subsets, tree DP with rerooting | DP state tracks employees loaded so far, current floor, and the destination-floor multiset of the up-to-4 passengers still inside the elevator. |
| 1301E | 2500 | Div2 | segment tree / BIT, standard DP over one dimension, shortest paths | DP computes each cell's max valid quarter-colored square size, a 2D sparse table answers range-max, and binary search per query finds the best size. |
| 1691F | 2500 | Div2 | digit DP, bitmask DP over subsets, tree DP with rerooting | Reroot the tree, tracking how many size-k subsets each edge separates from the root's rooted subtree, to sum f(r,S) in linear time per root. |
| 2248F | 2500 | Div2 | none | Decompose the decrement matrix's minimum-rectangle-operation cost via 2D differences, then optimize which cells to force into peaks within a budget. |
| 1218C | 2600 | Div1 | segment tree / BIT, standard DP over one dimension, shortest paths | Match each transformer's period-4 cycle time to the path's fixed diagonal-time to place its cost on one grid cell, then DP the path. |
| 1391E | 2600 | Div2 | DSU, basic graph traversal with a twist, binary search on answer | DFS tree ensures only ancestor-descendant edges; if depth is small, pair tree-siblings by parent, else the deep root-to-leaf chain itself is the long path. |
| 1905F | 2600 | Div2 | none | Good indices are prefix-max breakpoints splitting p into consecutive value-blocks; find which single swap fixing/merging blocks maximizes new breakpoints. |
| 2231F | 2600 | Div2 | none | Distances follow Legendre's sum-of-three-squares theorem (mod-8 exceptions add one), with small differences resolved by direct BFS over nearby vertices. |

**55 anchors**, drawn from the frozen corpus in the skill's build-time calibration data.
Ratings come from the Codeforces API and are labels, not estimates. Rows marked
`<!-- unsure -->` had a summary the summarizer could not reconcile with the true rating;
prefer another anchor when one is available.
