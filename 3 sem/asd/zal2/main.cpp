#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Node {
    int left, right;
    int prefixLength, prefixFirst;
    int sufixLength, sufixEnd;
    int combinedLength;
    int lazy;

    Node *left_node, *right_node;

    Node(int l = 0, int r = 0, int pL = 0, int pF = 0, int sL = 0, int sE = 0, int lz = 0, Node* lChild = nullptr, Node* rChild = nullptr)
        : left(l), right(r), prefixLength(pL), prefixFirst(pF), sufixLength(sL), sufixEnd(sE), combinedLength(pL), lazy(lz), left_node(lChild), right_node(rChild) {}

    static Node* buildTree(int left, int right) {
        if (left == right) {
            return new Node(left, right, 1, 1, 1, 1, 0);
        }
        int mid = (left + right) / 2;
        return new Node(left, right, right - left + 1, 1, right - left + 1, 1, 0,
            buildTree(left, mid), buildTree(mid + 1, right));
    }

    static void propagate(Node* root) {
        if (root->lazy) {
            root->prefixFirst += root->lazy;
            root->sufixEnd += root->lazy;

            if (root->left != root->right) {
                root->left_node->lazy += root->lazy;
                root->right_node->lazy += root->lazy;
            }

            root->lazy = 0;
        }
    }

    static void update(Node* root, int left, int right, int k) {
        if (root->right < left || root->left > right) return;

        if (root->left >= left && root->right <= right) {
            root->lazy += k;
            propagate(root);
            return;
        }

        propagate(root);

        update(root->left_node, left, right, k);
        update(root->right_node, left, right, k);

        propagate(root->left_node);
        propagate(root->right_node);

        root->prefixLength = root->left_node->prefixLength;
        root->prefixFirst = root->left_node->prefixFirst;
        if (root->left_node->prefixLength == root->left_node->right - root->left_node->left + 1 && root->left_node->sufixEnd <= root->right_node->prefixFirst) {
            root->prefixLength += root->right_node->prefixLength;
        }

        root->sufixLength = root->right_node->sufixLength;
        root->sufixEnd = root->right_node->sufixEnd;
        if (root->right_node->sufixLength == root->right_node->right - root->right_node->left + 1 && root->left_node->sufixEnd <= root->right_node->prefixFirst) {
            root->sufixLength += root->left_node->sufixLength;
        }

        root->combinedLength = max({root->left_node->combinedLength, root->right_node->combinedLength});
        if (root->left_node->sufixEnd <= root->right_node->prefixFirst) {
            root->combinedLength = max(root->combinedLength, root->left_node->sufixLength + root->right_node->prefixLength);
        }
    }

    static vector<pair<int, int>> longest(Node* root, int left, int right) {
        if (root == nullptr || root->right < left || root->left > right) {
            return {{0, 0}, {0, 0}, {0, 0}};
        }

        propagate(root);

        if (root->left >= left && root->right <= right) {
            return {{root->prefixLength, root->prefixFirst}, {root->sufixLength, root->sufixEnd}, {root->combinedLength, root->sufixEnd}};
        }

        vector<pair<int, int>> leftResults = longest(root->left_node, left, right);
        vector<pair<int, int>> rightResults = longest(root->right_node, left, right);

        int prefixLength = leftResults[0].first;
        int prefixFirst = leftResults[0].second;
        if (leftResults[0].first == root->left_node->right - root->left_node->left + 1
            && leftResults[1].second <= rightResults[0].second) {
            prefixLength = leftResults[0].first + rightResults[0].first;
            prefixFirst = leftResults[0].second;
        }

        int sufixLength = rightResults[1].first;
        int sufixEnd = rightResults[1].second;
        if (rightResults[1].first == root->right_node->right - root->right_node->left + 1
            && leftResults[1].second <= rightResults[0].second) {
            sufixLength = rightResults[1].first + leftResults[1].first;
            sufixEnd = rightResults[1].second;
        }

        int combinedLength = max(leftResults[2].first, rightResults[2].first);
        if (leftResults[1].second <= rightResults[0].second) {
            combinedLength = max(combinedLength, leftResults[1].first + rightResults[0].first);
        }

        return {{prefixLength, prefixFirst}, {sufixLength, sufixEnd}, {combinedLength, sufixEnd}};
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    int n, m;
    cin >> n >> m;

    Node* root = Node::buildTree(1, n);

    for (int i = 0; i < m; i++) {
        char action;
        cin >> action;

        if (action == 'N') {
            int ai, bi, ki;
            cin >> ai >> bi >> ki;
            Node::update(root, ai, bi, ki);
        } else if (action == 'C') {
            int ai, bi;
            cin >> ai >> bi;
            vector<pair<int, int>> result = Node::longest(root, ai, bi);
            cout << max(result[0].first, max(result[1].first, result[2].first)) << endl;
        }
    }

    return 0;
}
