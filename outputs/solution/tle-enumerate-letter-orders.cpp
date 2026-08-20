#include <bits/stdc++.h>
using namespace std;

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

        vector<int> order(k);
        iota(order.begin(), order.end(), 0);
        do {
            int end = 0;
            bool valid = true;
            for (int c : order) {
                int left = nextStart[end + 1][c];
                if (left + len - 1 > n) {
                    valid = false;
                    break;
                }
                end = left + len - 1;
            }
            if (valid) return true;
        } while (next_permutation(order.begin(), order.end()));
        return false;
    };

    int low = 1, high = n / k, answer = 0;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (feasible(mid)) answer = mid, low = mid + 1;
        else high = mid - 1;
    }
    cout << answer << '\n';
    return 0;
}
