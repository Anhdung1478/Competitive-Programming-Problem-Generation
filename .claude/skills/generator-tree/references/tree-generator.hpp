// Reference utility for generator-tree.
// Copy/adapt into gentest.cpp when needed; keep final Polygon generator self-contained.

#pragma once

#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

struct TreeGenerator {
    int n;

    explicit TreeGenerator(int n = 0) : n(n) {
        ensure(n >= 0);
    }

    vector<int> parent(const string& type) const {
        vector<int> par(n + 1, 0);
        if (n <= 1) return par;

        if (type == "line") {
            for (int i = 2; i <= n; ++i) par[i] = i - 1;
            return par;
        }

        if (type == "star") {
            for (int i = 2; i <= n; ++i) par[i] = 1;
            return par;
        }

        if (type == "binary") {
            for (int i = 2; i <= n; ++i) par[i] = i / 2;
            return par;
        }

        if (type == "random") {
            for (int i = 2; i <= n; ++i) par[i] = rnd.next(1, i - 1);
            return par;
        }

        if (type == "long") {
            for (int i = 2; i <= n; ++i) {
                par[i] = rnd.next(max(1, i - 7), i - 1);
            }
            return par;
        }

        if (type == "focused") {
            int hubCount = min(n, 7);
            vector<int> hubs = rnd.perm(n, 1);
            hubs.resize(hubCount);
            if (find(hubs.begin(), hubs.end(), 1) == hubs.end()) hubs.push_back(1);

            for (int i = 2; i <= n; ++i) {
                if (rnd.next(100) < 10) {
                    par[i] = rnd.next(1, i - 1);
                } else {
                    vector<int> valid;
                    for (int h : hubs) if (h < i) valid.push_back(h);
                    par[i] = valid.empty() ? rnd.next(1, i - 1) : rnd.any(valid);
                }
            }
            return par;
        }

        if (type == "mixed") {
            int x1 = max(1, (int)floor(0.45 * n));
            int x2 = max(x1, (int)floor(0.90 * n));

            for (int i = 2; i <= x1; ++i) par[i] = i - 1;

            vector<int> hubs;
            for (int i = 1; i <= x1; ++i) hubs.push_back(i);
            shuffle(hubs.begin(), hubs.end());
            hubs.resize(min<int>((int)hubs.size(), 7));

            for (int i = x1 + 1; i <= x2; ++i) par[i] = rnd.any(hubs);
            for (int i = x2 + 1; i <= n; ++i) par[i] = rnd.next(1, i - 1);

            return par;
        }

        if (type == "broom") {
            int pathLen = max(1, (int)floor(0.70 * n));
            for (int i = 2; i <= pathLen; ++i) par[i] = i - 1;
            for (int i = pathLen + 1; i <= n; ++i) par[i] = pathLen;
            return par;
        }

        if (type == "caterpillar") {
            int backbone = max(1, (int)floor(0.55 * n));
            for (int i = 2; i <= backbone; ++i) par[i] = i - 1;
            for (int i = backbone + 1; i <= n; ++i) {
                par[i] = rnd.next(1, backbone);
            }
            return par;
        }

        quitf(_fail, "unknown tree type: %s", type.c_str());
    }

    vector<pair<int,int>> edges(
        const string& type,
        bool relabelVertices = true,
        bool preserveRootOne = false,
        bool randomizeOrientation = true,
        bool randomizeEdgeOrder = true
    ) const {
        vector<int> par = parent(type);
        vector<int> label(n + 1);
        iota(label.begin(), label.end(), 0);

        if (relabelVertices && n >= 2) {
            if (preserveRootOne) {
                shuffle(label.begin() + 2, label.end());
            } else {
                shuffle(label.begin() + 1, label.end());
            }
        }

        vector<pair<int,int>> e;
        e.reserve(max(0, n - 1));

        for (int i = 2; i <= n; ++i) {
            ensure(1 <= par[i] && par[i] < i);
            e.emplace_back(label[par[i]], label[i]);
        }

        if (randomizeOrientation) {
            for (auto& [u, v] : e) {
                if (rnd.next(2)) swap(u, v);
            }
        }

        if (randomizeEdgeOrder) {
            shuffle(e.begin(), e.end());
        }

        ensure((int)e.size() == max(0, n - 1));
        for (auto [u, v] : e) {
            ensure(1 <= u && u <= n);
            ensure(1 <= v && v <= n);
            ensure(u != v);
        }

        return e;
    }
};
