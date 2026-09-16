# Prerequisite floors

The hardest technique a solution **requires** sets a floor: the level below which the
problem cannot land, whatever the code looks like. A floor is not an estimate — Pass C
produces the estimate.

A technique that merely appears does not count. Ask whether a solver who does not know it
can still solve the problem within the constraints. If yes, it is not a prerequisite.

| Technique | Floor |
|---|---|
| two pointers, prefix sums, sorting + greedy | 1000 |
| DSU, basic graph traversal with a twist, binary search on answer | 1200 |
| segment tree / BIT, standard DP over one dimension, shortest paths | 1500 |
| digit DP, bitmask DP over subsets, tree DP with rerooting | 1700 |
| lazy propagation, SOS DP, matrix exponentiation, flows | 1800 |
| FFT/NTT, convex hull trick, heavy-light decomposition | 1900 |
| centroid decomposition, link-cut-free offline tricks, Mo's on trees | 2000 |
| suffix automaton, suffix tree, advanced string automata | 2100 |

Nothing on the list required ⇒ floor `900`.

Two techniques from the same row do not stack: take the single hardest, then let Pass D
charge at most `+200` for the extra independent insights they represent.

<!-- Calibration: the drafted column was shifted down by 200 as one constant in tuning
round 2, to remove a measured signed bias of +192 on a 24-problem blind eval. Row order is
unchanged. -->
