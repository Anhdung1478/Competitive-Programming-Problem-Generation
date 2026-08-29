// tle-multiples.cpp — plausible TIME LIMIT EXCEEDED (logically correct).
// Iterates every multiple of 15 in [L,R] and checks its digit sum.
// Semantically correct for all inputs (a lucky number must be a multiple of 15),
// but O(R/15) iterations: passes small/weak tests yet blows past 1s for large R
// (e.g. L=1 R=1e18 forces ~6.7e16 iterations).
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int digitSum(ll x) {
    int s = 0;
    while (x > 0) { s += (int)(x % 10); x /= 10; }
    return s;
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(nullptr);

    #define TASK "min-number"
    if (fopen(TASK ".inp", "r")) {
        freopen(TASK ".inp", "r", stdin);
        freopen(TASK ".out", "w", stdout);
    }

    ll L, R, K;
    cin >> L >> R >> K;

    ll first = ((L + 14) / 15) * 15; // smallest multiple of 15 >= L
    ll cnt = 0, mn = -1;
    for (ll m = first; m <= R; m += 15) {
        if (digitSum(m) == (int)K) {
            ++cnt;
            if (mn == -1) mn = m;
        }
    }

    cout << cnt << '\n' << mn << '\n';
    return 0;
}
