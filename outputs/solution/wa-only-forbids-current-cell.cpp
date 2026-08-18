#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
const int64 MOD = 998'244'353;

struct Matrix2 {
    int64 a[2][2]{};
};

Matrix2 multiply(const Matrix2& x, const Matrix2& y) {
    Matrix2 z;
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int p = 0; p < 2; ++p)
                z.a[i][j] = (z.a[i][j] + x.a[i][p] * y.a[p][j]) % MOD;
    return z;
}

Matrix2 power(Matrix2 base, int64 exponent) {
    Matrix2 result;
    result.a[0][0] = result.a[1][1] = 1;
    while (exponent > 0) {
        if (exponent & 1) result = multiply(result, base);
        base = multiply(base, base);
        exponent >>= 1;
    }
    return result;
}

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
    const int64 cellsMod = (r % MOD) * (c % MOD) % MOD;
    Matrix2 transition;
    transition.a[0][0] = (cellsMod - 2 + MOD) % MOD;
    transition.a[0][1] = (cellsMod - 1 + MOD) % MOD;
    transition.a[1][0] = 1;
    const Matrix2 raised = power(transition, k);
    const bool sameEndpoint = (r == 1 && c == 1);
    const int64 different = sameEndpoint ? 0 : 1;
    const int64 equal = sameEndpoint ? 1 : 0;
    cout << (raised.a[1][0] * different + raised.a[1][1] * equal) % MOD << '\n';
    return 0;
}
