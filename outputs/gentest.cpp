#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

namespace {

constexpr int FULL_LIMIT = 100000;
constexpr int FULL_VALUE_CAP = 10000;

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
        const long double x = static_cast<long double>(low) *
            expl(static_cast<long double>(rnd.next()) *
                 logl(static_cast<long double>(high) /
                      static_cast<long double>(low)));
        const ll rounded = static_cast<ll>(floorl(x + 0.5L));
        return max(low, min(high, rounded));
    }
    if (type == "maximum") {
        return rnd.wnext(low, high, param);
    }
    if (type == "minimum") {
        ll answer = genValue(low, high, "logarit");
        for (int i = 0; i < param; ++i) {
            answer = min(answer, genValue(low, high, "logarit"));
        }
        return answer;
    }
    quitf(_fail, "unknown value type: %s", type.c_str());
}

int scaledMax(int limit, double rate) {
    return max(1, min(limit, static_cast<int>(floor(limit * rate))));
}

bool startsWith(const string& text, const string& prefix) {
    return text.size() >= prefix.size() &&
           equal(prefix.begin(), prefix.end(), text.begin());
}

bool contains(const string& text, const string& fragment) {
    return text.find(fragment) != string::npos;
}

bool isLucky(int value) {
    while (value > 0) {
        const int digit = value % 10;
        if (digit != 4 && digit != 7) {
            return false;
        }
        value /= 10;
    }
    return true;
}

void buildLucky(int value, int cap, vector<int>& lucky) {
    if (value > cap) {
        return;
    }
    if (value > 0) {
        lucky.push_back(value);
    }
    buildLucky(value * 10 + 4, cap, lucky);
    buildLucky(value * 10 + 7, cap, lucky);
}

class RangeMaximum {
public:
    explicit RangeMaximum(const vector<int>& values)
        : size_(static_cast<int>(values.size())), tree_(4 * size_ + 4),
          lazy_(4 * size_ + 4) {
        build(1, 0, size_ - 1, values);
    }

    int query(int left, int right) {
        return query(1, 0, size_ - 1, left, right);
    }

    void add(int left, int right, int delta) {
        add(1, 0, size_ - 1, left, right, delta);
    }

    int maximum() const {
        return tree_[1];
    }

private:
    int size_;
    vector<int> tree_;
    vector<int> lazy_;

    void build(int id, int left, int right, const vector<int>& values) {
        if (left == right) {
            tree_[id] = values[left];
            return;
        }
        const int middle = (left + right) / 2;
        build(id << 1, left, middle, values);
        build(id << 1 | 1, middle + 1, right, values);
        tree_[id] = max(tree_[id << 1], tree_[id << 1 | 1]);
    }

    void apply(int id, int delta) {
        tree_[id] += delta;
        lazy_[id] += delta;
    }

    void push(int id) {
        if (lazy_[id] == 0) {
            return;
        }
        apply(id << 1, lazy_[id]);
        apply(id << 1 | 1, lazy_[id]);
        lazy_[id] = 0;
    }

    int query(int id, int left, int right, int queryLeft, int queryRight) {
        if (queryLeft <= left && right <= queryRight) {
            return tree_[id];
        }
        push(id);
        const int middle = (left + right) / 2;
        int answer = 0;
        if (queryLeft <= middle) {
            answer = max(answer,
                         query(id << 1, left, middle, queryLeft, queryRight));
        }
        if (middle < queryRight) {
            answer = max(answer, query(id << 1 | 1, middle + 1, right,
                                       queryLeft, queryRight));
        }
        return answer;
    }

    void add(int id, int left, int right, int queryLeft, int queryRight,
             int delta) {
        if (queryLeft <= left && right <= queryRight) {
            apply(id, delta);
            return;
        }
        push(id);
        const int middle = (left + right) / 2;
        if (queryLeft <= middle) {
            add(id << 1, left, middle, queryLeft, queryRight, delta);
        }
        if (middle < queryRight) {
            add(id << 1 | 1, middle + 1, right, queryLeft, queryRight, delta);
        }
        tree_[id] = max(tree_[id << 1], tree_[id << 1 | 1]);
    }
};

struct Operation {
    bool isAdd;
    int left;
    int right;
    int delta;
};

