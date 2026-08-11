#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    int tokenId = 0;
    while (!ans.seekEof()) {
        ++tokenId;

        if (ouf.seekEof()) {
            quitf(_wa, "participant output ended before token %d", tokenId);
        }

        string expected = ans.readToken();
        string found = ouf.readToken();

        if (found != expected) {
            quitf(
                _wa,
                "token %d differs: expected '%s', found '%s'",
                tokenId,
                expected.c_str(),
                found.c_str()
            );
        }
    }

    if (!ouf.seekEof()) {
        string extra = ouf.readToken();
        quitf(_wa, "extra participant output starts with '%s'", extra.c_str());
    }

    quitf(_ok, "outputs match");
}
