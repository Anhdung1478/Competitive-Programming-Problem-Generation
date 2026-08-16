/**
 * @tag wrong-answer
 * @expect main=WA
 * @algorithm the intended CDQ plus monotone hull, except that the hull query walks a forward-only pointer instead of binary searching -- the reflex form of the convex hull trick, which assumes the query points arrive non-decreasing
 * @why-wrong combineHalves sweeps u in b_u order, so the query points a_u arrive in no order at all; once a query with a large a_u has pushed the pointer to the right, a later query with a smaller a_u cannot walk back to its own argmax
 * @complexity O(n log n) time, O(n) memory
 */
// wa-hull-pointer.cpp -- the third hull-internal mistake, and the one that
// hid the longest.
//
// The monotone convex hull trick is usually taught as a pair of pointers:
// slopes go in increasing, queries come out increasing, and both the hull and
// the query walk forward, giving O(1) amortised per query instead of O(log n).
// Half of that is true here.  Slopes really do arrive sorted -- the slope IS
// b_v and combineHalves inserts the left half in increasing charge.  The query
// side is not: combineHalves sweeps u in increasing b_u because that is what
// keeps the eligible set correct, and the query point is a_u, which the sweep
// order says nothing about.  Inside one CDQ node a_u wanders freely.
//
// So the pointer is right until the first query whose a_u is smaller than some
// earlier query's, and from then on it is stuck to the right of the argmax for
// the rest of the node.  It still returns a REAL line, so its delta is always
// achievable and never too large -- which is exactly why it is so hard to
// catch.  A CDQ answer is a maximum over every (u, node) pair, so the wrong
// choice is invisible unless the global optimum is decided by one specific
// late, small-a_u query.  It survived all 131 tests of the suite as it stood
// before section 11, including the entire hull-valley family that was added to
// close wa-hull-lastline and wa-hull-nopop.
//
// Recorded as flag tst-004 by the review phase; section 11 of build_tests.sh
// is the test that kills it, and the comment there works the counterexample
// through by hand.
#include <algorithm>
#include <climits>
#include <iostream>
#include <vector>

using ll = long long;

// -Wpedantic (part of the round's compile line) objects to __int128 as a GNU
// extension; __extension__ silences exactly that one complaint.
__extension__ typedef __int128 i128;

