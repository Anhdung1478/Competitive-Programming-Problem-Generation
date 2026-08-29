// gentest.cpp — Polygon testlib generator for "Mật thư của Dũng".
// Generates one line: L R K  (1 <= L <= R <= 1e18, 1 <= K <= 1e9).
// Invocation:  gentest <SEED> --subtask <S> --rate <R> [--profile <P>] > $
// See outputs/generator-config.md for the full design contract.
#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

ll pow10(int e) {
    ll r = 1;
    for (int i = 0; i < e; ++i) r *= 10;
    return r;
}

int digitSum(ll x) {
    int s = 0;
    while (x > 0) { s += (int)(x % 10); x /= 10; }
    return s;
}

int digitsOf(ll x) {
    int d = 0;
    while (x > 0) { ++d; x /= 10; }
    return d;
}

// A K that guarantees answer "0 -1".
ll genZeroK(int maxSum) {
    int strat = rnd.next(0, 2);
    if (strat == 0) {
        // K not divisible by 3 -> no candidate is divisible by 15.
        ll K = 3LL * rnd.next(0, maxSum / 3) + 1 + rnd.next(0, 1);
        ensure(K % 3 != 0);
        return K;
    } else if (strat == 1 && maxSum < 162) {
        // K exceeds the maximum digit sum of any number <= R.
        ll K = rnd.next((ll)(maxSum + 1), 162LL);
        ensure(K > maxSum);
        return K;
    } else {
        // K beyond any 18-digit digit sum.
        return rnd.next(163LL, 1000000000LL);
    }
}

// A "meaningful" K (mostly multiples of 3, with boundary values mixed in).
ll genDefaultK(int maxSum) {
    int roll = rnd.next(0, 9);
    if (roll == 0) {
        int cand[7] = {1, 3, 15, 45, 153, 156, 162};
        return min((ll)cand[rnd.next(0, 6)], (ll)maxSum);
    } else if (rnd.next(0, 1)) {
        return 3LL * rnd.next(1, maxSum / 3);
    } else {
        return rnd.next(1, maxSum);
    }
}

int main(int argc, char* argv[]) {
    registerGen(argc, argv, 1);
    prepareOpts(argc, argv);

    int subtask = opt<int>("subtask");
    double rate = opt<double>("rate");
    string profile = opt<string>("profile", "random");

    ensure(subtask >= 1 && subtask <= 5);
    ensure(0.70 <= rate && rate <= 1.00);

    // Subtask ceilings and digit-count ceilings.
    ll Rmax; int D;
    if (subtask == 1)      { Rmax = 999999LL;              D = 6;  }
    else if (subtask == 2) { Rmax = 99999999999LL;         D = 11; }
    else if (subtask == 3) { Rmax = 1000000000000000000LL; D = 18; }
    else if (subtask == 4) { Rmax = 999999999999999LL;     D = 15; }
    else                   { Rmax = 1000000000000000000LL; D = 18; }

    bool needRge15 = (profile == "nonzero" || profile == "kill-offbyone");

    // ---- Phase 1: R ----
    ll R;
    if (subtask == 3) {
        int x = (int)llround((rate - 0.70) / 0.30 * 18.0);
        x = max(0, min(18, x));
        if (needRge15) x = max(x, 2);
        R = pow10(x);
    } else if (profile == "maxR" || profile == "kill-tle") {
        R = Rmax;
    } else {
        int d = (int)llround((rate - 0.70) / 0.30 * D);
        d = max(1, min(D, d));
        if (needRge15) d = max(d, 2);
        ll lo = pow10(d - 1), hi = min(Rmax, pow10(d) - 1);
        if (needRge15) lo = max(lo, 15LL);
        R = rnd.next(lo, hi);
    }

    int dR = digitsOf(R);
    int maxSum = min(162, 9 * dR);

    // ---- Phase 2: L and K per profile ----
    ll L, K;
    if (profile == "kill-tle") {
        L = 1; K = genDefaultK(maxSum);
    } else if (profile == "maxR") {
        L = rnd.next(1LL, R); K = genDefaultK(maxSum);
    } else if (profile == "zero") {
        L = rnd.next(1LL, R); K = genZeroK(maxSum);
    } else if (profile == "nonzero") {
        ll m = 15LL * rnd.next(1LL, R / 15);
        K = digitSum(m);
        L = rnd.next(1LL, m);
        ensure(L <= m && m <= R && m % 15 == 0 && digitSum(m) == (int)K);
    } else if (profile == "kill-factor5") {
        K = 3 * rnd.next(1, 3);   // 3, 6, or 9
        L = 1;
        if (R < K) R = K;         // ensure the number K itself is in range
        ensure(R >= K);
    } else if (profile == "kill-offbyone") {
        ll m = 15LL * rnd.next(1LL, R / 15);
        L = m; K = digitSum(m);
        ensure(L % 15 == 0 && digitSum(L) == (int)K && 1 <= L && L <= R);
    } else {
        // random / full / single / narrow
        if (profile == "full")       L = 1;
        else if (profile == "single") L = R;
        else if (profile == "narrow") L = max(1LL, R - rnd.next(1LL, 1000LL));
        else                          L = rnd.next(1LL, R);  // random
        K = genDefaultK(maxSum);
    }

    // Subtask 3 fixes L = 1 (only L-free profiles are meaningful there).
    if (subtask == 3) {
        ensure(profile == "random" || profile == "full" || profile == "zero" || profile == "nonzero");
        L = 1;
    }

    // ---- Final invariants ----
    ensure(1 <= L && L <= R && R <= 1000000000000000000LL);
    ensure(1 <= K && K <= 1000000000);
    if (subtask == 1) ensure(R < 1000000LL);
    if (subtask == 2) ensure(R < 100000000000LL);
    if (subtask == 4) ensure(R < 1000000000000000LL);
    if (subtask == 5) ensure(R <= 1000000000000000000LL);
    if (subtask == 3) {
        ensure(L == 1);
        ll t = R;
        while (t > 1 && t % 10 == 0) t /= 10;
        ensure(t == 1); // R is a power of 10
    }

    cout << L << ' ' << R << ' ' << K << '\n';
    return 0;
}
