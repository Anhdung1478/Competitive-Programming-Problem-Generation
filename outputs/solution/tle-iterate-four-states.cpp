#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
const int64 MOD = 998'244'353;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    #define TASK "antirook"
    if (fopen(TASK ".inp", "r")) {
        freopen(TASK ".inp", "r", stdin);
        freopen(TASK ".out", "w", stdout);
    }

    int64 r, c, k;
    cin >> r >> c >> k;
    int64 rowWays[2][2] = {
        {(r - 2 + MOD) % MOD, (r == 1 ? 0 : 1)},
        {(r - 1) % MOD, 0}
    };
    int64 columnWays[2][2] = {
        {(c - 2 + MOD) % MOD, (c == 1 ? 0 : 1)},
        {(c - 1) % MOD, 0}
    };

    int64 current[2][2]{};
    current[r == 1][c == 1] = 1;
    while (k-- > 0) {
        int64 next[2][2]{};
        for (int x = 0; x < 2; ++x)
            for (int y = 0; y < 2; ++y)
                for (int nx = 0; nx < 2; ++nx)
                    for (int ny = 0; ny < 2; ++ny)
                        next[nx][ny] = (next[nx][ny] + current[x][y] *
                            rowWays[x][nx] % MOD * columnWays[y][ny]) % MOD;
        memcpy(current, next, sizeof(current));
    }
    cout << current[1][1] << '\n';
    return 0;
}
