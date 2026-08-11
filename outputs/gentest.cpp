#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

const ll MAX_N = 1000000000LL;
const ll MAX_M = 1000000000000000000LL;
const int MAX_K = 10000;

vector<string> valueTypes({"minimum", "uniform", "logarit", "maximum"});

ll genValue(ll low, ll high, const string& type, int param = 3) {
    ensure(low <= high);

    if (low != 1) {
        return low - 1 + genValue(1, high - low + 1, type, param);
    }

    if (type == "uniform") {
        return rnd.next(low, high);
    }

    if (type == "logarit") {
        long double x = (long double)low * expl(
            (long double)rnd.next() * logl((long double)high / (long double)low)
        );
        ll rounded = (ll)floorl(x + 0.5L);
        return max(low, min(high, rounded));
    }

    if (type == "maximum") {
        return rnd.wnext(low, high, param);
    }

    if (type == "minimum") {
        ll result = genValue(low, high, "logarit");
        for (int i = 0; i < param; ++i) {
            result = min(result, genValue(low, high, "logarit"));
        }
        return result;
    }

    quitf(_fail, "unknown value type: %s", type.c_str());
}

ll rateCeiling(ll low, ll high, double rate) {
    ensure(1 <= low && low <= high);
    long double value = (long double)low
        + floorl((long double)(high - low) * (long double)rate);
    value = max((long double)low, min((long double)high, value));
    return (ll)value;
}

vector<ll> sampleUniqueRange(ll low, ll high, int count) {
    ensure(count >= 0);
    if (count == 0) return {};
    ensure(low <= high);

    ll length = high - low + 1;
    ensure((ll)count <= length);

    set<ll> selected;
    for (ll j = length - count + 1; j <= length; ++j) {
        ll candidate = rnd.next(1LL, j);
        if (selected.count(candidate)) selected.insert(j);
        else selected.insert(candidate);
    }

    vector<ll> result;
    result.reserve(count);
    for (ll value : selected) result.push_back(low + value - 1);
    return result;
}

