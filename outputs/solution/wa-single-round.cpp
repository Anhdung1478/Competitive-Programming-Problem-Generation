// WA. Efficient (offline coordinate compression + Fenwick tree of (count, sum))
// and fast enough for the full constraints, but performs only ONE round of
// eating.
//
// Defect: the contestant reads "the frog eats the nearest fly to its left and
// jumps" as a single sweep -- eat every fly at position <= x, add up their
// positions, print -- and misses that the jump itself can bring further flies
// within reach, which must then be eaten too (the chain reaction).
//
// Correct on any test where the first sweep already lands on the fixpoint, i.e.
// no fly lies in (x, x + sum of flies <= x]. Sparse tests with huge coordinates
// and few flies hit that case often.
//
// Counterexample: N=3 Q=1, positions 1 2 5, query "2 2".
//   correct : 10 3   (eat 2 then 1 -> position 5, then eat 5 -> position 10)
//   this    : 5 2
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

    for (int i = 1; i <= n; ++i) modify(rank_le(pos[i]), pos[i], +1);

    for (int t = 0; t < q; ++t) {
        if (qs[t].type == 1) {
            int u = int(qs[t].a);
            ll y = qs[t].b;
            modify(rank_le(pos[u]), pos[u], -1);
            modify(rank_le(y), y, +1);
            pos[u] = y;
            continue;
        }

        ll x = qs[t].a;
        ll sum; int cnt;
        tie(sum, cnt) = query(rank_le(x));   // one sweep only -- no loop
        cout << x + sum << ' ' << cnt << '\n';
    }
    return 0;
}
