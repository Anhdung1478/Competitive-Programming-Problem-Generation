#include<bits/stdc++.h>
using namespace std;

#define fi first
#define se second
#define sz(x) int((x).size())
typedef long long ll;
typedef pair<int, int> ii;

const bool isMultiTest = 0;
const int MAXN = 5e5+5;
const int MOD = 998244353;

struct Matrix {
    ll a[3][3];
    int numRow, numCol;

    Matrix(int _r = 0, int _c = 0) {
        numRow = _r, numCol = _c;
        for (int i = 0; i < numRow; ++i) for (int j = 0; j < numCol; ++j) a[i][j] = 0;
    }

    Matrix operator * (const Matrix &other) {
        Matrix res(numRow, other.numCol);
        for (int i = 0; i < numRow; ++i) {
            for (int j = 0; j < other.numCol; ++j) {
                res.a[i][j] = 0;
                for (int k = 0; k < numCol; ++k) res.a[i][j] += a[i][k] * other.a[k][j] % MOD;
                res.a[i][j] %= MOD;
            }
        }

        return res;
    }

} A0, A1, A2, A3, X;

Matrix powerMatrix(Matrix A, ll expo) {
    assert(A.numRow == A.numCol);

    Matrix res(A.numRow, A.numCol);
    for (int i = 0; i < A.numRow; ++i) res.a[i][i] = 1;

    while(expo > 0) {
        if(expo & 1) res = res * A;

        A = A * A;
        expo >>= 1;
    }

    return res;
}

ll a[MAXN], mArr;
int b[MAXN], nArr, K;

void reset(void) {

}

void input(void) {
    cin >> nArr >> mArr >> K;
    for (int i = 0; i < K; ++i) cin >> a[i] >> b[i];
}

void prepare(void) {
    // 0: curr
    // 1: desti
    // 2: differ

    // b[i - 1] != b[i]
    A0 = Matrix(3, 3);
    A0.a[0][1] = A0.a[1][0] = 1;
    A0.a[0][2] = A0.a[1][2] = max(0, nArr - 2);
    A0.a[2][2] = max(0, nArr - 3);
    A0.a[2][0] = A0.a[2][1] = 1;
    
    // b[i - 1] == b[i]
    A1 = Matrix(3, 3);
    A1.a[0][2] = A1.a[1][2] = nArr - 1;
    A1.a[2][2] = max(0, nArr - 2);
    A1.a[2][1] = 1;

    // i == 0
    A2 = Matrix(3, 3);
    A2.a[0][1] = A2.a[2][1] = 1;
    A2.a[0][2] = nArr - 1;
    A2.a[1][2] = nArr - 1;
    A2.a[2][2] = max(0, nArr - 2);

    // i == K
    A3 = Matrix(3, 3);
    A3.a[0][1] = nArr - 1;
    A3.a[1][0] = 1;
    A3.a[1][1] = max(0, nArr - 2);
}

void solve(void) {
    X = Matrix(1, 3);
    X.a[0][0] = 1;

    ll last = 0;
    for (int i = 0; i < K; ++i) {
        Matrix A;
        
        if(i == 0) {
            A = powerMatrix(A2, a[i] - last);
        } else
            if(b[i - 1] != b[i]) {
                A = powerMatrix(A0, a[i] - last);
            } else {
                A = powerMatrix(A1, a[i] - last);
            }

        last = a[i];
        X = X * A;
        X.a[0][0] = X.a[0][1];
        X.a[0][1] = X.a[0][2] = 0;
    }

    if(K == 0) {
        Matrix A = powerMatrix(A2, mArr);
        X = X * A;
    }

    if(K > 0 && a[K - 1] < mArr) {
        Matrix A = powerMatrix(A3, mArr - last);
        X = X * A;
    }

    ll res = X.a[0][0] + X.a[0][1] + X.a[0][2];
    cout << res % MOD << '\n';
}

void process(void) {
    reset();
    input();
    prepare();
    solve();
}

int main(void) {
    ios_base::sync_with_stdio(0), cin.tie(0), cout.tie(0);

    #define TASK "main"
    if(fopen(TASK".inp", "r")) {
        freopen(TASK".inp", "r", stdin);
        freopen(TASK".out", "w", stdout);
    }

    int numTest = 1;
    if(isMultiTest) cin >> numTest;

    while(numTest--) process();
    return 0;
}