pair<int, int> chooseInterval(const string& profile, int n, int index) {
    if (contains(profile, "point")) {
        const int position = (index % 4 == 0 ? 1 :
                              index % 4 == 1 ? n : rnd.next(1, n));
        return {position, position};
    }
    if (contains(profile, "full") || profile == "max-work" ||
        contains(profile, "threshold") || contains(profile, "staggered")) {
        return {1, n};
    }
    if (contains(profile, "nested")) {
        const int depthLimit = min((n - 1) / 2, 1000);
        const int depth = (depthLimit == 0 ? 0 : index % (depthLimit + 1));
        return {1 + depth, n - depth};
    }
    if (contains(profile, "alternating")) {
        if (index % 3 == 0) {
            return {1, max(1, n / 2)};
        }
        if (index % 3 == 1) {
            return {min(n, n / 2 + 1), n};
        }
        return {1, n};
    }

    int left = rnd.next(1, n);
    int right = rnd.next(1, n);
    if (left > right) {
        swap(left, right);
    }
    if (rnd.next(0, 4) == 0) {
        left = 1;
    }
    if (rnd.next(0, 4) == 0) {
        right = n;
    }
    return {left, right};
}

int nonLuckyValue(int cap) {
    int value = static_cast<int>(genValue(1, cap, valueTypes[rnd.next(0, 3)]));
    while (isLucky(value)) {
        value = (value == cap ? max(1, value - 1) : value + 1);
    }
    return value;
}

}  // namespace

