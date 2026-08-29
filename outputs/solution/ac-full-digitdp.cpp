// ac-full-digitdp.cpp — correct for ALL constraints.
// Digit DP counting x in [L,R] with digit-sum == K and x % 15 == 0,
// then binary search for the smallest such x.
// Complexity: O(digits * sum * 15 * 2 * 10) per countUpTo call,
// plus O(log R) calls for the minimum => well under 1s for R <= 1e18.
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

ll memo[20][172][15][2];
bool vis[20][172][15][2];
string s;

ll dp(int pos, int sum, int rem, bool tight) {
    if (pos < 0) return (sum == 0 && rem == 0) ? 1 : 0;
    if (sum < 0) return 0;
    ll &res = memo[pos][sum][rem][tight];
    if (vis[pos][sum][rem][tight]) return res;
    vis[pos][sum][rem][tight] = true;
    int hi = tight ? (s[pos] - '0') : 9;
    res = 0;
    for (int d = 0; d <= hi; ++d)
        res += dp(pos - 1, sum - d, (rem * 10 + d) % 15, tight && (d == hi));
    return res;
}

ll countUpTo(ll x, int K) {
    if (x <= 0) return 0;
    if (K > 162) return 0; // max digit sum for any x <= 1e18 is 9*18 = 162
    s = to_string(x);
    reverse(s.begin(), s.end());
    memset(vis, 0, sizeof vis);
    return dp((int)s.size() - 1, K, 0, true);
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(nullptr);

    #define TASK "min-number"
    if (fopen(TASK ".inp", "r")) {
        freopen(TASK ".inp", "r", stdin);
        freopen(TASK ".out", "w", stdout);
    }

    ll L, R; int K;
    cin >> L >> R >> K;

    ll cL = countUpTo(L - 1, K);
    ll cnt = countUpTo(R, K) - cL;

    ll mn = -1;
    if (cnt > 0) {
        ll lo = L, hi = R;
        while (lo < hi) {
            ll mid = (lo + hi) / 2;
            if (countUpTo(mid, K) > cL) hi = mid;
            else lo = mid + 1;
        }
        mn = lo;
    }

    cout << cnt << '\n' << mn << '\n';
    return 0;
}
