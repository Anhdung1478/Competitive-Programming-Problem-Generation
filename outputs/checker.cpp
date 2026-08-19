#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

struct Edge {
    int u, v, h, d;
};

struct PathResult {
    bool impossible = false;
    vector<int> path;
};

[[noreturn]] void reject(bool jury, const string &message) {
    quitf(jury ? _fail : _wa, "%s", message.c_str());
}

ll parseInteger(const string &token, bool jury, const string &name) {
    if (token.empty()) reject(jury, name + " is empty");
    char *end = nullptr;
    errno = 0;
    long long value = strtoll(token.c_str(), &end, 10);
    if (errno == ERANGE || end != token.c_str() + token.size()) {
        reject(jury, name + " is not a valid integer");
    }
    return value;
}

PathResult readPath(InStream &stream, bool jury, int n) {
    if (stream.seekEof()) reject(jury, "output is empty");
    string first = stream.readToken("-?[0-9]+", "K");
    ll k = parseInteger(first, jury, "K");
    if (k == -1) {
        if (!stream.seekEof()) reject(jury, "extra output after -1");
        return {true, {}};
    }
    if (k < 1 || k > n) reject(jury, "K is outside [1, N]");
    vector<int> path;
    path.reserve((size_t)k);
    for (int pos = 0; pos < k; ++pos) {
        if (stream.seekEof()) reject(jury, "path ended before K vertices");
        ll vertex = parseInteger(
            stream.readToken("[0-9]+", "path vertex"), jury, "vertex");
        if (vertex < 1 || vertex > n) reject(jury, "path vertex is outside [1, N]");
        path.push_back((int)vertex);
    }
    if (!stream.seekEof()) reject(jury, "extra output after the path");
    return {false, path};
}

int main(int argc, char *argv[]) {
    registerTestlibCmd(argc, argv);

    int n = inf.readInt(1, 100'000, "N");
    int source = inf.readInt(1, n, "X");
    int sink = inf.readInt(1, n, "Y");
    int m = inf.readInt(0, 100'000, "M");

    vector<Edge> edges(m);
    vector<vector<int>> adj(n + 1);
    unordered_map<long long, pair<int, int>> road;
    road.reserve((size_t)m * 2 + 1);
    auto keyOf = [n](int u, int v) -> long long {
        if (u > v) swap(u, v);
        return 1LL * u * (n + 1LL) + v;
    };

    for (int id = 0; id < m; ++id) {
        int u = inf.readInt(1, n, "road endpoint i");
        int v = inf.readInt(1, n, "road endpoint j");
        int h = inf.readInt(1, 10'000, "road height H");
        int d = inf.readInt(1, 10'000, "road length D");
        edges[id] = {u, v, h, d};
        adj[u].push_back(id);
        adj[v].push_back(id);
        road[keyOf(u, v)] = {h, d};
    }

    const int HEIGHT_INF = 10'001;
    vector<int> best(n + 1, -1);
    priority_queue<pair<int, int>> pq;
    best[source] = HEIGHT_INF;
    pq.push({best[source], source});
    while (!pq.empty()) {
        auto [height, u] = pq.top();
        pq.pop();
        if (height != best[u]) continue;
        for (int id : adj[u]) {
            const Edge &e = edges[id];
            int v = e.u ^ e.v ^ u;
            int candidate = min(height, e.h);
            if (candidate > best[v]) {
                best[v] = candidate;
                pq.push({candidate, v});
            }
        }
    }

    const ll DIST_INF = (1LL << 60);
    vector<ll> dist(n + 1, DIST_INF);
    if (best[sink] != -1) {
        priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> heap;
        dist[source] = 0;
        heap.push({0, source});
        while (!heap.empty()) {
            auto [du, u] = heap.top();
            heap.pop();
            if (du != dist[u]) continue;
            for (int id : adj[u]) {
                const Edge &e = edges[id];
                if (e.h < best[sink]) continue;
                int v = e.u ^ e.v ^ u;
                if (dist[v] > du + e.d) {
                    dist[v] = du + e.d;
                    heap.push({dist[v], v});
                }
            }
        }
    }

    auto validate = [&](const PathResult &result, bool jury) {
        if (best[sink] == -1) {
            if (!result.impossible) reject(jury, "a path was printed although Y is unreachable");
            return;
        }
        if (result.impossible) reject(jury, "-1 was printed although a path exists");
        if (result.path.front() != source || result.path.back() != sink) {
            reject(jury, "path must start at X and end at Y");
        }
        int bottleneck = HEIGHT_INF;
        ll length = 0;
        for (size_t i = 1; i < result.path.size(); ++i) {
            auto it = road.find(keyOf(result.path[i - 1], result.path[i]));
            if (it == road.end()) reject(jury, "consecutive path vertices are not joined by a road");
            bottleneck = min(bottleneck, it->second.first);
            length += it->second.second;
        }
        if (bottleneck != best[sink]) reject(jury, "path does not maximize the allowed vehicle height");
        if (length != dist[sink]) reject(jury, "path is not shortest among maximum-height paths");
    };

    PathResult jury = readPath(ans, true, n);
    validate(jury, true);
    PathResult participant = readPath(ouf, false, n);
    validate(participant, false);

    quitf(_ok, "optimal path accepted");
}
