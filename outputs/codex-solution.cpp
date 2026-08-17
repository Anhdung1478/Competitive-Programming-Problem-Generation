#include <bits/stdc++.h>
using namespace std;

namespace {

constexpr int INF = 1'000'000'000;

struct Node {
    int minimum = INF;
    int minimumCount = 0;
    int lazy = 0;
};

class SegmentTree {
public:
    explicit SegmentTree(const vector<int>& gap)
        : size_(static_cast<int>(gap.size()) - 1), tree_(4 * size_ + 4) {
        build(1, 1, size_, gap);
    }

    void add(int left, int right, int value) {
        add(1, 1, size_, left, right, value);
    }

    Node query(int left, int right) {
        return query(1, 1, size_, left, right);
    }

    void assignPoint(int position, int value) {
        assignPoint(1, 1, size_, position, value);
    }

    int findNegative(int left, int right, int& negativeGap) {
        return findNegative(1, 1, size_, left, right, negativeGap);
    }

private:
    int size_;
    vector<Node> tree_;

    static Node mergeNodes(const Node& left, const Node& right) {
        Node result;
        result.minimum = min(left.minimum, right.minimum);
        if (left.minimum == result.minimum) {
            result.minimumCount += left.minimumCount;
        }
        if (right.minimum == result.minimum) {
            result.minimumCount += right.minimumCount;
        }
        return result;
    }

    void apply(int id, int value) {
        tree_[id].minimum -= value;
        tree_[id].lazy += value;
    }

    void push(int id) {
        if (tree_[id].lazy == 0) {
            return;
        }
        apply(id << 1, tree_[id].lazy);
        apply(id << 1 | 1, tree_[id].lazy);
        tree_[id].lazy = 0;
    }

    void pull(int id) {
        tree_[id] = mergeNodes(tree_[id << 1], tree_[id << 1 | 1]);
    }

    void build(int id, int left, int right, const vector<int>& gap) {
        if (left == right) {
            tree_[id].minimum = gap[left];
            tree_[id].minimumCount = 1;
            return;
        }
        const int middle = (left + right) / 2;
        build(id << 1, left, middle, gap);
        build(id << 1 | 1, middle + 1, right, gap);
        pull(id);
    }

    void add(int id, int left, int right, int queryLeft, int queryRight,
             int value) {
        if (queryLeft <= left && right <= queryRight) {
            apply(id, value);
            return;
        }
        push(id);
        const int middle = (left + right) / 2;
        if (queryLeft <= middle) {
            add(id << 1, left, middle, queryLeft, queryRight, value);
        }
        if (middle < queryRight) {
            add(id << 1 | 1, middle + 1, right, queryLeft, queryRight, value);
        }
        pull(id);
    }

    Node query(int id, int left, int right, int queryLeft, int queryRight) {
        if (queryLeft <= left && right <= queryRight) {
            return tree_[id];
        }
        push(id);
        const int middle = (left + right) / 2;
        if (queryRight <= middle) {
            return query(id << 1, left, middle, queryLeft, queryRight);
        }
        if (middle < queryLeft) {
            return query(id << 1 | 1, middle + 1, right, queryLeft, queryRight);
        }
        return mergeNodes(
            query(id << 1, left, middle, queryLeft, queryRight),
            query(id << 1 | 1, middle + 1, right, queryLeft, queryRight));
    }

    void assignPoint(int id, int left, int right, int position, int value) {
        if (left == right) {
            tree_[id] = Node{value, 1, 0};
            return;
        }
        push(id);
        const int middle = (left + right) / 2;
        if (position <= middle) {
            assignPoint(id << 1, left, middle, position, value);
        } else {
            assignPoint(id << 1 | 1, middle + 1, right, position, value);
        }
        pull(id);
    }

    int findNegative(int id, int left, int right, int queryLeft,
                     int queryRight, int& negativeGap) {
        if (right < queryLeft || queryRight < left || tree_[id].minimum >= 0) {
            return -1;
        }
        if (left == right) {
            negativeGap = tree_[id].minimum;
            return left;
        }
        push(id);
        const int middle = (left + right) / 2;
        const int fromLeft =
            findNegative(id << 1, left, middle, queryLeft, queryRight,
                         negativeGap);
        if (fromLeft != -1) {
            return fromLeft;
        }
        return findNegative(
            id << 1 | 1, middle + 1, right, queryLeft, queryRight,
            negativeGap);
    }
};

void generateLuckyNumbers(int value, vector<int>& lucky) {
    if (value > 10'000) {
        return;
    }
    if (value > 0) {
        lucky.push_back(value);
    }
    generateLuckyNumbers(value * 10 + 4, lucky);
    generateLuckyNumbers(value * 10 + 7, lucky);
}

}  // namespace

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;

    vector<int> value(n + 1);
    for (int i = 1; i <= n; ++i) {
        cin >> value[i];
    }

    vector<int> lucky;
    generateLuckyNumbers(0, lucky);
    sort(lucky.begin(), lucky.end());
    lucky.push_back(INF);  // Values never exceed 10^4, so this is only a sentinel.

    vector<int> nextLucky(n + 1);
    vector<int> gap(n + 1);
    for (int i = 1; i <= n; ++i) {
        nextLucky[i] = *lower_bound(lucky.begin(), lucky.end(), value[i]);
        gap[i] = nextLucky[i] - value[i];
    }

    SegmentTree tree(gap);

    while (q-- > 0) {
        string operation;
        int left, right;
        cin >> operation >> left >> right;

        if (operation == "count") {
            const Node result = tree.query(left, right);
            cout << (result.minimum == 0 ? result.minimumCount : 0) << '\n';
            continue;
        }

        int delta;
        cin >> delta;
        tree.add(left, right, delta);

        // Positive additions can pass one or more lucky numbers. Repair every
        // affected leaf until all distances are nonnegative again.
        while (true) {
            int negativeGap = 0;
            const int position = tree.findNegative(left, right, negativeGap);
            if (position == -1) {
                break;
            }

            value[position] = nextLucky[position] - negativeGap;
            nextLucky[position] =
                *lower_bound(lucky.begin(), lucky.end(), value[position]);
            tree.assignPoint(position, nextLucky[position] - value[position]);
        }
    }

    return 0;
}
