#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

// Subtasks (20% each), 50 tests
// ST1 N<=1000:                 1-10
// ST2 N<=1e5, 0<=x,y<=360:    11-20
// ST3 x=0, 0<=y:              21-30
// ST4 P=2:                    31-40
// ST5 full:                   41-50
// T = 5 always. Every TC: N >= 0.96*MAXN; some hit exact MAXN.
// Zigzag odd/even pockets; several adjacent layer products > 5e6 (ST2+).

const ll CMAX = 1000000000LL;
const ll WMAX = 1000000000LL;

struct Villager {
    ll x, y, w;
    int k;
};

struct Bounds {
    ll xlo, xhi, ylo, yhi;
};

void emit(vector<vector<Villager>> tests) {
    cout << (int)tests.size() << '\n';
    for (auto& tc : tests) {
        int P = 0;
        for (auto& v : tc) P = max(P, v.k);
        shuffle(tc.begin(), tc.end());
        cout << (int)tc.size() << ' ' << P << '\n';
        for (auto& v : tc)
            cout << v.x << ' ' << v.y << ' ' << v.k << ' ' << v.w << '\n';
    }
}

ll rand_coord(ll lo, ll hi) {
    if (lo > hi) swap(lo, hi);
    return rnd.next(lo, hi);
}

int n_floor(int nmax) { return max(1, (nmax * 96 + 99) / 100); }

int pick_n(int nmax, bool maximal) {
    if (maximal) return nmax;
    return rnd.next(n_floor(nmax), nmax);
}

int pick_T(bool /*maximal_file*/) {
    return rnd.wnext(4, 5, 2);
}

ll rand_w() {
    int typ = rnd.next(0, 4);
    if (typ == 0) return 0;
    if (typ == 1) return rnd.next(0LL, 1000LL);
    if (typ == 2) return rnd.next(0LL, WMAX / 1000);
    return rnd.next(0LL, WMAX);
}

// Odd layers: far upper-right; even layers: far lower-left (zigzag path).
pair<ll, ll> zigzag_point(int k, const Bounds& b, bool line) {
    bool odd = (k % 2 == 1);
    if (line) {
        ll span = max(1LL, (b.yhi - b.ylo) / 5);
        if (odd) return {b.xlo, rand_coord(max(b.ylo, b.yhi - span), b.yhi)};
        return {b.xlo, rand_coord(b.ylo, min(b.yhi, b.ylo + span))};
    }
    ll wx = max(1LL, (b.xhi - b.xlo) / 5);
    ll wy = max(1LL, (b.yhi - b.ylo) / 5);
    if (odd)
        return {rand_coord(max(b.xlo, b.xhi - wx), b.xhi),
                rand_coord(max(b.ylo, b.yhi - wy), b.yhi)};
    return {rand_coord(b.xlo, min(b.xhi, b.xlo + wx)),
            rand_coord(b.ylo, min(b.yhi, b.ylo + wy))};
}

void clamp_all(vector<Villager>& a, const Bounds& b) {
    for (auto& v : a) {
        v.x = max(b.xlo, min(v.x, b.xhi));
        v.y = max(b.ylo, min(v.y, b.yhi));
        v.w = max(0LL, min(v.w, WMAX));
    }
}

void ensure_levels(vector<Villager>& a, int P, const Bounds& b, bool line) {
    vector<char> have(P + 1, 0);
    for (auto& v : a)
        if (1 <= v.k && v.k <= P) have[v.k] = 1;
    for (int k = 1; k <= P; ++k) if (!have[k]) {
        auto [x, y] = zigzag_point(k, b, line);
        a.push_back({x, y, rand_w(), k});
    }
}

vector<int> layer_sizes(int n, int P) {
    vector<int> sz(P + 1, 0);
    int base = n / P, rem = n % P;
    for (int k = 1; k <= P; ++k) sz[k] = base + (k <= rem);
    return sz;
}

struct UniqueSampler {
    Bounds b;
    bool line;
    set<pair<ll, ll>> used;

    UniqueSampler(Bounds bb, bool ln) : b(bb), line(ln) {}

