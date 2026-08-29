#include "testlib.h"
using namespace std;

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    const long long LIMIT = 1000000000000000000LL; // 10^18, upper bound of R

    long long count_ouf = ouf.readLong(0LL, LIMIT, "count of lucky numbers");
    long long min_ouf   = ouf.readLong(-1LL, LIMIT, "smallest lucky number");

    long long count_ans = ans.readLong(0LL, LIMIT, "count of lucky numbers");
    long long min_ans   = ans.readLong(-1LL, LIMIT, "smallest lucky number");

    if (count_ouf != count_ans)
        quitf(_wa, "wrong count: expected %lld, found %lld", count_ans, count_ouf);

    if (min_ouf != min_ans)
        quitf(_wa, "wrong smallest lucky number: expected %lld, found %lld", min_ans, min_ouf);

    if (!ouf.seekEof())
        quitf(_wa, "extra output after the answer");

    quitf(_ok, "%lld lucky numbers, smallest %lld", count_ans, min_ans);
}
