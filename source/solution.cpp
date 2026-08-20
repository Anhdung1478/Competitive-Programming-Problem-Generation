#include<bits/stdc++.h>
using namespace std;

#define fi first
#define se second
#define sz(x) int((x).size())
typedef pair<int, int> ii;
typedef long long ll;

const int MAXN = 1000006;

ll dp[MAXN][6];
int val[MAXN], nArr;

void process(void) {
    cin >> nArr;
    for (int i = 1; i <= nArr; ++i) {
        cin >> val[i];
        for (int j = 0; j < 6; ++j)
            dp[i][j] = -1e18;
    }

    for (int j = 0; j < 6; ++j)
        dp[0][j] = -1e18;

    dp[0][0] = 0;
    for (int i = 1; i <= nArr; ++i) {
        for (int j = 0; j < 6; ++j) {
            int sign = (j & 1) ? 1 : -1;
            dp[i][j] = max(dp[i][j], dp[i - 1][j] + sign * val[i]);
            if(j + 1 < 6)
                dp[i][j + 1] = max(dp[i][j + 1], dp[i][j]);
        }
    }

    cout << dp[nArr][5] << '\n';
}

int main(void) {
    ios_base::sync_with_stdio(0), cin.tie(0), cout.tie(0);

    #define TASK "seq"
    if(fopen(TASK".inp", "r")) {
        freopen(TASK".inp", "r", stdin);
        freopen(TASK".out", "w", stdout);
    }

    process();
    return 0;
}
