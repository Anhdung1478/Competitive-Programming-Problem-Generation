#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

// ---------------------------------------------------------------- shared numeric helper
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
        ll ans = genValue(low, high, "logarit");
        for (int i = 0; i < param; ++i) ans = min(ans, genValue(low, high, "logarit"));
        return ans;
    }

    quitf(_fail, "unknown value type: %s", type.c_str());
}

// ---------------------------------------------------------------- subtask limits
// Section 3 of generator-config.md. maxLen is the digit count of the subtask ceiling;
// a number of exactly maxLen digits is legal only for subtasks whose ceiling is a power
// of ten, and then only for that exact power.
int subtask, maxLen;
bool topIsPowerOfTen;   // true for every subtask whose ceiling is a power of ten

string powTen(int zeros) { return "1" + string(zeros, '0'); }

string subtaskCap() {
    if (topIsPowerOfTen) return powTen(maxLen - 1);
    return string(maxLen, '9');            // subtask 4: 500 digits, value cap is 10^500 - 1
}

// Length usable by a profile that wants free digit content: the topmost length is
// reserved for the exact power of ten.
int structLen(int k) {
    if (topIsPowerOfTen && k >= maxLen) k = maxLen - 1;
    return max(1, k);
}

// ---------------------------------------------------------------- digit helpers
int cmpNum(const string &a, const string &b) {
    if (a.size() != b.size()) return a.size() < b.size() ? -1 : 1;
    return a < b ? -1 : (a == b ? 0 : 1);
}

string randDigits(int k) {
    string s(k, '0');
    s[0] = char('1' + rnd.next(9));
    for (int i = 1; i < k; ++i) s[i] = char('0' + rnd.next(10));
    return s;
}

// ---------------------------------------------------------------- profiles
// Each returns one query (L, R) with L <= R. `k` is the target digit count of R,
// derived from rate in main().
typedef pair<string, string> Query;

Query profRandom(int k) {
    int n = structLen(k);                    // the reserved top length belongs to `boundary`
    string R = randDigits(n);
    string L = randDigits((int)genValue(1, n, "maximum"));
    if (cmpNum(L, R) > 0) swap(L, R);
    return {L, R};
}

Query profEqual(int k) {
    string x = randDigits(structLen(k));
    return {x, x};
}

Query profPrefix(int k) {
    int n = structLen(k);
    if (n < 2) return profEqual(k);
    int p = n / 2;
    string pre = randDigits(p);
    string L = pre, R = pre;
    for (int i = p; i < n; ++i) {
        L += char('0' + rnd.next(4));        // 0..3
        R += char('6' + rnd.next(4));        // 6..9
    }
    return {L, R};
}

// L = R = d c c ... c with d >= 2 > ... > c: power is n-1 while the maximum power over
// [1, R] is n, which is exactly what the prefix-subtraction WA reports with count 0.
Query profWaTrap(int k) {
    int n = max(2, structLen(k));
    if (topIsPowerOfTen && n > maxLen - 1) n = maxLen - 1;
    if (n < 2) return profEqual(k);
    char d = char('2' + rnd.next(8));        // 2..9
    char c = char('0' + rnd.next(d - '0'));  // 0..d-1
    string x = string(1, d) + string(n - 1, c);
    return {x, x};
}

Query profFlat(int k) {
    char d = char('1' + rnd.next(9));
    string x(structLen(k), d);
    return {x, x};
}

Query profDecreasing(int k) {
    int n = structLen(k);
    int blocks = min(n, (int)rnd.next(2, 10));
    vector<int> digs;                        // strictly decreasing block digits, top >= 1
    {
        vector<int> pool;
        for (int d = 9; d >= 1; --d) pool.push_back(d);
        shuffle(pool.begin(), pool.end());
        pool.resize(min<size_t>(pool.size(), blocks));
        sort(pool.rbegin(), pool.rend());
        digs = pool;
        if (rnd.next(2) && (int)digs.size() < 10) digs.push_back(0);
    }
    blocks = (int)digs.size();
    vector<int> len(blocks, 1);
    for (int rest = n - blocks; rest > 0; --rest) len[rnd.next(blocks)]++;
    string x;
    for (int i = 0; i < blocks; ++i) x += string(len[i], char('0' + digs[i]));
    x.resize(n);
    ensure(x[0] != '0');
    return {x, x};
}

