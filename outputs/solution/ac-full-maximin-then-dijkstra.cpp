#include <bits/stdc++.h>
using namespace std;

struct Edge { int to, h, d; };

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
        graph[u].push_back({v, h, d});
        graph[v].push_back({u, h, d});
    }

    vector<int> height(n + 1, -1);
    priority_queue<pair<int, int>> maxHeap;
    height[source] = 10'001;
    maxHeap.push({height[source], source});
    while (!maxHeap.empty()) {
        auto [current, u] = maxHeap.top();
        maxHeap.pop();
        if (current != height[u]) continue;
        for (const Edge &e : graph[u]) {
            int candidate = min(current, e.h);
            if (candidate > height[e.to]) {
                height[e.to] = candidate;
                maxHeap.push({candidate, e.to});
            }
        }
    }

    if (height[sink] == -1) {
        cout << -1 << '\n';
        return 0;
    }

    const long long INF = (1LL << 60);
    vector<long long> dist(n + 1, INF);
    vector<int> parent(n + 1, -1);
    priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> minHeap;
    dist[source] = 0;
    minHeap.push({0, source});
    while (!minHeap.empty()) {
        auto [du, u] = minHeap.top();
        minHeap.pop();
        if (du != dist[u]) continue;
        for (const Edge &e : graph[u]) {
            if (e.h < height[sink]) continue;
            if (dist[e.to] > du + e.d) {
                dist[e.to] = du + e.d;
                parent[e.to] = u;
                minHeap.push({dist[e.to], e.to});
            }
        }
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
