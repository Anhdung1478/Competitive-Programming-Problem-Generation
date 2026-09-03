#include<bits/stdc++.h>
using namespace std;

#define inf 0x3f3f3f3f
#define sz(x) int((x).size())
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

const bool isMultiTest = 1;
const int MAXN = 10004;
const int MOD = 998244353;

string strL, strR, L, R;
ii dp[MAXN][10][2][2][2];

void update(ii &a, const ii &b) {
    if(a.fi < b.fi) a = ii(b.fi, 0);
    if(a.fi == b.fi && (a.se += b.se) >= MOD) a.se -= MOD;
}

ii solve(int id, int last, bool nonz, bool biggerL, bool smallerR) {
    if(id < 0) return ii(0, 1);

    ii &res(dp[id][last][nonz][biggerL][smallerR]);
    if(res.fi != -1) return res;

    res = ii(0, 0);
    int low_lim = (smallerR) ? 9 : (strR[id] - '0');
    int up_lim = (biggerL) ? 0 : (strL[id] - '0');
    for (int dig = up_lim; dig <= low_lim; ++dig) {
        bool nBiggerL  = biggerL  | (dig > up_lim);
        bool nSmallerR = smallerR | (dig < low_lim);
        bool nNonz = nonz | (dig > 0);
        for (int choose = 0; choose < 2; ++choose) if(!choose || nNonz && last <= dig) {
            int nLast = (choose) ? dig : last;
            ii ans = solve(id - 1, nLast, nNonz, nBiggerL, nSmallerR);

            ans.fi += (choose);
            update(res, ans);
        }
    }

    return res;
}

ii calc(const string &sL, const string &sR) {
    strL = sL;
    strR = sR;
    reverse(strL.begin(), strL.end());
    reverse(strR.begin(), strR.end());
    while(sz(strL) < sz(strR)) strL.push_back('0');

    for (int i = 0; i < sz(strR); ++i) for (int dig = 0; dig <= 9; ++dig) for (int t = 0; t < 8; ++t) dp[i][dig][t >> 2][t >> 1 & 1][t & 1] = ii(-1, -1);
    return solve(sz(strR) - 1, 0, 0, 0, 0);
}

void process(void) {
    cin >> L >> R;
    ii ans = calc(L, R);
    cout << ans.fi << ' ' << ans.se << '\n';
}

int main(void) {
    ios_base::sync_with_stdio(0), cin.tie(0), cout.tie(0);

    #define TASK "lis"
    if(fopen(TASK".inp", "r")) {
        freopen(TASK".inp", "r", stdin);
        freopen(TASK".out", "w", stdout);
    }

    int numTest = 1;
    if(isMultiTest) cin >> numTest;

    while(numTest--) process();
    return 0;
}