    pair<ll, ll> next(int k, bool prefer_zigzag) {
        auto try_put = [&](ll x, ll y) -> bool {
            if (used.count({x, y})) return false;
            used.insert({x, y});
            return true;
        };
        if (prefer_zigzag) {
            for (int t = 0; t < 50; ++t) {
                auto p = zigzag_point(k, b, line);
                if (try_put(p.first, p.second)) return p;
            }
        }
        for (int t = 0; t < 100; ++t) {
            ll x = line ? b.xlo : rand_coord(b.xlo, b.xhi);
            ll y = rand_coord(b.ylo, b.yhi);
            if (try_put(x, y)) return {x, y};
        }
        // Extremely rare on ST2 (grid has 130321 cells, N<=1e5)
        auto p = zigzag_point(k, b, line);
        used.insert(p);
        return p;
    }
};

// P so that (n/P)^2 is around / above targetProd, capped by maxP.
int choose_P_for_product(int n, ll targetProd, int maxP) {
    double s = sqrt((double)max(1LL, targetProd));
    int pMax = max(2, (int)floor(n / s));
    pMax = min({pMax, maxP, n, 101});
    if (pMax < 2) return 2;
    int mode = rnd.next(0, 2);
    if (mode == 0) return rnd.next(2, min(pMax, 10));          // very heavy products
    if (mode == 1) return rnd.next(min(pMax, 15), min(pMax, 50));
    return rnd.next(min(pMax, 40), pMax);                      // up to ~100 layers
}

vector<Villager> gen_zigzag(int n, int P, const Bounds& b, bool line, bool unique_coords) {
    P = max(2, min(P, n));
    auto sz = layer_sizes(n, P);
    vector<Villager> a;
    a.reserve(n + P);
    UniqueSampler samp(b, line);

    for (int k = 1; k <= P; ++k) {
        for (int i = 0; i < sz[k]; ++i) {
            pair<ll, ll> p;
            if (unique_coords) {
                // Most points in zigzag pocket; some spread in full box for diversity
                bool zig = rnd.next(0, 9) != 0;
                p = samp.next(k, zig);
            } else {
                if (rnd.next(0, 9) == 0) {
                    ll x = line ? b.xlo : rand_coord(b.xlo, b.xhi);
                    ll y = rand_coord(b.ylo, b.yhi);
                    p = {x, y};
                } else {
                    p = zigzag_point(k, b, line);
                }
            }
            a.push_back({p.first, p.second, rand_w(), k});
        }
    }

    // A few expensive decoys on the wrong side
    int decoy = min(n / 100, 50);
    for (int i = 0; i < decoy; ++i) {
        int k = rnd.next(1, P);
        auto p = zigzag_point(k + 1, b, line); // opposite parity pocket
        if (unique_coords) p = samp.next(k, false);
        a.push_back({p.first, p.second, rnd.next(WMAX / 2, WMAX), k});
    }

    clamp_all(a, b);
    ensure_levels(a, P, b, line);

    // Resize to exactly n, keeping >=1 per level
    if ((int)a.size() != n) {
        if ((int)a.size() < n) {
            UniqueSampler pad(b, line);
            if (unique_coords) pad.used = samp.used;
            while ((int)a.size() < n) {
                int k = rnd.next(1, P);
                auto p = unique_coords ? pad.next(k, true) : zigzag_point(k, b, line);
                a.push_back({p.first, p.second, rand_w(), k});
            }
        } else {
            vector<vector<Villager>> by(P + 1);
            for (auto& v : a) by[v.k].push_back(v);
            vector<Villager> kept;
            kept.reserve(n);
            for (int k = 1; k <= P; ++k) {
                if (by[k].empty()) {
                    auto p = zigzag_point(k, b, line);
                    by[k].push_back({p.first, p.second, rand_w(), k});
                }
                kept.push_back(by[k][0]);
            }
            for (int k = 1; k <= P && (int)kept.size() < n; ++k)
                for (int i = 1; i < (int)by[k].size() && (int)kept.size() < n; ++i)
                    kept.push_back(by[k][i]);
            while ((int)kept.size() < n) {
                int k = rnd.next(1, P);
                auto p = zigzag_point(k, b, line);
                kept.push_back({p.first, p.second, rand_w(), k});
            }
            a.swap(kept);
        }
    }
    clamp_all(a, b);
    return a;
}

