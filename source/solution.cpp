#include<bits/stdc++.h>
using namespace std;

#pragma GCC target ("avx2")
#pragma GCC optimization ("O3")
#pragma GCC optimization ("unroll-loops")

#define fi first
#define se second
typedef long long ll;
typedef pair<int, int> ii;

template<class X, class Y>
	inline bool maximize(X &x, const Y &y) {return (x < y ? x = y, 1 : 0);}
template<class X, class Y>
	inline bool minimize(X &x, const Y &y) {return (x > y ? x = y, 1 : 0);}

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
int Random(int l, int r) {
    return uniform_int_distribution<int>(l, r)(rng);
}

const int MOD = 998244353;

struct Matrix {
    ll val[4][4];
    int nRow, nCol;

    Matrix(int _r = 0, int _c = 0) {
        nRow = _r, nCol = _c;
        for (int i = 0; i < nRow; ++i) for (int j = 0; j < nCol; ++j) val[i][j] = 0;
    }

    Matrix operator * (const Matrix &other) {
        assert(nCol == other.nRow);
        Matrix res(nRow, other.nCol);
        for (int i = 0; i < res.nRow; ++i) {
            for (int j = 0; j < res.nCol; ++j) {
                for (int k = 0; k < nCol; ++k) res.val[i][j] += val[i][k] * other.val[k][j] % MOD;
                res.val[i][j] %= MOD;
            }
        }

        return res;
    }

    Matrix powerMatrix(ll k) {
        assert(nRow == nCol);
        Matrix res(nRow, nCol), A = (*this);
        for (int i = 0; i < nRow; ++i) res.val[i][i] = 1;

        while(k > 0) {
            if(k & 1) res = res * A;
            A = A * A;
            k >>= 1;
        }

        return res;
    }

} A, X;

ll numRow, numCol, k;

#define id(x, y) (x) * 2 + (y)

void process() {
    cin >> numRow >> numCol >> k;

    // dp[i][j] = num way go to row (i == 1: row == numRow, i == 0: row != numRow)
                             // col (j == 1: col == numCol, j == 0: col != numCol)

    ll row_way[2][2];
    ll col_way[2][2];

    row_way[0][0] = max(numRow - 2, 0LL) % MOD;
    row_way[0][1] = min(numRow - 1, 1LL);
    row_way[1][0] = (numRow - 1) % MOD;
    row_way[1][1] = 0;

    col_way[0][0] = max(numCol - 2, 0LL) % MOD;
    col_way[0][1] = min(numCol - 1, 1LL);
    col_way[1][0] = (numCol - 1) % MOD;
    col_way[1][1] = 0;

    X = Matrix(1, 4);
    X.val[0][id(numRow == 1, numCol == 1)] = 1;

    A = Matrix(4, 4);
    for (int x = 0; x < 2; ++x) {
        for (int y = 0; y < 2; ++y) {
            for (int z = 0; z < 2; ++z) {
                for (int t = 0; t < 2; ++t) {
                    // (x, y) of the last step contributes to (z, t) of the next step = row_way[x][z] * col_way[y][t]
                    A.val[id(x, y)][id(z, t)] = row_way[x][z] * col_way[y][t] % MOD;
                }
            }
        }
    }

    A = A.powerMatrix(k);
    X = X * A;
    
    ll ans = X.val[0][id(1, 1)] % MOD;
    cout << ans << '\n';
}

int main() {
    ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);

    #define TASK "antirook"
    if(fopen(TASK".inp", "r")) {
        freopen(TASK".inp", "r", stdin);
        freopen(TASK".out", "w", stdout);
    }

    clock_t time = clock();
    process();

    cerr << "Time run: " << (clock() - time) / CLOCKS_PER_SEC << " ms.\n";
    return 0;
}