namespace {

struct Line {
    ll slope;
    ll intercept;
};

// Upper envelope of lines inserted in strictly increasing slope order.
// Reused across every cross step, so the buffer is allocated once.
std::vector<Line> hull;
int hullSize = 0;

// With m1 < m2 < m3, line 2 never wins alone iff lines 1 and 3 already cross
// at or before the point where 1 and 2 cross:
//
//     (c1 - c3) / (m3 - m1)  <=  (c1 - c2) / (m2 - m1)
//
// Both denominators are positive, so cross-multiplying preserves the
// direction and gives the integer test below.  See LANDMINE 3 above for why
// the products are widened.
bool makesMiddleUseless(const Line& first, const Line& middle, const Line& last) {
    const i128 lhs = static_cast<i128>(last.intercept - first.intercept) *
                     (middle.slope - first.slope);
    const i128 rhs = static_cast<i128>(middle.intercept - first.intercept) *
                     (last.slope - first.slope);
    return lhs >= rhs;
}

void addLine(ll slope, ll intercept) {
    const Line incoming{slope, intercept};

    // LANDMINE 2: a repeated b value repeats the slope.  Keep the higher
    // intercept and drop the other, restoring strict slope monotonicity
    // before any geometry runs.
    if (hullSize > 0 && hull[hullSize - 1].slope == slope) {
        if (hull[hullSize - 1].intercept >= intercept) return;
        --hullSize;
    }

    while (hullSize >= 2 &&
           makesMiddleUseless(hull[hullSize - 2], hull[hullSize - 1], incoming)) {
        --hullSize;
    }
    hull[hullSize++] = incoming;
}

ll evaluate(int index, ll query) {
    return hull[index].slope * query + hull[index].intercept;
}

// Along the envelope the value at a fixed query point rises then falls, so
// walking forward while the next line is no worse lands on the maximum -- for
// THIS query.  The pointer is kept across queries, which is the whole point of
// the amortised form and the whole of the bug.
int hullPointer = 0;

ll queryMax(ll query) {
    // addLine may have popped the hull back past the pointer; clamp it so the
    // walk stays in range.  (This is the careful version of the mistake -- the
    // careless one indexes out of bounds instead.)
    if (hullPointer >= hullSize) hullPointer = hullSize - 1;
    while (hullPointer + 1 < hullSize &&
           evaluate(hullPointer, query) <= evaluate(hullPointer + 1, query)) {
        ++hullPointer;
    }
    return evaluate(hullPointer, query);
}

int reagent;                    // x from the input
std::vector<int> power;         // a, permuted into (a, b) sorted order
std::vector<int> charge;        // b, in the same permuted order
std::vector<int> mergePower;    // scratch for the merge-by-charge step
std::vector<int> mergeCharge;
std::vector<int> groupStart;    // element index where each equal-a group begins
ll bestDelta = LLONG_MIN;

// Rewrite [left, right) as the merge of the charge-sorted [left, middle) and
// [middle, right), so the caller's range comes back charge-sorted too.
void mergeByCharge(int left, int middle, int right) {
    int i = left, j = middle, out = left;
    while (i < middle && j < right) {
        if (charge[i] <= charge[j]) {
            mergePower[out] = power[i];
            mergeCharge[out] = charge[i];
            ++i;
        } else {
            mergePower[out] = power[j];
            mergeCharge[out] = charge[j];
            ++j;
        }
        ++out;
    }
    while (i < middle) {
        mergePower[out] = power[i];
        mergeCharge[out] = charge[i];
        ++i;
        ++out;
    }
    while (j < right) {
        mergePower[out] = power[j];
        mergeCharge[out] = charge[j];
        ++j;
        ++out;
    }
    for (int k = left; k < right; ++k) {
        power[k] = mergePower[k];
        charge[k] = mergeCharge[k];
    }
}

// Consider every valid pair whose v lies in groups [groupLow, groupMid) and
// whose u lies in groups [groupMid, groupHigh).
void combineHalves(int left, int middle, int right) {
    hullSize = 0;
    hullPointer = 0;
    int candidate = left;
    for (int u = middle; u < right; ++u) {
        // Strictly smaller charge only -- this is b_v < b_u verbatim.
        while (candidate < middle && charge[candidate] < charge[u]) {
            addLine(charge[candidate],
                    static_cast<ll>(power[candidate]) * (reagent - charge[candidate]));
            ++candidate;
        }
        if (hullSize > 0) {
            const ll delta = queryMax(power[u]) -
                             static_cast<ll>(power[u]) * charge[u];
            bestDelta = std::max(bestDelta, delta);
        }
    }
}

void solveGroups(int groupLow, int groupHigh) {
    // A single equal-a group holds no valid pair (a_v == a_u there) and is
    // already charge-sorted, because the initial sort broke a-ties by b.
    if (groupHigh - groupLow <= 1) return;

    const int groupMid = groupLow + (groupHigh - groupLow) / 2;
    solveGroups(groupLow, groupMid);
    solveGroups(groupMid, groupHigh);

    const int left = groupStart[groupLow];
    const int middle = groupStart[groupMid];
    const int right = groupStart[groupHigh];
    combineHalves(left, middle, right);
    mergeByCharge(left, middle, right);
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    if (!(std::cin >> n >> reagent)) return 0;

    std::vector<int> inputPower(n), inputCharge(n);
    for (int& value : inputPower) std::cin >> value;
    for (int& value : inputCharge) std::cin >> value;

    ll baseScore = 0;
    for (int i = 0; i < n; ++i)
        baseScore += static_cast<ll>(inputPower[i]) * inputCharge[i];

    std::vector<int> order(n);
    for (int i = 0; i < n; ++i) order[i] = i;
    std::sort(order.begin(), order.end(), [&](int lhs, int rhs) {
        if (inputPower[lhs] != inputPower[rhs])
            return inputPower[lhs] < inputPower[rhs];
        return inputCharge[lhs] < inputCharge[rhs];
    });

    power.resize(n);
    charge.resize(n);
    mergePower.resize(n);
    mergeCharge.resize(n);
    hull.resize(n);
    for (int i = 0; i < n; ++i) {
        power[i] = inputPower[order[i]];
        charge[i] = inputCharge[order[i]];
    }

    groupStart.clear();
    for (int i = 0; i < n; ++i)
        if (i == 0 || power[i] != power[i - 1]) groupStart.push_back(i);
    const int groupCount = static_cast<int>(groupStart.size());
    groupStart.push_back(n);  // sentinel: one past the last group

    solveGroups(0, groupCount);

    // The statement guarantees a valid pair exists, so this branch is
    // unreachable on judge data; it keeps the program total anyway.
    const ll answer = (bestDelta == LLONG_MIN) ? baseScore : baseScore + bestDelta;
    std::cout << answer << '\n';
    return 0;
}
