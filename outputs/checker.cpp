// Checker for "Ếch ham ăn" (jumping-frog).
//
// Output is deterministic: for every type-2 query the answer is the pair
// (final position, number of flies eaten), both uniquely determined by the
// input. So the check is a strict token-by-token comparison against the jury
// answer, plus rejection of missing / surplus participant tokens.
//
// Bounds used by the readers:
//   final position  <= x + sum of all fly positions <= 1e9 + 2e5 * 1e9
//                    = 200001000000000, and >= x >= 1;
//   flies eaten     in [0, N] with N <= 2e5.
#include "testlib.h"

#include <string>

const int MAXN = 200000;
const long long MAX_POS = 200001000000000LL; // 1e9 + 2e5 * 1e9

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    // Count the type-2 queries in the input, so a jury answer of the wrong
    // length is reported as a package inconsistency rather than as a
    // participant error.
    int n = inf.readInt(1, MAXN, "N");
    int q = inf.readInt(1, MAXN, "Q");
    for (int i = 1; i <= n; ++i) inf.readInt(1, 1000000000, "x_i");

    int expectedLines = 0;
    for (int t = 0; t < q; ++t) {
        int type = inf.readInt(1, 2, "query type");
        if (type == 1) {
            inf.readInt(1, n, "u");
            inf.readInt(1, 1000000000, "y");
        } else {
            inf.readInt(1, 1000000000, "x");
            ++expectedLines;
        }
    }

    for (int line = 1; line <= expectedLines; ++line) {
        long long juryPos = ans.readLong(1LL, MAX_POS, "final position");
        int juryCnt = ans.readInt(0, MAXN, "number of flies eaten");

        if (ouf.seekEof())
            quitf(_wa, "dòng %d: thiếu kết quả, người dự thi chỉ in ra %d dòng",
                  line, line - 1);

        long long outPos = ouf.readLong(1LL, MAX_POS, "final position");
        int outCnt = ouf.readInt(0, MAXN, "number of flies eaten");

        if (outPos != juryPos)
            quitf(_wa, "dòng %d: vị trí cuối cùng sai, người dự thi = %lld, đáp án = %lld",
                  line, outPos, juryPos);
        if (outCnt != juryCnt)
            quitf(_wa, "dòng %d: số ruồi đã ăn sai, người dự thi = %d, đáp án = %d",
                  line, outCnt, juryCnt);
    }

    if (!ans.seekEof())
        quitf(_fail, "đáp án của ban giám khảo có nhiều hơn %d dòng", expectedLines);
    if (!ouf.seekEof())
        quitf(_wa, "người dự thi in ra nhiều hơn %d dòng kết quả", expectedLines);

    quitf(_ok, "%d truy vấn loại 2 đúng", expectedLines);
}
