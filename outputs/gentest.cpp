// Generator for "Ếch ham ăn" (jumping-frog).
//
// Invocation (see outputs/test-script.txt):
//   gentest <SEED> --subtask <S> --rate <R> [--profile P] [--qmix M] [--xprofile X]
//
// The seed is the positional first argument consumed by registerGen; there is
// deliberately no --seed option. Every concrete input value is chosen here; the
// script only ever passes semantic profiles. See outputs/generator-config.md
// for the contract this file implements.
#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

const ll XMAX = 1000000000LL; // cap on a type-2 query's x, in every subtask

// ---------------------------------------------------------------------------
// shared numeric distribution
// ---------------------------------------------------------------------------
vector<string> valueTypes({"minimum", "uniform", "logarit", "maximum"});

ll genValue(ll low, ll high, const string &type, int param = 3) {
    ensure(low <= high);

    if (low != 1) {
        return low - 1 + genValue(1, high - low + 1, type, param);
    }

    if (type == "uniform") {
        return rnd.next(low, high);
    }

    if (type == "logarit") {
        double x = low * exp(rnd.next() * log(1.0 * high / low));
        return (ll)max<double>(low, min<double>(high, x + 0.5));
    }

    if (type == "maximum") {
        return rnd.wnext(low, high, param);
    }

    if (type == "minimum") {
        ll ans = genValue(low, high, "logarit");
        for (int i = 0; i < param; ++i) {
            ans = min(ans, genValue(low, high, "logarit"));
        }
        return ans;
    }

    quitf(_fail, "unknown value type: %s", type.c_str());
}

// ---------------------------------------------------------------------------
// subtask limits (outputs/generator-config.md section 3)
// ---------------------------------------------------------------------------
int sizeCap(int subtask) {
    switch (subtask) {
        case 1: return 200;
        case 2: return 2000;
        case 3: return 200000;
        case 4: return 200000;
        case 5: return 50000;
        case 6: return 200000;
    }
    quitf(_fail, "unknown subtask %d", subtask);
}

// cap on FLY positions: subtask.md bounds the flies, not the frog's x
ll posCap(int subtask) {
    switch (subtask) {
        case 3: return 100;
        case 4: return 100000;
        case 1: case 2: case 5: case 6: return XMAX;
    }
    quitf(_fail, "unknown subtask %d", subtask);
}

int scaledSize(int cap, double rate) {
    ensure(cap >= 1);
    double jitter = 0.95 + rnd.next() * 0.05; // independent per quantity
    ll x = (ll)floor(cap * rate * jitter + 0.5);
    return (int)max<ll>(1, min<ll>(cap, x));
}

