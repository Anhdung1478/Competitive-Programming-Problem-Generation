// AC on Subtask 1 only ($N, Q \le 200$).
//
// Direct simulation of the statement: for each type-2 query, repeatedly scan
// all flies for the uneaten one with the largest position <= the frog's
// position, eat it, and jump. No data structure at all.
//
// One fly is eaten per round and every round rescans all N flies, so a query
// that eats everything costs O(N^2) and the whole run is O(Q * N^2). At
// N = Q = 200 that is 8e6 scan steps (measured 0.14s on the worst case where
// every query eats every fly); at N = Q = 2000 it is 8e9 and takes over 12s,
// so Subtask 2 is already out of reach.
//
// Time  O(Q * N^2) worst case, Memory O(N).
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

    vector<ll> pos(n + 1);
    for (int i = 1; i <= n; ++i) cin >> pos[i];

    vector<char> eaten(n + 1, 0);
    while (q--) {
        int type;
        cin >> type;
        if (type == 1) {
            int u;
            ll y;
            cin >> u >> y;
            pos[u] = y;
            continue;
        }

        ll x;
        cin >> x;
        fill(eaten.begin(), eaten.end(), 0);
        ll cur = x;
        int cnt = 0;
        while (true) {
            int best = -1;
            for (int i = 1; i <= n; ++i)
                if (!eaten[i] && pos[i] <= cur && (best == -1 || pos[i] > pos[best]))
                    best = i;
            if (best == -1) break;
            eaten[best] = 1;
            cur += pos[best];
            ++cnt;
        }
        cout << cur << ' ' << cnt << '\n';
    }
    return 0;
}
