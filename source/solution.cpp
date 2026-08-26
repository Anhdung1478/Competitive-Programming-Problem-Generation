#include<bits/stdc++.h>
using namespace std;

#define fi first
#define se second
#define sz(x) int((x).size())
typedef pair<int, int> ii;
typedef long long ll;

const bool isMultiTest = 0;
const int MAXN = 2e5+5;

struct Query {
    int type, u, x, r_x;
} qr[MAXN];

vector<int> idx;
int pos[MAXN], r_pos[MAXN], nArr, numQuery;

void reset(void) {
}

void input(void) {
    cin >> nArr >> numQuery;
    for (int i = 1; i <= nArr; ++i) cin >> pos[i];

    for (int t = 0; t < numQuery; ++t) {
        cin >> qr[t].type >> qr[t].u;
        if(qr[t].type == 1) cin >> qr[t].x;
    }
}

typedef pair<ll, int> pli;
namespace Fenwick {
    ll fenSum[2 * MAXN];
    int fenCnt[2 * MAXN], nTree;

    void modify(int i, int val, int sign) {
        for (; i <= nTree; i += i & -i) {
            fenSum[i] += val * sign;
            fenCnt[i] += sign;
        }
    }

    pli get(int i) {
        ll res_sum = 0;
        int res_cnt = 0;
        for (; i > 0; i -= i & -i) {
            res_sum += fenSum[i];
            res_cnt += fenCnt[i];
        }

        return make_pair(res_sum, res_cnt);
    }

    void init(int _n) {
        nTree = _n;
        for (int i = 1; i <= nTree; ++i) fenSum[i] = fenCnt[i] = 0;
    }
}

void prepare(void) {
    for (int i = 1; i <= nArr; ++i) idx.push_back(pos[i]);
    for (int t = 0; t < numQuery; ++t) if(qr[t].type == 1) idx.push_back(qr[t].x);

    sort(idx.begin(), idx.end());
    idx.erase(unique(idx.begin(), idx.end()), idx.end());

    for (int i = 1; i <= nArr; ++i) r_pos[i] = upper_bound(idx.begin(), idx.end(), pos[i]) - idx.begin();
    for (int t = 0; t < numQuery; ++t) if(qr[t].type == 1) {
        qr[t].r_x = upper_bound(idx.begin(), idx.end(), qr[t].x) - idx.begin();
    }

    Fenwick::init(sz(idx));
}

pli get_result(int x) {
    ll res_pos = x;
    ll sum_last;
    int cnt_last;
    int res_cnt = 0;

    int r_pos = upper_bound(idx.begin(), idx.end(), res_pos) - idx.begin();
    tie(sum_last, cnt_last) = Fenwick::get(r_pos);

    res_pos += sum_last;
    res_cnt += cnt_last;
    while(1) {
        ll sum_now;
        int cnt_now;
        int r_pos_now = upper_bound(idx.begin(), idx.end(), res_pos) - idx.begin();

        tie(sum_now, cnt_now) = Fenwick::get(r_pos_now);
        if(cnt_last == cnt_now) break;

        res_pos += sum_now - sum_last;
        res_cnt += cnt_now - cnt_last;
        sum_last = sum_now;
        cnt_last = cnt_now;
    }

    return make_pair(res_pos, res_cnt);
}

void solve(void) {
    for (int i = 1; i <= nArr; ++i) Fenwick::modify(r_pos[i], pos[i], +1);

    for (int t = 0; t < numQuery; ++t) {
        int type(qr[t].type), u(qr[t].u), x(qr[t].x), r_x(qr[t].r_x);

        if(type == 1) {
            Fenwick::modify(r_pos[u], pos[u], -1);
            Fenwick::modify(r_x, x, +1);

            pos[u] = x;
            r_pos[u] = r_x;
            continue;
        }

        pli ans = get_result(u);
        cout << ans.fi << ' ' << ans.se << '\n';
    }
}

void process(void) {
    reset();
    input();
    prepare();
    solve();
}

int main(void) {
    ios_base::sync_with_stdio(0), cin.tie(0), cout.tie(0);
    
    #define TASK "jumping-frog"
    if(fopen(TASK".inp", "r")) {
        freopen(TASK".inp", "r", stdin);
        freopen(TASK".out", "w", stdout);
    }
    
    int numTest = 1;
    if(isMultiTest) cin >> numTest;
    
    while(numTest--) process();
    return 0;
}