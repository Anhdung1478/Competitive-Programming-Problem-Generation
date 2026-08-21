/**
 * @tag main
 * @expect main=OK
 * @algorithm read and discard the graph; print "A" if n is odd and "B" if n is even
 * @complexity O(n + m), all of it spent on input
 */
// sol-main.cpp -- model solution for "The Balance"
//
// ---------------------------------------------------------------------------
// The whole problem
// ---------------------------------------------------------------------------
// A wins if and only if n is odd.  The edges do not matter.  B's opening move
// -- deleting any subset of the edges -- does not matter either, because it
// cannot change n, and n is the only thing the outcome depends on.
//
// There are two ways to see it.  The second is the one worth remembering.
//
// ---------------------------------------------------------------------------
// Proof 1: induction on the number of remaining vertices
// ---------------------------------------------------------------------------
// Claim: in a position with k vertices remaining, the player to move wins if
// and only if k is odd.  Induct on k.
//
//   k = 0.  There is no vertex, so the player to move cannot move and loses.
//           k is even and the mover loses.  Consistent.
//
//   k odd.  By the handshake lemma the number of odd-degree vertices in any
//           graph is even, so it cannot be all k of them when k is odd.  Some
//           vertex of even degree therefore exists, and deleting it hands the
//           opponent a position with k - 1 vertices, which is even, which by
//           induction the opponent loses.  The mover wins.
//
//   k even. Every legal move leads to k - 1 vertices, which is odd, which by
//           induction the opponent wins.  And if no legal move exists the
//           mover loses at once.  Either way the mover loses.
//
// A moves first into a position with n vertices, so A wins exactly when n is
// odd.
//
// ---------------------------------------------------------------------------
// Proof 2: the game has no decisions in it at all
// ---------------------------------------------------------------------------
// The game stops only in a position where EVERY remaining vertex has odd
// degree.  By the handshake lemma again, a graph whose vertices all have odd
// degree has an even number of vertices.  So the game always ends with an even
// number of vertices left on the board, which means the number of moves played
// is
//
//     n - (something even)  ==  n   (mod 2)
//
// no matter how either player plays.  The length of the game is fixed modulo 2
// before the first move is made.  Neither player has any influence whatsoever:
// there are no good moves and no bad moves, only moves.  If n is odd the number
// of moves is odd, so the last one was A's (A moves on the odd-numbered turns)
// and B is the one left staring at a board with nothing legal on it.
//
// This is stronger than proof 1 and explains why the problem is a joke: there
// is nothing to optimize, so "both play optimally" is decoration on a statement
// that already had a fixed answer.
//
// ---------------------------------------------------------------------------
// Why n <= 36 and 3 seconds and 1 GB
// ---------------------------------------------------------------------------
// Bait.  n <= 36 is exactly the size that makes a contestant reach for meet in
// the middle over 2^18, and 2^36 states is far enough out of reach that the
// bitmask game search cannot be squeezed through no matter how carefully it is
// written.  The limits are generous so that nobody can claim the trap was a
// timing accident: even at 3 seconds and 1 GB, the exponential search does not
// come close.  See solutions/tle-bitmask-game.cpp.
//
// This solution reads the graph it does not need, for exactly one reason: the
// input format promises m lines and a submission that stops reading after the
// first line is still correct.  Reading them costs nothing and keeps the
// program honest about what it was given.
#include <bits/stdc++.h>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    if (!(std::cin >> n >> m)) return 0;
    for (int i = 0; i < m; ++i) {
        int u, v;
        std::cin >> u >> v;
        (void)u;
        (void)v;
    }

    std::cout << (n % 2 == 1 ? "Shen" : "Zed") << '\n';
    return 0;
}
