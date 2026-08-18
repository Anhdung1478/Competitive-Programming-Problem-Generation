#include "testlib.h"

int main(int argc, char* argv[]) {
    registerValidation(argc, argv);

    constexpr long long LIMIT = 1'000'000'000'000'000'000LL;
    inf.readLong(1, LIMIT, "r");
    inf.readSpace();
    inf.readLong(1, LIMIT, "c");
    inf.readSpace();
    inf.readLong(1, LIMIT, "k");
    inf.readEoln();
    inf.readEof();
    return 0;
}
