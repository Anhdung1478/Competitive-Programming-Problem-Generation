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

    auto feasible = [&](int len) {
        int position = 0;
        for (int c = 0; c < k; ++c) {
            int bad = 0;
            for (int i = position; i < min(n, position + len); ++i) {
                bad += (s[i] != '?' && s[i] != char('a' + c));
            }
            bool found = false;
            for (int left = position; left + len <= n; ++left) {
                if (left > position) {
                    bad -= (s[left - 1] != '?' && s[left - 1] != char('a' + c));
                    bad += (s[left + len - 1] != '?' && s[left + len - 1] != char('a' + c));
                }
                if (bad == 0) {
                    position = left + len;
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
        return true;
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
