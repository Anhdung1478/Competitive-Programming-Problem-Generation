#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char *argv[]) {
    registerValidation(argc, argv);

    int n = inf.readInt(1, 100'000, "N");
    inf.readSpace();
    inf.readInt(1, n, "X");
    inf.readSpace();
    inf.readInt(1, n, "Y");
    inf.readEoln();

    int m = inf.readInt(0, 100'000, "M");
    inf.readEoln();

    long long simpleCapacity = 1LL * n * (n - 1) / 2;
    ensuref(m <= simpleCapacity, "M=%d exceeds the simple-graph capacity for N=%d", m, n);

    set<pair<int, int>> roads;
    for (int id = 1; id <= m; ++id) {
        int u = inf.readInt(1, n, "i");
        inf.readSpace();
        int v = inf.readInt(1, n, "j");
        inf.readSpace();
        inf.readInt(1, 10'000, "H");
        inf.readSpace();
        inf.readInt(1, 10'000, "D");
        inf.readEoln();

        ensuref(u != v, "road %d is a self-loop at vertex %d", id, u);
        if (u > v) swap(u, v);
        ensuref(roads.insert({u, v}).second,
                "road %d duplicates the unordered pair (%d,%d)", id, u, v);
    }

    inf.readEof();
    return 0;
}
