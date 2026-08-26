// WA. Full correct algorithm (offline coordinate compression + Fenwick tree of
// (count, sum), doubling rounds) with one modelling misread.
//
// Defect: the contestant misses the "gia su" note -- that a type-2 query is
// hypothetical -- and lets the frog really eat the flies, deleting every fly it
// consumed from the structure. The first type-2 query of a test is therefore
// always right and every later one can be wrong, because flies that should
// still be on the swamp are gone.
//
// A type-1 query on an already-eaten fly reinserts it at its new position, the
// natural reading once flies are being removed.
//
// Counterexample: N=1 Q=2, positions 1, queries "2 1" then "2 1".
//   correct : 2 1 / 2 1
//   this    : 2 1 / 1 0
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

    auto rank_le = [&](ll v) {
        return int(upper_bound(coords.begin(), coords.end(), v) - coords.begin());
    };

    multiset<pair<ll, int> > alive;           // (position, fly index)
    vector<char> onSwamp(n + 1, 1);
    for (int i = 1; i <= n; ++i) {
        modify(rank_le(pos[i]), pos[i], +1);
        alive.insert(make_pair(pos[i], i));
    }

    for (int t = 0; t < q; ++t) {
        if (qs[t].type == 1) {
            int u = int(qs[t].a);
            ll y = qs[t].b;
            if (onSwamp[u]) {
                modify(rank_le(pos[u]), pos[u], -1);
                alive.erase(alive.find(make_pair(pos[u], u)));
            }
            pos[u] = y;
            onSwamp[u] = 1;
            modify(rank_le(y), y, +1);
            alive.insert(make_pair(y, u));
            continue;
        }

        ll x = qs[t].a;
        ll sumPrev; int cntPrev;
        tie(sumPrev, cntPrev) = query(rank_le(x));
        ll cur = x + sumPrev;
        while (true) {
            ll sumNow; int cntNow;
            tie(sumNow, cntNow) = query(rank_le(cur));
            if (cntNow == cntPrev) break;
            cur += sumNow - sumPrev;
            sumPrev = sumNow;
            cntPrev = cntNow;
        }
        cout << cur << ' ' << cntPrev << '\n';

        // the defect: the flies really are removed
        while (!alive.empty() && alive.begin()->first <= cur) {
            int idx = alive.begin()->second;
            modify(rank_le(pos[idx]), pos[idx], -1);
            onSwamp[idx] = 0;
            alive.erase(alive.begin());
        }
    }
    return 0;
}
