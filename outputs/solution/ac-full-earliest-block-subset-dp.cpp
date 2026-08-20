#include <bits/stdc++.h>
using namespace std;

const int INF = 1'000'000'000;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    #define TASK "courses"
    if (fopen(TASK ".inp", "r")) {
        freopen(TASK ".inp", "r", stdin);
        freopen(TASK ".out", "w", stdout);
    }

    int n, k;
    string s;
    cin >> n >> k >> s;
    s = " " + s;

    vector<vector<int>> incompatible(k, vector<int>(n + 1));
    for (int c = 0; c < k; ++c) {
        for (int i = 1; i <= n; ++i) {
            incompatible[c][i] = incompatible[c][i - 1]
                + (s[i] != '?' && s[i] != char('a' + c));
        }
    }

    vector<array<int, 17>> nextStart(n + 2);
    vector<int> earliest(1 << k);

    auto feasible = [&](int len) {
        for (int c = 0; c < k; ++c) {
            int nearest = n + 1;
            nextStart[n + 1][c] = n + 1;
            for (int left = n; left >= 1; --left) {
                if (left + len - 1 <= n
                    && incompatible[c][left + len - 1] == incompatible[c][left - 1]) {
                    nearest = left;
                }
                nextStart[left][c] = nearest;
            }
        }

        fill(earliest.begin(), earliest.end(), INF);
        earliest[0] = 0;
        int fullMask = (1 << k) - 1;
        for (int mask = 0; mask <= fullMask; ++mask) {
            if (earliest[mask] > n) continue;
            int remaining = fullMask ^ mask;
            while (remaining) {
                int bit = remaining & -remaining;
                int c = __builtin_ctz((unsigned)bit);
                int left = nextStart[earliest[mask] + 1][c];
                if (left + len - 1 <= n) {
                    earliest[mask | bit] = min(earliest[mask | bit], left + len - 1);
                }
                remaining ^= bit;
            }
        }
        return earliest[fullMask] <= n;
    };

    int low = 1, high = n / k, answer = 0;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (feasible(mid)) {
            answer = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    cout << answer << '\n';
    return 0;
}
