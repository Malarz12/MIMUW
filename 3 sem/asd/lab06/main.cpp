#include <iostream>
#include <algorithm>
using namespace std;

struct AVLNode {
    int val;
    int count;
    int height;
    int subtreeSize;
    AVLNode* left;
    AVLNode* right;

    AVLNode(int _val, int _count) : val(_val), count(_count), height(1), subtreeSize(_count), left(nullptr), right(nullptr) {}
};

inline int getHeight(AVLNode* node) {
    return node ? node->height : 0;
}

inline int getSize(AVLNode* node) {
    return node ? node->subtreeSize : 0;
}

void recalc(AVLNode* node) {
    if (node) {
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        node->subtreeSize = getSize(node->left) + getSize(node->right) + node->count;
    }
}

AVLNode* rotateRight(AVLNode* y) {
    if (!y || !y->left) return y;
    AVLNode* x = y->left;
    AVLNode* T = x->right;

    x->right = y;
    y->left = T;

    recalc(y);
    recalc(x);

    return x;
}

AVLNode* rotateLeft(AVLNode* x) {
    if (!x || !x->right) return x;
    AVLNode* y = x->right;
    AVLNode* T = y->left;

    y->left = x;
    x->right = T;

    recalc(x);
    recalc(y);

    return y;
}

AVLNode* mergeNodes(AVLNode* l, AVLNode* r) {
    if (!l || !r) return l ? l : r;
    if (getHeight(l) > getHeight(r)) {
        l->right = mergeNodes(l->right, r);
        recalc(l);
        return l;
    } else {
        r->left = mergeNodes(l, r->left);
        recalc(r);
        return r;
    }
}

int getBalance(AVLNode* node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

AVLNode* insertBlock(AVLNode* node, int pos, int val, int count) {
    if (!node) return new AVLNode(val, count);

    int leftSize = getSize(node->left);

    if (pos <= leftSize) {
        node->left = insertBlock(node->left, pos, val, count);
    } else if (pos >= leftSize + node->count) {
        node->right = insertBlock(node->right, pos - leftSize - node->count, val, count);
    } else {
        int leftCount = pos - leftSize;
        int rightCount = node->count - leftCount;

        // Ensure valid counts
        if (leftCount < 0 || rightCount < 0) {
            throw invalid_argument("Invalid split positions");
        }

        AVLNode* rightPart = nullptr;
        if (rightCount > 0) {
            rightPart = new AVLNode(node->val, rightCount);
            rightPart->right = node->right;
        }

        node->count = leftCount;
        node->right = nullptr;

        recalc(node);
        if (rightPart) recalc(rightPart);

        AVLNode* newNode = new AVLNode(val, count);
        return mergeNodes(mergeNodes(node, newNode), rightPart);
    }

    recalc(node);

    int balance = getBalance(node);

    if (balance > 1 && pos <= getSize(node->left)) {
        return rotateRight(node);
    }
    if (balance < -1 && pos > getSize(node->left) + node->count) {
        return rotateLeft(node);
    }
    if (balance > 1 && pos > getSize(node->left)) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && pos <= getSize(node->left) + node->count) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

int getKth(AVLNode* node, int k) {
    if (!node) throw invalid_argument("Invalid k: node is null");

    int leftSize = getSize(node->left);

    if (k < 0 || k >= getSize(node)) {
        throw invalid_argument("Invalid k: out of bounds");
    }

    if (k < leftSize) return getKth(node->left, k);
    k -= leftSize;

    if (k < node->count) return node->val;
    return getKth(node->right, k - node->count);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int m;
    cin >> m;

    AVLNode* root = nullptr;
    long long w = 0;
    long long n = 0;

    while (m--) {
        char op;
        cin >> op;

        if (op == 'i') {
            long long j, x;
            int k;
            cin >> j >> x >> k;
            j = (j + w) % (n + 1);

            try {
                root = insertBlock(root, (int)j, (int)x, k);
                n += k;
            } catch (const invalid_argument& e) {
                cout << "Error: " << e.what() << "\n";
            }
        } else {
            long long j;
            cin >> j;
            j = (j + w) % n;

            try {
                long long result = getKth(root, (int)j);
                cout << result << "\n";
                w = result;
            } catch (const invalid_argument& e) {
                cout << "Error: " << e.what() << "\n";
            }
        }
    }

    return 0;
}
