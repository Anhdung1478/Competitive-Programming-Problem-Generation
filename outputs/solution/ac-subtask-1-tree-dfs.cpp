#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    #define TASK "tunnel"
    if (fopen(TASK ".inp", "r")) {
        freopen(TASK ".inp", "r", stdin);
        freopen(TASK ".out", "w", stdout);
    }

    int n, source, sink, m;
    cin >> n >> source >> sink;
    cin >> m;
    vector<vector<int>> graph(n + 1);
    for (int i = 0; i < m; ++i) {
        int u, v, h, d;
        cin >> u >> v >> h >> d;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    vector<int> parent(n + 1, -1);
    vector<int> stack = {source};
    parent[source] = 0;
    while (!stack.empty()) {
        int u = stack.back();
        stack.pop_back();
        for (int v : graph[u]) {
            if (parent[v] == -1) {
                parent[v] = u;
                stack.push_back(v);
            }
        }
    }

    vector<int> path;
    for (int u = sink; u != 0; u = parent[u]) path.push_back(u);
    reverse(path.begin(), path.end());
    cout << path.size() << '\n';
    for (size_t i = 0; i < path.size(); ++i) {
        if (i) cout << ' ';
        cout << path[i];
    }
    cout << '\n';
}
