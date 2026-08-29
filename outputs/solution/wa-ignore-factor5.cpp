// wa-ignore-factor5.cpp — plausible WRONG ANSWER.
// Defect: "divisible by 15" is misread as "divisible by 3". The DP tracks
// the value modulo 3 (digit-sum test) but completely drops the divisibility
// by 5 (last digit must be 0 or 5) that 15 = 3 * 5 also requires.
// Counterexample: L=1 R=15 K=6 -> correct is "1\n15" (only 15), but this
// solution counts 6 (sum 6, divisible by 3, but 6 % 15 != 0) and returns "2\n6".
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

ll memo[20][172][3][2];
bool vis[20][172][3][2];
string s;

ll dp(int pos, int sum, int rem3, bool tight) {
    if (pos < 0) return (sum == 0 && rem3 == 0) ? 1 : 0;
    if (sum < 0) return 0;
    ll &res = memo[pos][sum][rem3][tight];
    if (vis[pos][sum][rem3][tight]) return res;
    vis[pos][sum][rem3][tight] = true;
    int hi = tight ? (s[pos] - '0') : 9;
    res = 0;
    for (int d = 0; d <= hi; ++d)
        res += dp(pos - 1, sum - d, (rem3 * 10 + d) % 3, tight && (d == hi));
    return res;
}

ll countUpTo(ll x, int K) {
    if (x <= 0) return 0;
    if (K > 162) return 0;
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
