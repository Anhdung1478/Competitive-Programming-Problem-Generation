#include <bits/stdc++.h>
using namespace std;

struct Edge { int to, d; };

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
    vector<vector<Edge>> graph(n + 1);
    for (int i = 0; i < m; ++i) {
        int u, v, h, d;
        cin >> u >> v >> h >> d;
        graph[u].push_back({v, d});
        graph[v].push_back({u, d});
    }

    const long long INF = (1LL << 60);
    vector<long long> dist(n + 1, INF);
    vector<int> parent(n + 1, -1);
    priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> heap;
    dist[source] = 0;
    heap.push({0, source});
    while (!heap.empty()) {
        auto [du, u] = heap.top();
        heap.pop();
        if (du != dist[u]) continue;
        for (const Edge &e : graph[u]) {
            if (dist[e.to] > du + e.d) {
                dist[e.to] = du + e.d;
                parent[e.to] = u;
                heap.push({dist[e.to], e.to});
            }
        }
    }
    if (dist[sink] == INF) {
        cout << -1 << '\n';
        return 0;
    }
    vector<int> path;
    for (int u = sink; u != -1; u = parent[u]) {
        path.push_back(u);
        if (u == source) break;
    }
    reverse(path.begin(), path.end());
    cout << path.size() << '\n';
    for (size_t i = 0; i < path.size(); ++i) {
        if (i) cout << ' ';
        cout << path[i];
    }
    cout << '\n';
}
