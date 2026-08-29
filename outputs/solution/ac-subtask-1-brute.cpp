// ac-subtask-1-brute.cpp — correct for Subtask 1 scope only (R < 1e6).
// Iterates every integer in [L,R] and checks the two conditions directly.
// Complexity O(R-L+1): fine for R < 1e6, but TLE beyond that scope.
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

    ll cnt = 0, mn = -1;
    for (ll x = L; x <= R; ++x) {
        if (x % 15 == 0 && digitSum(x) == (int)K) {
            ++cnt;
            if (mn == -1) mn = x;
        }
    }

    cout << cnt << '\n' << mn << '\n';
    return 0;
}