bool isOneOf(const string& value, initializer_list<const char*> choices) {
    for (const char* choice : choices) {
        if (value == choice) return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    registerGen(argc, argv, 1);
    prepareOpts(argc, argv);

    int subtask = opt<int>("subtask");
    double rate = opt<double>("rate");
    string profile = opt<string>("profile");

    ensure(1 <= subtask && subtask <= 5);
    ensure(0.70 <= rate && rate <= 1.00);

    const set<string> allowedProfiles = {
        "random", "sparse", "dense", "no-fixed",
        "all-fixed-valid", "all-fixed-conflict", "edge-days",
        "n-one", "n-two", "same-endpoints", "different-endpoints",
        "max-values", "long-gap-same", "long-gap-different",
        "free-random", "free-n-one", "free-n-two", "free-max"
    };
    ensure(allowedProfiles.count(profile));

    bool maximumProfile = isOneOf(profile, {"max-values", "free-max"});
    string sizeType = maximumProfile ? "maximum" : valueTypes[rnd.next(0, 3)];

    ll n = 1;
    ll m = 1;
    ll productCap = max(1LL, (ll)floor(1000000.0L * (long double)rate));

    if (subtask == 1) {
        int shape = rnd.next(0, 2);
        if (shape == 0) {
            n = genValue(1, min(MAX_N, productCap), sizeType);
            m = genValue(1, productCap / n, sizeType);
        } else if (shape == 1) {
            m = genValue(1, productCap, sizeType);
            n = genValue(1, min(MAX_N, productCap / m), sizeType);
        } else {
            ll middle = max(1LL, (ll)sqrt((long double)productCap));
            n = genValue(1, middle, "maximum");
            m = genValue(1, productCap / n, "maximum");
        }
    } else if (subtask == 2) {
        n = genValue(1, rateCeiling(1, MAX_N, rate), sizeType);
        m = 2;
    } else {
        n = genValue(1, rateCeiling(1, MAX_N, rate), sizeType);
        ll mLimit = (subtask == 4 ? 10 : MAX_M);
        m = genValue(1, rateCeiling(1, mLimit, rate), sizeType);
    }

    if (isOneOf(profile, {"n-one", "free-n-one"})) n = 1;
    if (isOneOf(profile, {"n-two", "free-n-two"})) n = 2;

    bool needsTwoDays = isOneOf(profile, {
        "all-fixed-conflict", "same-endpoints", "different-endpoints",
        "long-gap-same", "long-gap-different"
    });
    bool needsTwoCandies = isOneOf(profile, {
        "all-fixed-valid", "different-endpoints", "long-gap-different"
    });

    if (needsTwoCandies) n = max(2LL, n);
    if (needsTwoDays) m = max(2LL, m);

    if (maximumProfile) {
        if (subtask != 1) {
            ll nLimit = rateCeiling(1, MAX_N, rate);
            ll mLimit = (subtask == 2 ? 2 : (subtask == 4 ? 10 : MAX_M));
            mLimit = rateCeiling(1, mLimit, rate);
            n = genValue(max(1LL, nLimit - min(1000000LL, nLimit - 1)), nLimit, "maximum");
            m = (subtask == 2 ? 2 : genValue(max(1LL, mLimit - min(1000000LL, mLimit - 1)), mLimit, "maximum"));
        }
    }

    if (isOneOf(profile, {"long-gap-same", "long-gap-different"}) && subtask != 1) {
        ll mLimit = (subtask == 2 ? 2 : (subtask == 4 ? 10 : MAX_M));
        m = rateCeiling(1, mLimit, rate);
        m = max(2LL, m);
    }

    if (isOneOf(profile, {"all-fixed-valid", "all-fixed-conflict"})) {
        m = min(m, (ll)MAX_K);
        if (profile == "all-fixed-conflict") m = max(2LL, m);
    }

    if (subtask == 1) {
        if (needsTwoDays) {
            n = min(n, productCap / 2);
            n = max(n, needsTwoCandies ? 2LL : 1LL);
            m = max(2LL, min(m, productCap / n));
        } else {
            m = min(m, productCap / n);
            m = max(1LL, m);
        }
    }
    if (subtask == 2) m = 2;
    if (subtask == 4) m = min(m, 10LL);

    int kCap = (int)min<ll>(m, MAX_K);
    int k = 0;

    bool freeProfile = isOneOf(profile, {
        "no-fixed", "free-random", "free-n-one", "free-n-two", "free-max"
    });

    if (subtask == 3 || freeProfile) {
        k = 0;
    } else if (isOneOf(profile, {"all-fixed-valid", "all-fixed-conflict"})) {
        k = (int)m;
    } else if (isOneOf(profile, {
        "same-endpoints", "different-endpoints", "long-gap-same", "long-gap-different"
    })) {
        k = 2;
    } else if (profile == "sparse") {
        k = rnd.next(1, min(kCap, 10));
    } else if (profile == "dense") {
        int low = max(1, (int)((4LL * kCap + 4) / 5));
        k = rnd.next(low, kCap);
    } else if (profile == "edge-days") {
        int minimum = (m == 1 ? 1 : 2);
        k = rnd.next(minimum, min(kCap, 12));
    } else if (profile == "max-values") {
        int low = max(1, (9 * kCap + 9) / 10);
        k = rnd.next(low, kCap);
    } else {
        string kType = valueTypes[rnd.next(0, 3)];
        k = (int)genValue(0, kCap, kType);
    }

    vector<ll> days;
    if (k == 0) {
        days.clear();
    } else if (isOneOf(profile, {"all-fixed-valid", "all-fixed-conflict"})) {
        days.resize(k);
        iota(days.begin(), days.end(), 1LL);
    } else if (isOneOf(profile, {
        "same-endpoints", "different-endpoints", "long-gap-same", "long-gap-different"
    })) {
        days = {1, m};
    } else if (profile == "edge-days") {
        days.push_back(1);
        if (m > 1) days.push_back(m);
        int internal = k - (int)days.size();
        if (internal > 0) {
            vector<ll> middle = sampleUniqueRange(2, m - 1, internal);
            days.insert(days.end(), middle.begin(), middle.end());
            sort(days.begin(), days.end());
        }
    } else {
        days = sampleUniqueRange(1, m, k);
    }

    vector<ll> candies(k, 1);
    if (profile == "all-fixed-valid") {
        for (int i = 0; i < k; ++i) candies[i] = 1 + i % n;
    } else if (profile == "all-fixed-conflict") {
        for (int i = 0; i < k; ++i) candies[i] = genValue(1, n, "uniform");
        candies[1] = candies[0];
    } else if (isOneOf(profile, {"same-endpoints", "long-gap-same"})) {
        candies[0] = candies[1] = genValue(1, n, "uniform");
    } else if (isOneOf(profile, {"different-endpoints", "long-gap-different"})) {
        candies[0] = 1;
        candies[1] = n;
    } else if (profile == "dense" && k > 0) {
        int pattern = rnd.next(0, 2);
        if (pattern == 0) {
            fill(candies.begin(), candies.end(), genValue(1, n, "uniform"));
        } else if (pattern == 1) {
            for (int i = 0; i < k; ++i) candies[i] = (n == 1 ? 1 : 1 + (i & 1) * (n - 1));
        } else {
            ll alphabet = min(n, 3LL);
            for (ll& candy : candies) candy = genValue(1, alphabet, "uniform");
        }
    } else {
        for (ll& candy : candies) {
            candy = genValue(1, n, valueTypes[rnd.next(0, 3)]);
        }
        if (isOneOf(profile, {"edge-days", "max-values"}) && k >= 1) candies[0] = 1;
        if (isOneOf(profile, {"edge-days", "max-values"}) && k >= 2) candies[k - 1] = n;
    }

    ensure(1 <= n && n <= MAX_N);
    ensure(1 <= m && m <= MAX_M);
    ensure(0 <= k && k <= min<ll>(m, MAX_K));
    ensure((int)days.size() == k && (int)candies.size() == k);
    for (int i = 0; i < k; ++i) {
        ensure(1 <= days[i] && days[i] <= m);
        ensure(1 <= candies[i] && candies[i] <= n);
        if (i > 0) ensure(days[i - 1] < days[i]);
    }

    if (subtask == 1) ensure(m <= 1000000LL / n);
    if (subtask == 2) ensure(m == 2);
    if (subtask == 3) ensure(k == 0);
    if (subtask == 4) ensure(m <= 10);

    if (profile == "all-fixed-valid") {
        ensure(k == m);
        for (int i = 1; i < k; ++i) ensure(candies[i - 1] != candies[i]);
    }
    if (profile == "all-fixed-conflict") {
        ensure(k == m && k >= 2 && candies[0] == candies[1]);
    }
    if (isOneOf(profile, {"same-endpoints", "long-gap-same"})) {
        ensure(k == 2 && days[0] == 1 && days[1] == m && candies[0] == candies[1]);
    }
    if (isOneOf(profile, {"different-endpoints", "long-gap-different"})) {
        ensure(k == 2 && days[0] == 1 && days[1] == m && candies[0] != candies[1]);
    }

    cout << n << ' ' << m << ' ' << k << '\n';
    for (int i = 0; i < k; ++i) {
        cout << days[i] << ' ' << candies[i] << '\n';
    }

    return 0;
}
