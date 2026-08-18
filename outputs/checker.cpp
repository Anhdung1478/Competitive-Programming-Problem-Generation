#include "testlib.h"

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    long long expected = ans.readLong();
    long long found = ouf.readLong();

    if (!ans.seekEof()) {
        quitf(_fail, "jury output contains extra tokens");
    }
    if (!ouf.seekEof()) {
        quitf(_wa, "participant output contains extra tokens");
    }
    if (found != expected) {
        quitf(_wa, "expected %lld, found %lld", expected, found);
    }

    quitf(_ok, "answer is %lld", found);
}
