// WA (full scope) - the classic prefix-subtraction misconception.
// Computes g(x) = (maxPower, purity) over [1, x] with a correct digit DP, then tries
// to answer [L, R] as g(R) "minus" g(L-1). Subtracting the two count components is
// invalid: when every number attaining the maximum power lies below L, the difference
// reports that (too large) power with a count of 0 instead of the real answer.
#include <bits/stdc++.h>
using namespace std;

typedef pair<int, int> pii;
const int MOD = 998244353;

void merge(pii &a, const pii &b) {
    if (b.first < 0) return;
    if (a.first < b.first) a = pii(b.first, 0);
    if (a.first == b.first && (a.second += b.second) >= MOD) a.second -= MOD;
}

// g(x): maximum power over [1, x] and the total purity of the numbers attaining it.
pii upTo(const string &s) {
    int n = (int)s.size();
    // dp[last][nonz][tightHigh]
    static pii cur[10][2][2], nxt[10][2][2];
    for (int d = 0; d < 10; ++d)
        for (int z = 0; z < 2; ++z)
            for (int tr = 0; tr < 2; ++tr) cur[d][z][tr] = pii(-1, 0);
    cur[0][0][1] = pii(0, 1);

    for (int i = 0; i < n; ++i) {
        for (int d = 0; d < 10; ++d)
            for (int z = 0; z < 2; ++z)
                for (int tr = 0; tr < 2; ++tr) nxt[d][z][tr] = pii(-1, 0);
        int digHi = s[i] - '0';
        for (int last = 0; last < 10; ++last)
            for (int nonz = 0; nonz < 2; ++nonz)
                for (int tr = 0; tr < 2; ++tr) {
                    pii st = cur[last][nonz][tr];
                    if (st.first < 0) continue;
                    int to = tr ? digHi : 9;
                    for (int d = 0; d <= to; ++d) {
                        int ntr = tr && (d == digHi);
                        int nz = nonz | (d > 0);
                        merge(nxt[last][nz][ntr], st);
                        if (nz && d >= last) merge(nxt[d][nz][ntr], pii(st.first + 1, st.second));
                    }
                }
        for (int d = 0; d < 10; ++d)
            for (int z = 0; z < 2; ++z)
                for (int tr = 0; tr < 2; ++tr) cur[d][z][tr] = nxt[d][z][tr];
    }

    pii res(0, 0);
    for (int last = 0; last < 10; ++last)
        for (int nonz = 0; nonz < 2; ++nonz)
            for (int tr = 0; tr < 2; ++tr) merge(res, cur[last][nonz][tr]);
    return res;
}

string decrement(string s) {
    int i = (int)s.size() - 1;
    while (i >= 0 && s[i] == '0') s[i--] = '9';
    if (i < 0) return "0";
    --s[i];
    int p = 0;
    while (p + 1 < (int)s.size() && s[p] == '0') ++p;
    return s.substr(p);
}

int main() {
    ios_base::sync_with_stdio(false), cin.tie(nullptr);

    #define TASK "lis"
    if (fopen(TASK".inp", "r")) {
        freopen(TASK".inp", "r", stdin);
        freopen(TASK".out", "w", stdout);
    }

    int t;
    cin >> t;
    while (t--) {
        string L, R;
        cin >> L >> R;
        pii hi = upTo(R);
        string lm1 = decrement(L);
        pii lo = (lm1 == "0") ? pii(0, 0) : upTo(lm1);
        int cnt = hi.second;
        if (hi.first == lo.first) cnt = (int)((hi.second - lo.second + MOD) % MOD);
        cout << hi.first << ' ' << cnt << '\n';
    }
    return 0;
}
