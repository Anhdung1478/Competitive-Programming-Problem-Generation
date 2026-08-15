#include<bits/stdc++.h>
using namespace std;

#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#define fi first
#define se second
#define sz(x) int((x).size())
typedef pair<int, int> ii;
typedef long long ll;

const bool isMultiTest = 1;
const int MAXN = 1e5+5;

int a[MAXN], nArr, mArr;

void input(void) {
    cin >> nArr >> mArr;
    for (int i = 1; i <= nArr; ++i) cin >> a[i];
}

void prepare(void) {
    
}

void solve(void) {

}

void process(void) {
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

    precompute();

    int numTest = 1;
    if(isMultiTest) cin >> numTest;

    while(numTest--) process();
    return 0;
}
