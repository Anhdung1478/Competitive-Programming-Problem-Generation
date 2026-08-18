#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
const int64 MOD = 998'244'353;

struct Matrix2 {
    int64 a[2][2]{};
};

Matrix2 operator*(const Matrix2& left, const Matrix2& right) {
    Matrix2 result;
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int p = 0; p < 2; ++p)
                result.a[i][j] = (result.a[i][j] + left.a[i][p] * right.a[p][j]) % MOD;
    return result;
}

Matrix2 power(Matrix2 base, int64 exponent) {
    Matrix2 result;
    result.a[0][0] = result.a[1][1] = 1;
    while (exponent > 0) {
        if (exponent & 1) result = result * base;
        base = base * base;
        exponent >>= 1;
    }
    return result;
}

int64 coordinateWays(int64 size, int64 steps) {
    const int64 sizeMod = size % MOD;
    Matrix2 transition;
    transition.a[0][0] = (sizeMod - 2 + MOD) % MOD;
    transition.a[0][1] = (sizeMod - 1 + MOD) % MOD;
    transition.a[1][0] = 1;

    const Matrix2 raised = power(transition, steps);
    const int64 initialDifferent = (size == 1 ? 0 : 1);
    const int64 initialEqual = (size == 1 ? 1 : 0);
    return (raised.a[1][0] * initialDifferent +
            raised.a[1][1] * initialEqual) % MOD;
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
    cout << coordinateWays(r, k) * coordinateWays(c, k) % MOD << '\n';
    return 0;
}
