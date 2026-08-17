#include<bits/stdc++.h>
using namespace std;

#define fi first
#define se second
#define sz(x) int((x).size())
typedef pair<int, int> ii;
typedef long long ll;

const int MAXN = 290;
const int MOD = 998244353;

int dp[2][MAXN][MAXN], sumRow[MAXN], sumCol[MAXN];
int numRow, numCol, numStep;

void inline add(int &a, const int &b) {
    if((a += b) >= MOD)
        a -= MOD;
}

void inline sub(int &a, const int &b) {
    if((a -= b) < 0)
        a += MOD;
}

void process(void) {
    cin >> numRow >> numCol >> numStep;

    dp[0][1][1] = 1;
    for (int t = 0; t < numStep; ++t) {
        int pre(t & 1), cur(!pre);

        ll sumPre(0);
        for (int x = 1; x <= numRow; ++x)
            sumRow[x] = 0;

        for (int y = 1; y <= numCol; ++y)
            sumCol[y] = 0;

        for (int x = 1; x <= numRow; ++x) {
            for (int y = 1; y <= numCol; ++y) {
                sumPre += dp[pre][x][y];
                add(sumRow[x], dp[pre][x][y]);
                add(sumCol[y], dp[pre][x][y]);
            }
        }

        sumPre %= MOD;
        for (int x = 1; x <= numRow; ++x) {
            for (int y = 1; y <= numCol; ++y) {
                /*dp[cur][x][y] = (sumPre + dp[pre][x][y]) % MOD;
                for (int k = 1; k <= numRow; ++k)
                    sub(dp[cur][x][y], dp[pre][k][y]);

                for (int k = 1; k <= numCol; ++k)
                    sub(dp[cur][x][y], dp[pre][x][k]);*/

                ll sum = (sumPre + dp[pre][x][y] - sumRow[x] - sumCol[y] + 2 * MOD) % MOD;
                //assert(sum == dp[cur][x][y]);
                dp[cur][x][y] = sum;
            }
        }

        for (int x = 1; x <= numRow; ++x) {
            for (int y = 1; y <= numCol; ++y) {
                dp[pre][x][y] = 0;
            }
        }
    }

    cout << dp[numStep & 1][numRow][numCol] << '\n';
}

int main(void) {
    ios_base::sync_with_stdio(0), cin.tie(0), cout.tie(0);

    #define TASK "antirook"
    if(fopen(TASK".inp", "r")) {
        freopen(TASK".inp", "r", stdin);
        freopen(TASK".out", "w", stdout);
    }

    process();
    return 0;
}