Query profZeros(int k) {
    int n = structLen(k);
    string R(n, '0');
    R[0] = char('1' + rnd.next(9));
    for (int i = 1; i < n; ++i)
        if (rnd.next(10) == 0) R[i] = char('0' + rnd.next(10));
    string L = powTen(n - 1);                // smallest n-digit number
    ensure(cmpNum(L, R) <= 0);
    return {L, R};
}

// Cycles deterministically through the three extremes so all of them appear.
Query profBoundary(int idx) {
    string cap = subtaskCap();
    switch (idx % 3) {
        case 0: return {"1", "1"};
        case 1: return {"1", cap};
        default: return {cap, cap};
    }
}

// ---------------------------------------------------------------- validation
void check(const Query &q) {
    const string &L = q.first, &R = q.second;
    for (const string *s : {&L, &R}) {
        ensure(!s->empty());
        ensure((*s)[0] != '0');
        for (char ch : *s) ensure('0' <= ch && ch <= '9');
    }
    ensure(cmpNum(L, R) <= 0);
    ensure((int)R.size() <= maxLen);
    if (topIsPowerOfTen && (int)R.size() == maxLen) ensure(R == powTen(maxLen - 1));
    if (subtask == 4) ensure((int)R.size() <= 500);
    if (subtask == 3) {
        ensure(L == "1");
        ensure(R == powTen((int)R.size() - 1));
    }
}

int main(int argc, char *argv[]) {
    registerGen(argc, argv, 1);
    prepareOpts(argc, argv);

    subtask = opt<int>("subtask");
    double rate = opt<double>("rate");
    string profile = opt<string>("profile");

    ensure(1 <= subtask && subtask <= 5);
    ensure(0.70 <= rate && rate <= 1.00);

    static const int MAXLEN[6] = {0, 6, 12, 10001, 500, 10001};
    maxLen = MAXLEN[subtask];
    topIsPowerOfTen = (subtask != 4);   // subtasks 1, 2, 3, 5 are capped by a power of ten

    int numtest = has_opt("numtest")
                      ? opt<int>("numtest")
                      : min(20, max(1, (int)lround(20.0 * (rate - 0.50) / 0.50)));
    ensure(1 <= numtest && numtest <= 20);

    vector<Query> qs;

    if (subtask == 3) {
        // L = 1, R = 10^x. The profile is ignored: the shape is fully determined by x.
        static const int BOUND_X[5] = {0, 1, 2, 10000, 9999};
        int x = min(10000, max(0, (int)lround(10000.0 * (rate - 0.60) / 0.40)));
        for (int i = 0; i < numtest; ++i) {
            int xi = (profile == "boundary") ? BOUND_X[i % 5] : x;
            qs.push_back({"1", powTen(xi)});
        }
    } else {
        int k = min(maxLen, max(1, (int)lround(maxLen * (rate - 0.60) / 0.40)));
        static const char *POOL[] = {"random", "equal", "prefix", "wa-trap",
                                     "flat", "decreasing", "zeros"};
        for (int i = 0; i < numtest; ++i) {
            string p = (profile == "mixed") ? POOL[rnd.next(7)] : profile;
            if (p == "random") qs.push_back(profRandom(k));
            else if (p == "equal") qs.push_back(profEqual(k));
            else if (p == "prefix") qs.push_back(profPrefix(k));
            else if (p == "wa-trap") qs.push_back(profWaTrap(k));
            else if (p == "flat") qs.push_back(profFlat(k));
            else if (p == "decreasing") qs.push_back(profDecreasing(k));
            else if (p == "zeros") qs.push_back(profZeros(k));
            else if (p == "boundary") qs.push_back(profBoundary(i));
            else quitf(_fail, "unknown profile: %s", p.c_str());
        }
    }

    ensure((int)qs.size() == numtest);
    for (const Query &q : qs) check(q);

    cout << numtest << '\n';
    for (const Query &q : qs) cout << q.first << ' ' << q.second << '\n';
    return 0;
}
