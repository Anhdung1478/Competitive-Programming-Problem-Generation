#include<bits/stdc++.h>
using namespace std;

#define inf 0x3f3f3f3f
#define fi first
#define se second
#define sz(x) int((x).size())
typedef long long ll;
typedef pair<int, int> ii;

const int MAXN = 200005;
const bool isMultiTest = 0;

vector<int> idx;
int a[MAXN], na[MAXN], dp[MAXN][3], fen[3][2 * MAXN], nTree, nArr;

void modify(int id, int i, int v) {
    for (; i <= nTree; i += i & -i)
        fen[id][i] = max(fen[id][i], v);
}

int get(int id, int i) {
    int res(0);
    for (; i > 0; i -= i & -i)
        res = max(res, fen[id][i]);

    return res;
}

int magicFunc(void) {
    for (int i = 1; i <= nArr; ++i) {
        dp[i][0] = get(0, a[i] - 1) + 1;
        dp[i][1] = 1 + max(get(0, na[i] - 1), get(1, na[i] - 1));
        dp[i][2] = 1 + max(get(1, a[i] - 1), get(2, a[i] - 1));

        modify(0, a[i], dp[i][0]);
        modify(1, na[i], dp[i][1]);
        modify(2, a[i], dp[i][2]);
    }

    return max({get(0, nTree), get(1, nTree), get(2, nTree)});
}

void process() {
    cin >> nArr;
    for (int i = 1; i <= nArr; ++i) {
        cin >> a[i];
        na[i] = -a[i];
        idx.push_back(na[i]);
        idx.push_back(a[i]);
    }

    sort(idx.begin(), idx.end());
    idx.erase(unique(idx.begin(), idx.end()), idx.end());

    nTree = idx.size();
    for (int i = 1; i <= nArr; ++i) {
        a[i] = lower_bound(idx.begin(), idx.end(), a[i]) - idx.begin() + 1;
        na[i] = lower_bound(idx.begin(), idx.end(), na[i]) - idx.begin() + 1;
    }

    cout << magicFunc() << '\n';
}

int main() {
    ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);

    #define TASK "lis"
    if(fopen(TASK".inp", "r")) {
        freopen(TASK".inp", "r", stdin);
        freopen(TASK".out", "w", stdout);
    }

    int numTest = 1;
    if(isMultiTest)
        cin >> numTest;

    while(numTest--)
        process();

    return 0;
}
