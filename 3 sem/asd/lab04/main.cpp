#include <iostream>
#include <vector>
using namespace std;

const int MOD = 1000000000;

struct Node {
    int left, right;
    int val; // 0 - mixed, 1 - black, 2 - white
    int lazy; // -1 don't update, 1 black, 2 white
    int white_count; // Number of white cells in this segment
    Node* left_node;
    Node* right_node;

    Node(int left, int right, int val, Node* left_node = nullptr, Node* right_node = nullptr)
        : left(left), right(right), val(val), lazy(-1), white_count((val == 2) ? (right - left + 1) : 0),
          left_node(left_node), right_node(right_node) {}

    static Node* build_segment_tree(int left, int right) {
        if (left == right) {
            return new Node(left, right, 1);  // Initialize each cell as black
        }
        int mid = (left + right) / 2;
        return new Node(left, right, 1, build_segment_tree(left, mid), build_segment_tree(mid + 1, right));
    }

    void propagate() {
        if (lazy == -1) return;

        val = lazy;
        white_count = (val == 2) ? (right - left + 1) : 0;

        if (left != right) {
            if (left_node) left_node->lazy = lazy;
            if (right_node) right_node->lazy = lazy;
        }
        lazy = -1;
    }

    int modify(int l, int r, int color) {
        propagate();
        if (right < l || left > r) return white_count;

        if (l <= left && right <= r) {
            lazy = color;
            propagate();
            return white_count;
        }

        int left_result = (left_node ? left_node->modify(l, r, color) : 0);
        int right_result = (right_node ? right_node->modify(l, r, color) : 0);

        if (left_node && right_node && left_node->val == right_node->val) {
            val = left_node->val;
        } else {
            val = 0;
        }
        white_count = left_result + right_result;
        return white_count;
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    cin >> n >> m;
    Node* root = Node::build_segment_tree(1, n);
    vector<int> res;

    for (int i = 0; i < m; i++) {
        int a, b;
        char c;
        cin >> a >> b >> c;

        int color = (c == 'B') ? 2 : 1;
        res.push_back(root->modify(a, b, color));
    }

    for (int ans : res) {
        cout << ans << "\n";
    }

    return 0;
}
