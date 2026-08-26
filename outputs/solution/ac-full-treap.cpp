// AC, full constraints.
//
// Independent approach: keep the multiset of fly positions in a randomized
// balanced BST (treap) keyed by position, every node carrying the count and the
// sum of its subtree. This is fully ONLINE -- no coordinate compression and no
// pre-reading of the query list.
//
// A type-2 query at x repeats: q = (sum, cnt) of flies at position <= current
// position; while cnt grows, position = x + sum. Each round eats a fly that sat
// beyond the previous round's position, so the position more than doubles per
// round => O(log(answer)) ~ 48 rounds, each an O(log N) root-to-leaf descent.
//
// Time  O((N + Q) log N) plus O(log(answer) * log N) per type-2 query.
// Memory O(N + Q) nodes.
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

const int MAXNODE = 400005;

int lc[MAXNODE], rc[MAXNODE], cntSub[MAXNODE], key[MAXNODE], prio[MAXNODE];
ll sumSub[MAXNODE];
int nodeCount = 0, root = 0;

mt19937 rng(1234567);

int newNode(int k) {
    int v = ++nodeCount;
    lc[v] = rc[v] = 0;
    key[v] = k;
    prio[v] = int(rng() >> 1);
    cntSub[v] = 1;
    sumSub[v] = k;
    return v;
}

void pull(int v) {
    cntSub[v] = cntSub[lc[v]] + cntSub[rc[v]] + 1;
    sumSub[v] = sumSub[lc[v]] + sumSub[rc[v]] + key[v];
}

// a gets keys <= k, b gets keys > k
void split(int v, int k, int &a, int &b) {
    if (!v) { a = b = 0; return; }
    if (key[v] <= k) { a = v; split(rc[v], k, rc[a], b); pull(a); }
    else { b = v; split(lc[v], k, a, lc[b]); pull(b); }
}

int merge(int a, int b) {
    if (!a || !b) return a ? a : b;
    if (prio[a] > prio[b]) { rc[a] = merge(rc[a], b); pull(a); return a; }
    lc[b] = merge(a, lc[b]); pull(b); return b;
}

void insertPos(int k) {
    int a, b;
    split(root, k, a, b);
    root = merge(merge(a, newNode(k)), b);
}

void erasePos(int k) {
    int a, b, c, d;
    split(root, k, a, b);        // a: <= k
    split(a, k - 1, c, d);       // d: == k, non-empty because k is present
    d = merge(lc[d], rc[d]);     // drop one occurrence (the subtree root)
    root = merge(merge(c, d), b);
}

// (sum, count) over flies at position <= p
pair<ll, int> prefix(ll p) {
    ll s = 0;
    int c = 0, v = root;
    while (v) {
        if (key[v] <= p) {
            s += sumSub[lc[v]] + key[v];
            c += cntSub[lc[v]] + 1;
            v = rc[v];
        } else {
            v = lc[v];
        }
    }
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

    vector<int> pos(n + 1);
    // Node 0 is the empty sentinel: cntSub[0] = 0, sumSub[0] = 0.
    cntSub[0] = 0;
    sumSub[0] = 0;
    for (int i = 1; i <= n; ++i) {
        cin >> pos[i];
        insertPos(pos[i]);
    }

    while (q--) {
        int type;
        cin >> type;
        if (type == 1) {
            int u, y;
            cin >> u >> y;
            erasePos(pos[u]);
            insertPos(y);
            pos[u] = y;
            continue;
        }

        ll x;
        cin >> x;
        ll sumPrev;
        int cntPrev;
        tie(sumPrev, cntPrev) = prefix(x);
        ll cur = x + sumPrev;
        while (true) {
            ll sumNow;
            int cntNow;
            tie(sumNow, cntNow) = prefix(cur);
            if (cntNow == cntPrev) break;
            cur += sumNow - sumPrev;
            sumPrev = sumNow;
            cntPrev = cntNow;
        }
        cout << cur << ' ' << cntPrev << '\n';
    }
    return 0;
}
