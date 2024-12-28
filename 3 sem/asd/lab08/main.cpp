#include <iostream>
#include <vector>
using namespace std;

struct Node {
    int val;
    Node *parent = nullptr;
    Node *left_child = nullptr;
    Node *right_child = nullptr;
};

int final_res = 0;
Node* n1 = nullptr;
Node* n2 = nullptr;
vector<vector<int>> ancestors_map1;
vector<vector<int>> ancestors_map2;

void generate_ancestors(vector<vector<int>>& map, Node* root, int max_depth) {
    map[root->val][0] = root->parent ? root->parent->val : -1;
    for (int k = 1; (1 << k) <= max_depth; k++) {
        int ancestor = map[root->val][k - 1];
        if (ancestor == -1) break;
        map[root->val][k] = map[ancestor][k - 1];
    }
    if (root->left_child != nullptr) generate_ancestors(map, root->left_child, max_depth + 1);
    if (root->right_child != nullptr) generate_ancestors(map, root->right_child, max_depth + 1);
}

pair<Node*, int> find_diameter(Node* root) {
    if (root == nullptr) {
        return make_pair(nullptr, 0);
    }

    pair<Node*, int> left_res = find_diameter(root->left_child);
    pair<Node*, int> right_res = find_diameter(root->right_child);

    int diameter = left_res.second + right_res.second;
    if (diameter > final_res) {
        final_res = diameter;
        n1 = left_res.first;
        n2 = right_res.first;
    }

    if (root->left_child == nullptr && root->right_child == nullptr) {
        return make_pair(root, 1);
    }

    if (left_res.second > right_res.second) {
        return make_pair(left_res.first, left_res.second + 1);
    } else {
        return make_pair(right_res.first, right_res.second + 1);
    }
}

void make_root(Node* new_root) {
    Node* current = new_root;
    Node* prev = nullptr;
    Node* next = nullptr;

    while (current != nullptr) {
        next = current->parent;
        current->parent = prev;

        if (current->left_child == prev) {
            current->left_child = next;
        } else if (current->right_child == prev) {
            current->right_child = next;
        }

        prev = current;
        current = next;
    }
}

Node* clone_tree(Node* root, vector<Node*>& mapping) {
    if (root == nullptr) {
        return nullptr;
    }

    Node* new_root = new Node();
    new_root->val = root->val;
    mapping[root->val] = new_root;
    new_root->left_child = clone_tree(root->left_child, mapping);
    new_root->right_child = clone_tree(root->right_child, mapping);

    if (new_root->left_child) {
        new_root->left_child->parent = new_root;
    }
    if (new_root->right_child) {
        new_root->right_child->parent = new_root;
    }

    return new_root;
}

int log2_floor(int n) {
    int result = 0;
    while (n > 1) {
        n >>= 1;
        result++;
    }
    return result;
}

int power_of_two(int n) {
    return 1 << n;
}

int up_search(const vector<vector<int>>& ancestors_map, int node_val, int d) {
    if (d == 0) return node_val;
    if (d < 0 || node_val == -1) return -1;

    int i = log2_floor(d);
    int j = power_of_two(i);

    if (node_val >= ancestors_map.size() || ancestors_map[node_val][i] == -1) {
        return -1;
    }

    return up_search(ancestors_map, ancestors_map[node_val][i], d - j);
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int n;
    cin >> n;
    vector<Node*> nodes(n);
    ancestors_map1.resize(n + 1, vector<int>(log2_floor(n) + 1, -1));
    ancestors_map2.resize(n + 1, vector<int>(log2_floor(n) + 1, -1));

    for (int i = 0; i < n; i++) {
        nodes[i] = new Node();
        nodes[i]->val = i + 1;
    }

    for (int i = 0; i < n; i++) {
        int a, b;
        cin >> a >> b;
        if (a != -1) {
            nodes[i]->left_child = nodes[a - 1];
            nodes[a - 1]->parent = nodes[i];
        }
        if (b != -1) {
            nodes[i]->right_child = nodes[b - 1];
            nodes[b - 1]->parent = nodes[i];
        }
    }

    find_diameter(nodes[0]);
    make_root(n1);
    vector<Node*> mapping1(n + 1, nullptr);
    Node* cloned_tree1 = clone_tree(n1, mapping1);
    make_root(n2);
    vector<Node*> mapping2(n + 1, nullptr);
    Node* cloned_tree2 = clone_tree(n2, mapping2);
    generate_ancestors(ancestors_map1, cloned_tree1, 0);
    generate_ancestors(ancestors_map2, cloned_tree2, 0);
    int q;
    cin >> q;
    vector<int> results;
    for (int i = 0; i < q; i++) {
        int v, d;
        cin >> v >> d;

        int res1 = up_search(ancestors_map1, v, d);
        int res2 = up_search(ancestors_map2, v, d);

        results.push_back(res1 != -1 ? res1 : res2);
    }

    for (int res : results) {
        cout << res << endl;
    }
    return 0;
}