int main(int argc, char* argv[]) {
    registerGen(argc, argv, 1);
    prepareOpts(argc, argv);

    const int subtask = opt<int>("subtask");
    const double rate = opt<double>("rate");
    const string profile = opt<string>("profile");

    ensure(1 <= subtask && subtask <= 4);
    ensure(0.70 <= rate && rate <= 1.00);

    const set<string> allowedProfiles = {
        "min-case", "random", "threshold-hit", "threshold-cross",
        "multi-threshold", "staggered-thresholds", "mixed-duplicates",
        "point-ops", "full-range", "nested-ranges", "alternating-ranges",
        "query-heavy", "update-heavy", "all-lucky", "no-lucky",
        "boundary-values", "wrong-killer", "max-work", "static-random",
        "static-all-lucky", "static-no-lucky", "static-mixed",
        "static-duplicates", "static-point", "static-full",
        "static-nested", "static-boundary", "small-random",
        "small-all-lucky", "small-no-lucky", "small-threshold-hit",
        "small-threshold-cross", "small-point", "small-full-range",
        "small-nested", "small-query-heavy", "small-update-heavy",
        "small-duplicates", "small-staggered", "small-boundary"
    };
    ensure(allowedProfiles.count(profile) == 1);

    const int sizeLimit = (subtask == 1 ? 1000 : FULL_LIMIT);
    const int valueCap = (subtask == 3 ? 100 : FULL_VALUE_CAP);
    const int nCeil = scaledMax(sizeLimit, rate);
    const int qCeil = scaledMax(sizeLimit, rate);

    int n;
    int q;
    if (profile == "min-case" || profile == "wrong-killer") {
        n = q = 1;
    } else if (profile == "max-work") {
        n = nCeil;
        q = qCeil;
    } else {
        const int nLow = max(1, nCeil / 2);
        const int qLow = max(1, qCeil / 2);
        n = static_cast<int>(genValue(nLow, nCeil, valueTypes[rnd.next(0, 3)]));
        q = static_cast<int>(genValue(qLow, qCeil, valueTypes[rnd.next(0, 3)]));
    }

    vector<int> lucky;
    buildLucky(0, valueCap, lucky);
    sort(lucky.begin(), lucky.end());

    vector<int> values(n);
    if (profile == "min-case" || profile == "wrong-killer") {
        fill(values.begin(), values.end(), 4);
    } else if (contains(profile, "all-lucky")) {
        for (int& value : values) {
            value = lucky[rnd.next(0, static_cast<int>(lucky.size()) - 1)];
        }
    } else if (contains(profile, "no-lucky")) {
        for (int& value : values) {
            value = nonLuckyValue(valueCap);
        }
    } else if (contains(profile, "threshold-hit")) {
        fill(values.begin(), values.end(), 3);
    } else if (contains(profile, "threshold-cross")) {
        fill(values.begin(), values.end(), 4);
    } else if (profile == "multi-threshold") {
        fill(values.begin(), values.end(), 1);
    } else if (contains(profile, "staggered")) {
        const int target = (valueCap == 100 ? 77 : 7777);
        for (int i = 0; i < n; ++i) {
            values[i] = max(1, target - i % 12);
        }
    } else if (contains(profile, "duplicates") || profile == "mixed-duplicates" ||
               profile == "static-mixed") {
        const vector<int> palette = (valueCap == 100)
            ? vector<int>{3, 4, 5, 7, 8, 44, 45, 47}
            : vector<int>{3, 4, 5, 7, 8, 44, 45, 47, 74, 77, 444, 777};
        for (int i = 0; i < n; ++i) {
            values[i] = palette[(i / max(1, n / 20)) % palette.size()];
        }
    } else if (contains(profile, "boundary")) {
        vector<int> palette{1, valueCap};
        for (int number : lucky) {
            palette.push_back(number);
            if (number > 1) {
                palette.push_back(number - 1);
            }
            if (number < valueCap) {
                palette.push_back(number + 1);
            }
        }
        for (int i = 0; i < n; ++i) {
            values[i] = palette[i % palette.size()];
        }
    } else if (profile == "max-work") {
        fill(values.begin(), values.end(), 1);
    } else {
        for (int& value : values) {
            value = static_cast<int>(
                genValue(1, valueCap, valueTypes[rnd.next(0, 3)]));
        }
    }

    RangeMaximum current(values);
    vector<Operation> operations;
    operations.reserve(q);

    const bool staticOnly = subtask == 2 || startsWith(profile, "static-");
    int countOperations = 0;

    for (int index = 0; index < q; ++index) {
        pair<int, int> interval = chooseInterval(profile, n, index);
        int left = interval.first;
        int right = interval.second;

        bool wantAdd = !staticOnly && index + 1 < q;
        if (profile == "min-case" || profile == "wrong-killer") {
            wantAdd = false;
        } else if (contains(profile, "query-heavy")) {
            wantAdd = (index % 10 == 0);
        } else if (contains(profile, "update-heavy")) {
            wantAdd = (index % 10 != 0);
        } else if (profile == "max-work") {
            wantAdd = index < min(q - 1, valueCap - 1);
            left = 1;
            right = n;
        } else if (index == 0 && (contains(profile, "threshold") ||
                                  contains(profile, "staggered") ||
                                  profile == "multi-threshold")) {
            wantAdd = true;
            left = 1;
            right = n;
        } else {
            wantAdd = rnd.next(0, 99) < 55;
        }

        // Exact subtask restrictions and the reserved final answer-producing
        // query take precedence over every profile-specific probability.
        if (staticOnly || index + 1 == q) {
            wantAdd = false;
        }

        const int rangeMaximum = current.query(left - 1, right - 1);
        const int budget = valueCap - rangeMaximum;
        if (wantAdd && budget > 0) {
            int delta;
            if (profile == "max-work") {
                delta = 1;
            } else if (index == 0 && contains(profile, "threshold-hit")) {
                delta = 1;
            } else if (index == 0 && contains(profile, "threshold-cross")) {
                delta = 1;
            } else if (index == 0 && profile == "multi-threshold") {
                delta = min(50, budget);
            } else if (index == 0 && contains(profile, "staggered")) {
                delta = 1;
            } else {
                const int deltaCap = min(budget,
                    contains(profile, "update-heavy") ? 10 : 1000);
                delta = static_cast<int>(genValue(
                    1, deltaCap, valueTypes[rnd.next(0, 3)]));
            }
            operations.push_back({true, left, right, delta});
            current.add(left - 1, right - 1, delta);
        } else {
            operations.push_back({false, left, right, 0});
            ++countOperations;
        }
    }

    if (countOperations == 0) {
        operations.back() = {false, 1, n, 0};
        ++countOperations;
    }

    ensure(1 <= n && n <= sizeLimit);
    ensure(1 <= q && q <= sizeLimit);
    ensure(static_cast<int>(values.size()) == n);
    ensure(static_cast<int>(operations.size()) == q);
    ensure(countOperations >= 1);
    for (int value : values) {
        ensure(1 <= value && value <= valueCap);
    }
    for (const Operation& operation : operations) {
        ensure(1 <= operation.left && operation.left <= operation.right &&
               operation.right <= n);
        if (operation.isAdd) {
            ensure(1 <= operation.delta && operation.delta <= FULL_VALUE_CAP);
        }
    }
    ensure(current.maximum() <= valueCap);
    if (subtask == 2) {
        for (const Operation& operation : operations) {
            ensure(!operation.isAdd);
        }
    }
    if (profile == "wrong-killer") {
        ensure(n == 1 && q == 1 && values[0] == 4 && !operations[0].isAdd);
    }

    cout << n << ' ' << q << '\n';
    for (int i = 0; i < n; ++i) {
        if (i > 0) {
            cout << ' ';
        }
        cout << values[i];
    }
    cout << '\n';

    for (const Operation& operation : operations) {
        if (operation.isAdd) {
            cout << "add " << operation.left << ' ' << operation.right << ' '
                 << operation.delta << '\n';
        } else {
            cout << "count " << operation.left << ' ' << operation.right << '\n';
        }
    }

    return 0;
}
