#include <iostream>
#include <vector>
using namespace std;

const int MOD = 1000000000;  // Modulo value for calculations

// Structure to represent a segment tree node
struct Node {
    int left;  // Left index of the segment
    int right;  // Right index of the segment
    int val;  // Value stored at this node
    Node* left_node;  // Pointer to the left child node
    Node* right_node;  // Pointer to the right child node

    // Constructor to initialize a node
    Node(int left, int right, int val, Node* left_node = nullptr, Node* right_node = nullptr)
        : left(left), right(right), val(val), left_node(left_node), right_node(right_node) {}

    // Factory method to create a new node
    static Node* create(int left, int right, int val, Node* left_node = nullptr, Node* right_node = nullptr) {
        return new Node(left, right, val, left_node, right_node);
    }

    // Build a segment tree for the range [left, right]
    static Node* build_segment_tree(int left, int right) {
        Node* res = Node::create(left, right, 0);
        if (right == left) {
            return res;  // Leaf node (single element)
        } else if (left > right) {
            return nullptr;  // Invalid range
        } else if (right - left == 1) {
            // Create two leaf nodes for the range of size 1
            res->left_node = Node::create(left, left, 0);
            res->right_node = Node::create(right, right, 0);
        } else {
            // Recursively build the left and right subtrees
            res->left_node = build_segment_tree(left, left + (right - left) / 2);
            res->right_node = build_segment_tree(left + (right - left) / 2 + 1, right);
        }
        return res;
    }

    // Update the segment tree by adding 'val' at position 'number'
    static void update_segment_tree(Node* root, int val, int number) {
        root->val = (root->val + val) % MOD;  // Update the current node value
        if (root->left_node != nullptr && root->left_node->left <= number && root->left_node->right >= number)
            update_segment_tree(root->left_node, val, number);  // Update the left subtree
        if (root->right_node != nullptr && root->right_node->left <= number && root->right_node->right >= number)
            update_segment_tree(root->right_node, val, number);  // Update the right subtree
    }

    // Query the sum of values in the range [left_border, right_border]
    static int sum(Node* root, int left_border, int right_border) {
        if (root == nullptr || root->right < left_border || root->left > right_border)
            return 0;  // No overlap with the query range
        if (root->left >= left_border && root->right <= right_border)
            return root->val;  // Fully within the range

        // Query both left and right children
        int left_sum = sum(root->left_node, left_border, right_border);
        int right_sum = sum(root->right_node, left_border, right_border);

        return (left_sum + right_sum) % MOD;  // Combine the results
    }

    // Clean up memory by deleting all nodes in the tree
    static void clean_up(Node* root) {
        if (root->left_node != nullptr) {
            clean_up(root->left_node);  // Recursively clean up left subtree
        }
        if (root->right_node != nullptr) {
            clean_up(root->right_node);  // Recursively clean up right subtree
        }
        delete root;  // Delete the current node
    }
};

int main() {
    int n, k;
    cin >> n >> k;
    vector<int> permutation(n);

    // Read the permutation
    for (int i = 0; i < n; i++) {
        cin >> permutation[i];
    }

    // Base case for k = 1
    if (k == 1) return n;

    // Vector to store k segment trees
    vector<Node*> k_trees;
    for (int i = 0; i < k; i++) {
        Node* temp = Node::build_segment_tree(1, n);  // Build a new segment tree for each 'k'
        k_trees.push_back(temp);
    }

    // Process the permutation and update the segment trees
    for (int i = 0; i < n; i++) {
        for (int j = 1; j <= k && j <= i + 1; j++) {
            if (j == 1) {
                // For the first tree, just update the segment tree at position 'permutation[i]'
                Node::update_segment_tree(k_trees[0], 1, permutation[i]);
            } else {
                // For other trees, query the previous tree for the sum and update the current tree
                int sum_val = Node::sum(k_trees[j - 2], permutation[i], n);
                Node::update_segment_tree(k_trees[j - 1], sum_val, permutation[i]);
            }
        }
    }

    // Output the final result, which is the sum for the last tree
    cout << Node::sum(k_trees[k - 1], 0, n);

    // Clean up the memory used by the segment trees
    for (int i = 0; i < k_trees.size(); i++) {
        Node::clean_up(k_trees[i]);
    }

    return 0;
}
