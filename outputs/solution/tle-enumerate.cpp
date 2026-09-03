// TLE (full scope, semantically correct) - enumerate every number in [L, R] with
// decimal-string increment and run the standard O(k^2) counting LIS on its digits.
// Passes when R - L + 1 is small (Subtask 1 sized ranges), hopeless otherwise.
#include <bits/stdc++.h>
using namespace std;

typedef pair<int, int> pii;
const int MOD = 998244353;

pii power(const string &s) {
    int k = (int)s.size();
    vector<int> len(k, 1);
    vector<int> cnt(k, 1);
    for (int i = 0; i < k; ++i)
        for (int j = 0; j < i; ++j)
            if (s[j] <= s[i]) {
                if (len[j] + 1 > len[i]) len[i] = len[j] + 1, cnt[i] = cnt[j];
                else if (len[j] + 1 == len[i] && (cnt[i] += cnt[j]) >= MOD) cnt[i] -= MOD;
            }
    pii res(0, 0);
    for (int i = 0; i < k; ++i) {
        if (len[i] > res.first) res = pii(len[i], 0);
        if (len[i] == res.first && (res.second += cnt[i]) >= MOD) res.second -= MOD;
    }
    return res;
}

void increment(string &s) {
    int i = (int)s.size() - 1;
    while (i >= 0 && s[i] == '9') s[i--] = '0';
    if (i < 0) s.insert(s.begin(), '1');
    else ++s[i];
}

int main() {
    ios_base::sync_with_stdio(false), cin.tie(nullptr);

    #define TASK "lis"
    if (fopen(TASK".inp", "r")) {
        freopen(TASK".inp", "r", stdin);
        freopen(TASK".out", "w", stdout);
    }

    int t;
    cin >> t;
    while (t--) {
        string L, R;
        cin >> L >> R;
        pii res(0, 0);
        for (string x = L;; increment(x)) {
            pii cur = power(x);
            if (cur.first > res.first) res = pii(cur.first, 0);
            if (cur.first == res.first && (res.second += cur.second) >= MOD) res.second -= MOD;
            if (x == R) break;
        }
        cout << res.first << ' ' << res.second << '\n';
    }
    return 0;
}
