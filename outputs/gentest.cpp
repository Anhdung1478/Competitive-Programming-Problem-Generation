#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

struct Edge {
    int u, v, h, d;
};

vector<string> valueTypes({"minimum", "uniform", "logarit", "maximum"});

ll genValue(ll low, ll high, const string &type, int param = 3) {
    ensure(low <= high);
    if (low != 1) return low - 1 + genValue(1, high - low + 1, type, param);
    if (type == "uniform") return rnd.next(low, high);
    if (type == "logarit") {
        double x = low * exp(rnd.next() * log(1.0 * high / low));
        return (ll)max<double>(low, min<double>(high, x + 0.5));
    }
    if (type == "maximum") return rnd.wnext(low, high, param);
    if (type == "minimum") {
        ll answer = genValue(low, high, "logarit");
        for (int i = 0; i < param; ++i) answer = min(answer, genValue(low, high, "logarit"));
        return answer;
    }
    quitf(_fail, "unknown value type: %s", type.c_str());
}

ll scaledMax(ll limit, double rate) {
    ensure(limit >= 1);
    ll value = (ll)floor(limit * rate + 0.5);
    return max(1LL, min(limit, value));
}

int main(int argc, char *argv[]) {
    registerGen(argc, argv, 1);
    prepareOpts(argc, argv);

    int subtask = opt<int>("subtask");
    double rate = opt<double>("rate");
    string profile = opt<string>("profile");

    ensure(1 <= subtask && subtask <= 4);
    ensure(0.70 <= rate && rate <= 1.00);

    const set<string> allProfiles = {
        "minimum", "x-equals-y", "disconnected", "tree-random", "tree-path",
        "tree-star", "tree-balanced", "tree-broom", "random-sparse", "dense", "height-ties", "distance-ties",
        "competing-paths", "threshold-chain", "near-limit"
    };
    ensure(allProfiles.count(profile));
    if (subtask == 1) {
        ensure(profile != "disconnected" && profile != "random-sparse" &&
               profile != "dense" && profile != "competing-paths");
    }
    if (subtask == 2 || subtask == 3) {
        ensure(profile != "tree-random" && profile != "tree-path" && profile != "tree-star" &&
               profile != "tree-balanced" && profile != "tree-broom");
    }

    const int nLimit = (subtask == 2 ? 200 : 100'000);
    const int mLimit = (subtask == 2 ? 200 : 100'000);
    const int nEnvelope = (int)scaledMax(nLimit, rate);
    const int mEnvelope = (int)floor(mLimit * rate + 0.5);

    int n = 1;
    int source = 1;
    int sink = 1;
    vector<Edge> edges;
    unordered_set<unsigned long long> used;
    used.reserve((size_t)mLimit * 2 + 1);

    auto edgeKey = [](int u, int v) -> unsigned long long {
        if (u > v) swap(u, v);
        return (static_cast<unsigned long long>(static_cast<unsigned int>(u)) << 32) |
               static_cast<unsigned int>(v);
    };

    auto randomWeight = [&]() -> int {
        return (int)genValue(1, 10'000, valueTypes[rnd.next(0, (int)valueTypes.size() - 1)]);
    };

    auto addEdge = [&](int u, int v, int h = -1, int d = -1) {
        ensure(u != v);
        unsigned long long key = edgeKey(u, v);
        ensure(!used.count(key));
        used.insert(key);
        int id = (int)edges.size();
        if (h == -1) {
            if (profile == "height-ties") h = (id % 5 == 0 ? 1 : 10'000);
            else h = randomWeight();
        }
        if (d == -1) {
            if (subtask == 3) d = 1;
            else if (profile == "distance-ties") d = (id % 5 == 0 ? 10'000 : 1);
            else d = randomWeight();
        }
        if (subtask == 3) d = 1;
        edges.push_back({u, v, h, d});
    };

    auto nearEnvelopeN = [&]() -> int {
        int low = max(1, (int)floor(nEnvelope * 0.85));
        return rnd.next(low, nEnvelope);
    };

    auto addTree = [&](const string &shape) {
        for (int v = 2; v <= n; ++v) {
            int parent;
            if (shape == "tree-path") parent = v - 1;
            else if (shape == "tree-star") parent = 1;
            else if (shape == "tree-balanced") parent = v / 2;
            else if (shape == "tree-broom") {
                int pathLength = max(1, (int)floor(0.70 * n));
                parent = (v <= pathLength ? v - 1 : pathLength);
            }
            else parent = rnd.next(1, v - 1);
            addEdge(parent, v);
        }
    };

    auto capacity = [](int count) -> ll {
        return 1LL * count * (count - 1) / 2;
    };

    auto addRandomWithin = [&](int lowVertex, int highVertex, int targetTotal) {
        int count = max(0, highVertex - lowVertex + 1);
        ensure((ll)targetTotal - (ll)edges.size() <= capacity(count));
        ll cap = capacity(count);
        if (cap <= 300'000) {
            vector<pair<int, int>> candidates;
            candidates.reserve((size_t)cap);
            for (int u = lowVertex; u <= highVertex; ++u) {
                for (int v = u + 1; v <= highVertex; ++v) {
                    if (!used.count(edgeKey(u, v))) candidates.push_back({u, v});
                }
            }
            shuffle(candidates.begin(), candidates.end());
            for (auto [u, v] : candidates) {
                if ((int)edges.size() == targetTotal) break;
                addEdge(u, v);
            }
        } else {
            while ((int)edges.size() < targetTotal) {
                int u = rnd.next(lowVertex, highVertex);
                int v = rnd.next(lowVertex, highVertex);
                if (u == v || used.count(edgeKey(u, v))) continue;
                addEdge(u, v);
            }
        }
        ensure((int)edges.size() == targetTotal);
    };

    if (profile == "minimum") {
        n = 1;
        source = sink = 1;
    } else if (profile == "threshold-chain") {
        n = max(2, nearEnvelopeN());
        source = 1;
        sink = n;
        for (int v = 2; v <= n; ++v) {
            int h = 10'000 - ((v - 2) % 10'000);
            if (v == n) h = 1;
            addEdge(v - 1, v, h, subtask == 3 ? 1 : randomWeight());
        }
    } else if (profile == "competing-paths") {
        n = max(4, nearEnvelopeN());
        source = 1;
        sink = 4;
        int longDistance = (subtask == 3 ? 1 : 10'000);
        addEdge(1, 2, 10'000, longDistance);
        addEdge(2, 3, 10'000, longDistance);
        addEdge(3, 4, 10'000, longDistance);
        addEdge(1, 4, 9'999, 1);
        if (n >= 6) {
            int desired = min(mEnvelope, (int)min<ll>(mLimit, 4 + capacity(n - 4)));
            desired = max(4, desired);
            addRandomWithin(5, n, desired);
        }
    } else if (subtask == 1 || profile == "tree-random" || profile == "tree-path" ||
               profile == "tree-star" || profile == "tree-balanced" || profile == "tree-broom") {
        n = nearEnvelopeN();
        if (profile == "x-equals-y") source = sink = rnd.next(1, n);
        else {
            source = 1;
            sink = n;
        }
        string shape = profile;
        if (shape != "tree-path" && shape != "tree-star" && shape != "tree-balanced" &&
            shape != "tree-broom") shape = "tree-random";
        addTree(shape);
    } else if (profile == "disconnected") {
        n = max(2, nearEnvelopeN());
        source = 1;
        sink = n;
        int middle = n / 2;
        int target = min(mEnvelope, (int)min<ll>(mLimit, capacity(middle) + capacity(n - middle)));
        if (middle >= 2 && target >= middle - 1) {
            for (int v = 2; v <= middle; ++v) addEdge(v - 1, v);
        }
        if (n - middle >= 2 && target - (int)edges.size() >= n - middle - 1) {
            for (int v = middle + 2; v <= n; ++v) addEdge(v - 1, v);
        }
        while ((int)edges.size() < target) {
            bool left = rnd.next(0, 1);
            int lo = left ? 1 : middle + 1;
            int hi = left ? middle : n;
            if (hi - lo + 1 < 2) continue;
            int u = rnd.next(lo, hi), v = rnd.next(lo, hi);
            if (u == v || used.count(edgeKey(u, v))) continue;
            addEdge(u, v);
        }
    } else {
        int target;
        if (profile == "dense") {
            target = max(1, mEnvelope);
            n = 2;
            while (n < nEnvelope && capacity(n) < target) ++n;
            target = min(target, (int)capacity(n));
        } else {
            n = nearEnvelopeN();
            ll cap = capacity(n);
            if (profile == "random-sparse") target = min(mEnvelope, (int)min<ll>(cap, 2LL * n));
            else target = min(mEnvelope, (int)min<ll>(cap, mLimit));
        }
        source = 1;
        sink = n;
        if (profile == "x-equals-y") source = sink = rnd.next(1, n);
        if (target >= n - 1 && n >= 2) addTree("tree-random");
        addRandomWithin(1, n, target);
    }

    shuffle(edges.begin(), edges.end());
    for (Edge &edge : edges) {
        if (rnd.next(0, 1)) swap(edge.u, edge.v);
    }

    ensure(1 <= n && n <= nLimit);
    ensure(1 <= source && source <= n && 1 <= sink && sink <= n);
    ensure(0 <= (int)edges.size() && (int)edges.size() <= mLimit);
    ensure((ll)edges.size() <= capacity(n));
    set<unsigned long long> finalPairs;
    vector<vector<int>> graph(n + 1);
    for (const Edge &edge : edges) {
        ensure(1 <= edge.u && edge.u <= n && 1 <= edge.v && edge.v <= n);
        ensure(edge.u != edge.v);
        ensure(1 <= edge.h && edge.h <= 10'000);
        ensure(1 <= edge.d && edge.d <= 10'000);
        ensure(finalPairs.insert(edgeKey(edge.u, edge.v)).second);
        graph[edge.u].push_back(edge.v);
        graph[edge.v].push_back(edge.u);
        if (subtask == 3) ensure(edge.d == 1);
    }
    if (subtask == 1) {
        ensure((int)edges.size() == n - 1);
        vector<char> seen(n + 1, false);
        queue<int> queue;
        queue.push(1);
        seen[1] = true;
        int reached = 0;
        while (!queue.empty()) {
            int u = queue.front();
            queue.pop();
            ++reached;
            for (int v : graph[u]) if (!seen[v]) {
                seen[v] = true;
                queue.push(v);
            }
        }
        ensure(reached == n);
    }
    if (subtask == 2) ensure(n <= 200 && (int)edges.size() <= 200);

    cout << n << ' ' << source << ' ' << sink << '\n';
    cout << edges.size() << '\n';
    for (const Edge &edge : edges) {
        cout << edge.u << ' ' << edge.v << ' ' << edge.h << ' ' << edge.d << '\n';
    }
    return 0;
}
