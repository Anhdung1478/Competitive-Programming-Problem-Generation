#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]) {
    registerValidation(argc, argv);

    int n = inf.readInt(1, 200'000, "n");
    inf.readSpace();
    int k = inf.readInt(1, 17, "k");
    inf.readEoln();

    string s = inf.readToken("[a-q?]+", "S");
    inf.readEoln();

    ensuref((int)s.size() == n,
            "S must have length n: expected %d, found %d.", n, (int)s.size());
    for (int i = 0; i < n; ++i) {
        ensuref(s[i] == '?' || ('a' <= s[i] && s[i] < char('a' + k)),
                "S[%d] = '%c' is outside '?' and the first k lowercase letters.",
                i + 1, s[i]);
    }

    inf.readEof();
    return 0;
}
