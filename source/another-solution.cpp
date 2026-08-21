#include <bits/stdc++.h>

#define el '\n'
#define fi first
#define sec second
#define pb push_back
#define ll long long
#define pii pair<int,int>
#define sz(v) (int)(v).size()
#define all(v) (v).begin(),(v).end()
#define FOR(i, a, b) for(int i = (a), _b = (b); i <= _b; i++)
#define REP(i, a, b) for(int i = (a), _b = (b); i >= _b; i--)

using namespace std;

const long long LLNF = 0x3f3f3f3f3f3f3f3f;
const int INF = 0x3f3f3f3f;
const int MOD = 1e9 + 7;
const int MAX_N = 2e5;
const int LOG = 18;

struct Mint{
    int val;

    Mint(int _val = 0){
        val = _val;
    }

    Mint operator + (const Mint &other) const{
        ll res = val + other.val;
        while(res >= MOD) res -= MOD;
        return {res};
    }

    Mint operator - (const Mint &other) const{
        ll res = val - other.val;
        if(res < 0) res += MOD;
        return {res};
    }

    Mint operator * (const Mint &other) const{
        return {(1LL * val * other.val) % MOD};
    }
};

struct Block_GCD{
    int l, r, val;
};

vector<Block_GCD> L[MAX_N + 5], R[MAX_N + 5];
Mint a[MAX_N + 5];
int n;

struct Sparse_Table{
    pii table_max[MAX_N + 5][LOG + 5];
    int table_gcd[MAX_N + 5][LOG + 5];

    void build(int n){
        FOR(i, 1, n){
            table_max[i][0] = {a[i].val, i};
            table_gcd[i][0] = a[i].val;
        }

        FOR(j, 1, LOG) FOR(i, 1, n - (1 << j) + 1){
            table_max[i][j] = max(table_max[i][j - 1], table_max[i + (1 << (j - 1))][j - 1]);
            table_gcd[i][j] = __gcd(table_gcd[i][j - 1], table_gcd[i + (1 << (j - 1))][j - 1]);
        }
    }

    pii query_max(int l, int r){
        int k = __lg(r - l + 1);
        return max(table_max[l][k], table_max[r - (1 << k) + 1][k]);
    }

    int query_gcd(int l, int r){
        int k = __lg(r - l + 1);
        return __gcd(table_gcd[l][k], table_gcd[r - (1 << k) + 1][k]);
    }
}rmq;

void Input(){
    cin >> n;
    FOR(i, 1, n) cin >> a[i].val;
}

void Prepare(){
    rmq.build(n);

    FOR(i, 1, n){
        int j = i;
        while(j <= n){
            int lo = j, hi = n, pos;

            while(lo <= hi){
                int mid = (lo + hi) / 2;

                if(rmq.query_gcd(i, j) == rmq.query_gcd(i, mid)){
                    pos = mid;
                    lo = mid + 1;
                }
                else hi = mid - 1;
            }

            R[i].pb({j, pos, rmq.query_gcd(i, pos)});
            j = pos + 1;
        }
    }

    REP(i, n, 1){
        int j = i;
        while(j >= 1){
            int lo = 1, hi = j, pos;

            while(lo <= hi){
                int mid = (lo + hi) / 2;

                if(rmq.query_gcd(mid, i) == rmq.query_gcd(j, i)){
                    pos = mid;
                    hi = mid - 1;
                }
                else lo = mid + 1;
            }

            L[i].pb({pos, j, rmq.query_gcd(pos, i)});
            j = pos - 1;
        }
    }
}

Mint dnc(int l, int r){
    if(l > r) return 0;
    if(l == r) return a[l] * a[l];

    int mid = rmq.query_max(l, r).sec;
    Mint res = dnc(l, mid - 1) + dnc(mid + 1, r);

    for(Block_GCD lb : L[mid]){
        int real_L_left = max(l, lb.l);
        int real_R_left = min(mid, lb.r);
        if(real_L_left > real_R_left) continue;

        int len_left = real_R_left - real_L_left + 1;

        for(Block_GCD rb : R[mid]){
            int real_L_right = max(mid, rb.l);
            int real_R_right = min(r, rb.r);
            if(real_L_right > real_R_right) continue;

            int len_right = real_R_right - real_L_right + 1;

            int g = __gcd(lb.val, rb.val);
            Mint ways = Mint(len_left) * Mint(len_right);

            res = res + ways * Mint(g) * a[mid];
        }
    }

    return res;
}

void Solve(){
    cout << dnc(1, n).val;
}

int main(){
    freopen("gcd-max.inp", "r", stdin);
    freopen("gcd-max.out", "w", stdout);
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    Input();
    Prepare();
    Solve();

    return 0;
}