vector<vector<Villager>> build_file(int testId) {
    int st = (testId - 1) / 10 + 1;
    int local = (testId - 1) % 10;

    Bounds full{-CMAX, CMAX, -CMAX, CMAX};
    Bounds grid{0, 360, 0, 360};
    Bounds line{0, 0, 0, CMAX};

    int nmax = (st == 1 ? 1000 : (st == 2 ? 100000 : 200000));
    bool maximal_file = (local >= 8); // tests *9 and *10 maximal
    int T = pick_T(maximal_file);

    vector<vector<Villager>> tests;
    tests.reserve(T);

    for (int t = 0; t < T; ++t) {
        bool maximal_tc = maximal_file || (t == 0 && local >= 6);
        int n = pick_n(nmax, maximal_tc);
        if (maximal_file && t == 0) n = nmax;

        int P = 2;
        bool use_line = false;
        bool unique = false;

        if (st == 1) {
            // Cannot reach product 5e6; still zigzag + low P, near-max N
            if (local % 3 == 0 || t == 0) P = 2;
            else P = rnd.next(2, min(n, 30));
            unique = false;
        } else if (st == 2) {
            unique = true;
            if (t == 0 || local % 5 == 0) P = 2;
            else if (local % 5 == 1) P = choose_P_for_product(n, 5000000LL, 80);
            else if (local % 5 == 2) P = rnd.next(40, min(101, n)); // ~100 consecutive pairs
            else P = choose_P_for_product(n, 5000000LL, 50);
        } else if (st == 3) {
            use_line = true;
            unique = true;
            if (t == 0 || local % 5 == 0) P = 2;
            else if (local % 5 == 1) P = choose_P_for_product(n, 5000000LL, 100);
            else if (local % 5 == 2) P = rnd.next(50, min(101, n));
            else P = choose_P_for_product(n, 8000000LL, 60);
        } else if (st == 4) {
            P = 2;
            use_line = ((local + t) % 3 == 0);
            unique = !use_line; // full plane: allow duplicates rarely; prefer spread
            // For full plane unique of 2e5 in huge space is fine with set
            unique = true;
        } else {
            if (t == 0 || local % 5 == 0) P = 2;
            else if (local % 5 == 1) P = choose_P_for_product(n, 5000000LL, 100);
            else if (local % 5 == 2) P = rnd.next(60, min(101, n));
            else if (local % 5 == 3) P = choose_P_for_product(n, 10000000LL, 40);
            else P = choose_P_for_product(n, 5000000LL, 80);
            use_line = ((local + t) % 7 == 0);
            unique = false;
        }

        Bounds b = st == 2 ? grid : (use_line || st == 3 ? line : full);
        auto tc = gen_zigzag(n, P, b, use_line || st == 3, unique);

        // Hard clamp size / ST
        if (st == 4) {
            for (auto& v : tc) v.k = (v.k <= 1 ? 1 : 2);
            P = 2;
            ensure_levels(tc, 2, b, use_line);
        }
        if ((int)tc.size() > nmax) {
            // should already be n, but safety
            vector<vector<Villager>> by(P + 1);
            for (auto& v : tc) by[min(max(v.k, 1), P)].push_back(v);
            tc.clear();
            for (int k = 1; k <= P; ++k) {
                if (by[k].empty()) {
                    auto p = zigzag_point(k, b, use_line || st == 3);
                    by[k].push_back({p.first, p.second, rand_w(), k});
                }
                tc.push_back(by[k][0]);
            }
            for (int k = 1; k <= P && (int)tc.size() < n; ++k)
                for (int i = 1; i < (int)by[k].size() && (int)tc.size() < n; ++i)
                    tc.push_back(by[k][i]);
        }
        while ((int)tc.size() < n) {
            int k = rnd.next(1, P);
            auto p = zigzag_point(k, b, use_line || st == 3);
            tc.push_back({p.first, p.second, rand_w(), k});
        }
        if ((int)tc.size() > n) tc.resize(n);
        // After resize, re-ensure levels (resize may have dropped tail only — levels ok if built carefully)
        ensure_levels(tc, P, b, use_line || st == 3);
        if ((int)tc.size() > nmax) tc.resize(nmax);
        // If ensure grew past n but still >= floor, OK; else pad was already done
        if ((int)tc.size() < n_floor(nmax)) {
            while ((int)tc.size() < n_floor(nmax)) {
                int k = rnd.next(1, P);
                auto p = zigzag_point(k, b, use_line || st == 3);
                tc.push_back({p.first, p.second, rand_w(), k});
            }
        }
        if ((int)tc.size() > nmax) tc.resize(nmax);

        clamp_all(tc, b);
        tests.push_back(std::move(tc));
    }

    return tests;
}

int main(int argc, char* argv[]) {
    registerGen(argc, argv, 1);
    int testId = opt<int>("test");
    emit(build_file(testId));
    return 0;
}
