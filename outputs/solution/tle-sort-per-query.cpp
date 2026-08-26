// TLE. Semantically correct for the full problem, too slow for large N and Q.
//
// The naive "just recompute it" solution: on every type-2 query, copy the fly
// positions, sort them, build prefix sums, then run the same doubling rounds
// with binary search. Updates are O(1). Nothing is reused between queries.
//
// Time  O(Q * N log N) -- about 4e10 elementary steps at N = Q = 2e5, versus
// 2s. Passes Subtasks 1 and 2 and any test with few type-2 queries or a small
// N, which is exactly why a contestant submits it.
// Memory O(N).
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

int main(void) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    #define TASK "jumping-frog"
    if (fopen(TASK ".inp", "r")) {
        freopen(TASK ".inp", "r", stdin);
        freopen(TASK ".out", "w", stdout);
    }

    int n, q;
    cin >> n >> q;

    vector<ll> pos(n);
    for (int i = 0; i < n; ++i) cin >> pos[i];

    vector<ll> sorted(n), pre(n + 1);
    while (q--) {
        int type;
        cin >> type;
        if (type == 1) {
            int u;
            ll y;
            cin >> u >> y;
            pos[u - 1] = y;
            continue;
        }

        ll x;
        cin >> x;

        sorted = pos;
        sort(sorted.begin(), sorted.end());
        pre[0] = 0;
        for (int i = 0; i < n; ++i) pre[i + 1] = pre[i] + sorted[i];

        int cntPrev = int(upper_bound(sorted.begin(), sorted.end(), x) - sorted.begin());
        ll cur = x + pre[cntPrev];
        while (true) {
            int cntNow = int(upper_bound(sorted.begin(), sorted.end(), cur) - sorted.begin());
            if (cntNow == cntPrev) break;
            cur += pre[cntNow] - pre[cntPrev];
            cntPrev = cntNow;
        }
        cout << cur << ' ' << cntPrev << '\n';
    }
    return 0;
}