int main(int argc, char *argv[]) {
    registerGen(argc, argv, 1);
    prepareOpts(argc, argv);

    int subtask = opt<int>("subtask");
    double rate = opt<double>("rate");
    string profile = opt<string>("profile", "random");
    string qmix = opt<string>("qmix", "balanced");
    string xprofile = opt<string>("xprofile", "random");

    ensure(1 <= subtask && subtask <= 6);
    ensure(0.70 <= rate && rate <= 1.00);

    const int cap = sizeCap(subtask);
    const ll C = posCap(subtask);
    ensure(C == (subtask == 3 ? 100LL : subtask == 4 ? 100000LL : XMAX));

    int n, q;
    if (profile == "minimum") {
        n = q = 1; // degenerate boundary; rate deliberately ignored
    } else {
        n = scaledSize(cap, rate);
        q = scaledSize(cap, rate);
    }

    // one value-type per test, so large tests keep value-profile diversity
    const string vtype = valueTypes[rnd.next((int)valueTypes.size())];

    // -----------------------------------------------------------------------
    // initial fly positions
    // -----------------------------------------------------------------------
    vector<ll> pos(n + 1);

    // context kept per profile so that type-1 targets preserve the shape
    vector<ll> dupValues;   // profile dup
    vector<ll> centers;     // profile cluster
    ll clusterWidth = 1;

    if (profile == "random" || profile == "minimum") {
        for (int i = 1; i <= n; ++i) pos[i] = genValue(1, C, vtype);
    } else if (profile == "chain" || profile == "collide") {
        // Reachability chain. Assume the frog starts at 1 (xprofile small/onfly).
        // prevPos = position before the last jump, cur = position after it.
        // A new link must be > prevPos (so it was out of reach before) and
        // <= cur (so it is in reach now). `collide` always takes the upper end,
        // putting a fly exactly on the frog's square; `chain` randomises inside
        // the window.
        vector<ll> links;
        ll prevPos = 0, cur = 1;
        while ((int)links.size() < n) {
            ll lo = prevPos + 1, hi = cur;
            if (lo > C) break;
            hi = min(hi, C);
            if (lo > hi) break;
            ll p = (profile == "collide") ? hi : rnd.next(lo, hi);
            links.push_back(p);
            prevPos = cur;
            cur += p;
            if (cur > 4 * C) break; // chain has saturated the coordinate cap
        }
        ensure(!links.empty());
        for (size_t i = 0; i < links.size(); ++i) {
            ensure(1 <= links[i] && links[i] <= C);
            pos[i + 1] = links[i];
        }
        // Pad the rest above the chain's reach when there is room, otherwise
        // at the cap. Padding never lowers a link below its window.
        ll padLow = min<ll>(C, cur + 1);
        for (int i = (int)links.size() + 1; i <= n; ++i) {
            pos[i] = (profile == "collide" && rnd.next(100) < 40)
                         ? links[rnd.next((int)links.size())]  // duplicate a landing spot
                         : rnd.next(padLow, C);
        }
    } else if (profile == "dup") {
        int k = (int)min<ll>(rnd.next(1, 5), C);
        set<ll> chosen;
        while ((int)chosen.size() < k) chosen.insert(genValue(1, C, vtype));
        dupValues.assign(chosen.begin(), chosen.end());
        for (int i = 1; i <= n; ++i) pos[i] = dupValues[rnd.next((int)dupValues.size())];
    } else if (profile == "maxvalue") {
        ll lo = max<ll>(1, C - 10);
        for (int i = 1; i <= n; ++i) pos[i] = rnd.next(lo, C);
    } else if (profile == "minvalue") {
        ll hi = min<ll>(3, C);
        for (int i = 1; i <= n; ++i) pos[i] = rnd.next(1LL, hi);
    } else if (profile == "cluster") {
        int k = rnd.next(3, 6);
        for (int i = 0; i < k; ++i) centers.push_back(genValue(1, C, "logarit"));
        clusterWidth = max<ll>(1, C / 1000);
        for (int i = 1; i <= n; ++i) {
            ll c = centers[rnd.next(k)];
            ll lo = max<ll>(1, c - clusterWidth), hi = min<ll>(C, c + clusterWidth);
            pos[i] = rnd.next(lo, hi);
        }
    } else {
        quitf(_fail, "unknown profile: %s", profile.c_str());
    }

    for (int i = 1; i <= n; ++i) ensure(1 <= pos[i] && pos[i] <= C);

    // -----------------------------------------------------------------------
    // queries
    // -----------------------------------------------------------------------
    // A type-1 target keeps the test's shape instead of injecting a foreign
    // value, so a chain/dup/cluster test stays adversarial after updates.
    vector<ll> live = pos;
    auto drawTarget = [&]() -> ll {
        if (profile == "dup") return dupValues[rnd.next((int)dupValues.size())];
        if (profile == "maxvalue") return rnd.next(max<ll>(1, C - 10), C);
        if (profile == "minvalue") return rnd.next(1LL, min<ll>(3, C));
        if (profile == "cluster") {
            ll c = centers[rnd.next((int)centers.size())];
            return rnd.next(max<ll>(1, c - clusterWidth), min<ll>(C, c + clusterWidth));
        }
        if (profile == "chain" || profile == "collide") {
            if (rnd.next(100) < 70) return live[rnd.next(1, n)];
            return genValue(1, C, "logarit");
        }
        return genValue(1, C, vtype);
    };

    auto drawX = [&]() -> ll {
        if (xprofile == "small") return rnd.next(1LL, min<ll>(10, XMAX));
        if (xprofile == "large") return rnd.next(XMAX - 10, XMAX);
        if (xprofile == "onfly") return live[rnd.next(1, n)];
        if (xprofile == "justabove") return min<ll>(XMAX, live[rnd.next(1, n)] + 1);
        if (xprofile == "random") return genValue(1, XMAX, vtype);
        quitf(_fail, "unknown xprofile: %s", xprofile.c_str());
    };

    // query kinds first, so `updates-then-query` can place its single type-2
    vector<int> kind(q);
    if (qmix == "balanced") {
        for (int i = 0; i < q; ++i) kind[i] = rnd.next(1, 2);
    } else if (qmix == "q2heavy") {
        for (int i = 0; i < q; ++i) kind[i] = (rnd.next(100) < 90) ? 2 : 1;
    } else if (qmix == "q1heavy") {
        for (int i = 0; i < q; ++i) kind[i] = (rnd.next(100) < 90) ? 1 : 2;
    } else if (qmix == "q2only") {
        for (int i = 0; i < q; ++i) kind[i] = 2;
    } else if (qmix == "q1only") {
        for (int i = 0; i < q; ++i) kind[i] = 1; // legal: the file produces no output
    } else if (qmix == "updates-then-query") {
        for (int i = 0; i < q; ++i) kind[i] = 1;
        kind[q - 1] = 2;
    } else {
        quitf(_fail, "unknown qmix: %s", qmix.c_str());
    }

    vector<array<ll, 3> > queries(q); // {type, a, b}
    for (int t = 0; t < q; ++t) {
        if (kind[t] == 1) {
            int u = rnd.next(1, n);
            ll y = drawTarget();
            ensure(1 <= u && u <= n);
            ensure(1 <= y && y <= C);
            queries[t] = {1, (ll)u, y};
            live[u] = y;
        } else {
            ll x = drawX();
            ensure(1 <= x && x <= XMAX);
            queries[t] = {2, x, 0};
        }
    }

    // -----------------------------------------------------------------------
    // print, in the statement's exact order
    // -----------------------------------------------------------------------
    ensure(1 <= n && n <= cap);
    ensure(1 <= q && q <= cap);
    ensure((int)queries.size() == q);

    cout << n << ' ' << q << '\n';
    for (int i = 1; i <= n; ++i) {
        if (i > 1) cout << ' ';
        cout << pos[i];
    }
    cout << '\n';
    for (int t = 0; t < q; ++t) {
        if (queries[t][0] == 1) cout << "1 " << queries[t][1] << ' ' << queries[t][2] << '\n';
        else cout << "2 " << queries[t][1] << '\n';
    }

    return 0;
}
