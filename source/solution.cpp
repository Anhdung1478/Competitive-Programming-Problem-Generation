#include<bits/stdc++.h>
using namespace std;

#define fi first
#define se second
#define sz(x) int((x).size())
typedef long long ll;
typedef pair<int, int> ii;

const bool isMultiTest = 0;
const int MAXN = 5e5+5;
const int MOD = 1e9+7;

vector<int> adj[MAXN];
ii reqs[MAXN];
int degIn[MAXN], nArr, numReqs;

void input(void) {
    cin >> nArr >> numReqs;
    for (int i = 0; i < numReqs; ++i) {
        cin >> reqs[i].fi >> reqs[i].se;
    }
}

void prepare(void) {
    for (int i = 0; i < numReqs; ++i) {
        int u(reqs[i].fi), v(reqs[i].se);
        adj[u].push_back(v);
        ++degIn[v];
    }
}

void solve(void) {
    priority_queue<int, vector<int>, greater<int>> source;

    for (int i = 1; i <= nArr; ++i) if(!degIn[i]) source.push(i);

    vector<int> result;
    while(sz(source)) {
        int u(source.top());
        source.pop();

        result.push_back(u);
        for (int v : adj[u]) if(--degIn[v] == 0) source.push(v);
    }

    if(sz(result) != nArr) {
        cout << "impossible\n";
        return;
    }

    for (int x : result) cout << x << ' ';
    cout << '\n';
}

void process(void) {
    input();
    prepare();
    solve();
}

int main(void) {
    ios_base::sync_with_stdio(0), cin.tie(0), cout.tie(0);

    #define TASK "minimum-lineup"
    if(fopen(TASK".inp", "r")) {
        freopen(TASK".inp", "r", stdin);
        freopen(TASK".out", "w", stdout);
    }

    int numTest = 1;
    if(isMultiTest) cin >> numTest;

    while(numTest--) process();

    return 0;
}
