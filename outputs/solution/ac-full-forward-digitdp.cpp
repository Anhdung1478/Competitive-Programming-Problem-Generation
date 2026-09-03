// AC (full scope) - forward (prefix) digit DP, iterative, rolling layer.
// State after processing a prefix: (last chosen digit, seen-nonzero, tightLow, tightHigh)
// Value: (length of the chosen non-decreasing subsequence so far, number of
//         (prefix, chosen index set) pairs reaching that length in this state).
#include <bits/stdc++.h>
using namespace std;

typedef pair<int, int> pii;
const int MOD = 998244353;

// Merge b into a, keeping the maximum length and summing the counts of that length.
void merge(pii &a, const pii &b) {
    if (b.first < 0) return;                      // unreachable source state
    if (a.first < b.first) a = pii(b.first, 0);
    if (a.first == b.first && (a.second += b.second) >= MOD) a.second -= MOD;
}

// dp[last][nonz][tightLow][tightHigh]
typedef array<array<array<array<pii, 2>, 2>, 2>, 10> Layer;

Layer makeEmpty() {
    Layer L;
    for (int d = 0; d < 10; ++d)
        for (int z = 0; z < 2; ++z)
            for (int tl = 0; tl < 2; ++tl)
                for (int tr = 0; tr < 2; ++tr) L[d][z][tl][tr] = pii(-1, 0);
    return L;
}

pii solve(const string &sL, const string &sR) {
    int n = (int)sR.size();
    string lo = string(n - (int)sL.size(), '0') + sL;

    Layer cur = makeEmpty(), nxt;
    cur[0][0][1][1] = pii(0, 1);

    for (int i = 0; i < n; ++i) {
        nxt = makeEmpty();
        int digLo = lo[i] - '0', digHi = sR[i] - '0';
        for (int last = 0; last < 10; ++last)
            for (int nonz = 0; nonz < 2; ++nonz)
                for (int tl = 0; tl < 2; ++tl)
                    for (int tr = 0; tr < 2; ++tr) {
                        pii st = cur[last][nonz][tl][tr];
                        if (st.first < 0) continue;
                        int from = tl ? digLo : 0;
                        int to = tr ? digHi : 9;
                        for (int d = from; d <= to; ++d) {
                            int ntl = tl && (d == digLo);
                            int ntr = tr && (d == digHi);
                            int nz = nonz | (d > 0);
                            merge(nxt[last][nz][ntl][ntr], st);           // skip digit i
                            if (nz && d >= last)                          // take digit i
                                merge(nxt[d][nz][ntl][ntr], pii(st.first + 1, st.second));
                        }
                    }
        cur.swap(nxt);
    }

    pii res(0, 0);
    for (int last = 0; last < 10; ++last)
        for (int nonz = 0; nonz < 2; ++nonz)
            for (int tl = 0; tl < 2; ++tl)
                for (int tr = 0; tr < 2; ++tr)
                    if (cur[last][nonz][tl][tr].first >= 0) merge(res, cur[last][nonz][tl][tr]);
    return res;
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
        pii ans = solve(L, R);
        cout << ans.first << ' ' << ans.second << '\n';
    }
    return 0;
}
