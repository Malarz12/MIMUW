#include <iostream>
#include <vector>
using namespace std;

const int MOD = 1000000000;

struct Node {
    int left;
    int right;
    int val;
    Node* left_node;
    Node* right_node;

    Node(int left, int right, int val, Node* left_node = nullptr, Node* right_node = nullptr)
        : left(left), right(right), val(val), left_node(left_node), right_node(right_node) {}

    static Node* create(int left, int right, int val, Node* left_node = nullptr, Node* right_node = nullptr) {
        return new Node(left, right, val, left_node, right_node);
    }

    static Node* build_segment_tree(int left, int right) {
        Node* res = Node::create(left, right, 0);
        if (right == left) {
            return res;
        } else if (left > right) {
            return nullptr;
        } else if (right - left == 1) {
            res->left_node = Node::create(left, left, 0);
            res->right_node = Node::create(right, right, 0);
        } else {
            res->left_node = build_segment_tree(left, left + (right - left) / 2);
            res->right_node = build_segment_tree(left + (right - left) / 2 + 1, right);
        }
        return res;
    }

    static void update_segment_tree(Node* root, int val, int number) {
        root->val = (root->val + val) % MOD;
        if (root->left_node != nullptr && root->left_node->left <= number && root->left_node->right >= number)
            update_segment_tree(root->left_node, val, number);
        if (root->right_node != nullptr && root->right_node->left <= number && root->right_node->right >= number)
            update_segment_tree(root->right_node, val, number);
    }

    static int sum(Node* root, int left_border, int right_border) {
        if (root == nullptr || root->right < left_border || root->left > right_border) return 0;
        if (root->left > left_border && root->right <= right_border) return root->val;
        int left_sum = sum(root->left_node, left_border, right_border);
        int right_sum = sum(root->right_node, left_border, right_border);

        return (left_sum + right_sum) % MOD;
    }

    static void clean_up(Node* root) {
        if(root -> left_node != nullptr) {
            clean_up(root -> left_node);
        }
        if(root ->right_node != nullptr) {
            clean_up(root -> right_node);
        }
        delete root;
    }
};

int main() {
    int n, k;
    cin >> n >> k;
    vector<int> permutation(n);
    for (int i = 0; i < n; i++) {
        cin >> permutation[i];
    }

    if (k == 1) return n;
    vector<Node*> k_trees;
    for (int i = 0; i < k; i++) {
        Node* temp = Node::build_segment_tree(1, n);
        k_trees.push_back(temp);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 1; j <= k && j <= i + 1; j++) {
            if (j == 1) {
                Node::update_segment_tree(k_trees[0], 1, permutation[i]);
            } else {
                int sum = Node::sum(k_trees[j - 2], permutation[i], n);
                Node::update_segment_tree(k_trees[j - 1], sum, permutation[i]);
            }
        }
    }
    cout << Node::sum(k_trees[k - 1], 0, n);
    for (int i = 0; i < k_trees.size(); i++) {
        Node::clean_up(k_trees[i]);
    }
    return 0;
}