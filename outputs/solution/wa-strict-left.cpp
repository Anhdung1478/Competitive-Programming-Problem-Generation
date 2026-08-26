// WA. Full correct algorithm (offline coordinate compression + Fenwick tree of
// (count, sum), doubling rounds) with one boundary misread.
//
// Defect: "the nearest fly to its left" is implemented as strictly to the left,
// i.e. positions < current position, using lower_bound where upper_bound is
// required. The statement says a fly standing on the same square as the frog is
// also eaten.
//
// Correct on every test where no fly ever sits exactly on the frog's current
// position -- which random tests with 1e9-wide coordinates almost always
// satisfy, so this needs a deliberate collision to expose.
//
// Counterexample: N=1 Q=1, positions 7, query "2 7".
//   correct : 14 1
//   this    : 7 0
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

struct Query { int type; ll a, b; };

int nTree;
vector<ll> fenSum;
vector<int> fenCnt;

void modify(int i, ll val, int sign) {
    for (; i <= nTree; i += i & -i) { fenSum[i] += val * sign; fenCnt[i] += sign; }
}

pair<ll, int> query(int i) {
    ll s = 0; int c = 0;
    for (; i > 0; i -= i & -i) { s += fenSum[i]; c += fenCnt[i]; }
    return make_pair(s, c);
}

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
    vector<ll> pos(n + 1);
    vector<ll> coords;
    for (int i = 1; i <= n; ++i) { cin >> pos[i]; coords.push_back(pos[i]); }

    vector<Query> qs(q);
    for (int t = 0; t < q; ++t) {
        cin >> qs[t].type >> qs[t].a;
        if (qs[t].type == 1) { cin >> qs[t].b; coords.push_back(qs[t].b); }
    }

    sort(coords.begin(), coords.end());
    coords.erase(unique(coords.begin(), coords.end()), coords.end());
    nTree = int(coords.size());
    fenSum.assign(nTree + 1, 0);
    fenCnt.assign(nTree + 1, 0);

    // rank(v) = number of distinct coordinates strictly below v -- the
    // lower_bound that makes this solution wrong.
    auto rank_of = [&](ll v) {
        return int(lower_bound(coords.begin(), coords.end(), v) - coords.begin());
    };

    for (int i = 1; i <= n; ++i)
        modify(int(upper_bound(coords.begin(), coords.end(), pos[i]) - coords.begin()), pos[i], +1);

    for (int t = 0; t < q; ++t) {
        if (qs[t].type == 1) {
            int u = int(qs[t].a);
            ll y = qs[t].b;
            modify(int(upper_bound(coords.begin(), coords.end(), pos[u]) - coords.begin()), pos[u], -1);
            modify(int(upper_bound(coords.begin(), coords.end(), y) - coords.begin()), y, +1);
            pos[u] = y;
            continue;
        }

        ll x = qs[t].a;
        ll sumPrev; int cntPrev;
        tie(sumPrev, cntPrev) = query(rank_of(x));
        ll cur = x + sumPrev;
        while (true) {
            ll sumNow; int cntNow;
            tie(sumNow, cntNow) = query(rank_of(cur));
            if (cntNow == cntPrev) break;
            cur += sumNow - sumPrev;
            sumPrev = sumNow;
            cntPrev = cntNow;
        }
        cout << cur << ' ' << cntPrev << '\n';
    }
    return 0;
}
