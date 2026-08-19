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
    vector<int> thresholds;
    for (int i = 0; i < m; ++i) {
        int u, v, h, d;
        cin >> u >> v >> h >> d;
        graph[u].push_back({v, h, d});
        graph[v].push_back({u, h, d});
        thresholds.push_back(h);
    }
    if (source == sink) {
        cout << 1 << '\n' << source << '\n';
        return 0;
    }
    sort(thresholds.begin(), thresholds.end(), greater<int>());
    thresholds.erase(unique(thresholds.begin(), thresholds.end()), thresholds.end());

    const long long INF = (1LL << 60);
    vector<int> answer;
    for (int limit : thresholds) {
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
                if (e.h < limit || dist[e.to] <= du + e.d) continue;
                dist[e.to] = du + e.d;
                parent[e.to] = u;
                heap.push({dist[e.to], e.to});
            }
        }
        if (dist[sink] != INF) {
            for (int u = sink; u != -1; u = parent[u]) {
                answer.push_back(u);
                if (u == source) break;
            }
            reverse(answer.begin(), answer.end());
            break;
        }
    }
    if (answer.empty()) {
        cout << -1 << '\n';
        return 0;
    }
    cout << answer.size() << '\n';
    for (size_t i = 0; i < answer.size(); ++i) {
        if (i) cout << ' ';
        cout << answer[i];
    }
    cout << '\n';
}
