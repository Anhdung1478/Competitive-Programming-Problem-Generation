#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

constexpr int64 MOD = 998244353;

int64 modularMultiply(int64 lhs, int64 rhs) {
    return static_cast<int64>(static_cast<__int128>(lhs) * rhs % MOD);
}

struct Matrix {
    int64 value[2][2]{};
};

Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
    Matrix result;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                result.value[i][j] =
                    (result.value[i][j] +
                     modularMultiply(lhs.value[i][k], rhs.value[k][j])) % MOD;
            }
        }
    }
    return result;
}

Matrix matrixPower(Matrix base, int64 exponent) {
    Matrix result;
    result.value[0][0] = result.value[1][1] = 1;

    while (exponent > 0) {
        if (exponent & 1LL) {
            result = result * base;
        }
        base = base * base;
        exponent >>= 1LL;
    }
    return result;
}

int64 modularPower(int64 base, int64 exponent) {
    int64 result = 1;
    base %= MOD;
    while (exponent > 0) {
        if (exponent & 1LL) {
            result = modularMultiply(result, base);
        }
        base = modularMultiply(base, base);
        exponent >>= 1LL;
    }
    return result;
}

// Count valid color sequences across a gap with both endpoint colors fixed.
int64 waysBetween(int64 colorCount, int64 distance, bool sameColor) {
    // equal(t): ways to reach the required right endpoint after t moves.
    // total(t): all valid continuations after t moves, equal to (N - 1)^t.
    // Therefore equal(t + 1) = total(t) - equal(t).
    Matrix transition;
    transition.value[0][0] = MOD - 1;
    transition.value[0][1] = 1;
    transition.value[1][1] = (colorCount - 1) % MOD;

    const Matrix powered = matrixPower(transition, distance);
    const int64 initialEqual = sameColor ? 1 : 0;
    return (powered.value[0][0] * initialEqual + powered.value[0][1]) % MOD;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int64 colorCount, dayCount;
    int fixedCount;
    cin >> colorCount >> dayCount >> fixedCount;

    vector<int64> day(fixedCount), color(fixedCount);
    for (int i = 0; i < fixedCount; ++i) {
        cin >> day[i] >> color[i];
    }

    if (fixedCount == 0) {
        const int64 answer =
            colorCount % MOD * modularPower(colorCount - 1, dayCount - 1) % MOD;
        cout << answer << '\n';
        return 0;
    }

    int64 answer = modularPower(colorCount - 1, day.front() - 1);
    for (int i = 1; i < fixedCount; ++i) {
        const int64 distance = day[i] - day[i - 1];
        answer = modularMultiply(
            answer, waysBetween(colorCount, distance, color[i] == color[i - 1]));
    }
    answer = modularMultiply(
        answer, modularPower(colorCount - 1, dayCount - day.back()));

    cout << answer << '\n';
    return 0;
}
