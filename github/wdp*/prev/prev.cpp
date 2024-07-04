#include  "prev.h"
#include  <iostream>
#include  <vector>
#include  <algorithm>
#include  <climits>
#include  <cassert>

using namespace std;

// Structure representing an interval tree.
typedef struct tree {
  pair < int, int > interval;
  int value;
  struct tree * left, * right;
}
tree;

// Vectors for storing positive trees, negative trees, and trees to delete.
vector < tree * > positive_trees;
vector < tree * > negative_trees;
vector < tree * > to_delete;
int current_index = 0;

// Function to create a tree for a given interval.
tree * extend(pair < int, int > interval, int v) {
  tree * result = new tree;
  result -> interval = interval;
  result -> left = nullptr;
  result -> right = nullptr;
  // Interval split.
  if (interval.first != interval.second) {
    long long mid = interval.first;
    mid = mid + interval.second;
    mid = mid - 1;
    mid = mid / 2;
    int mid1 = (int) mid;
    // Create left or right subtree.
    if (mid1 >= v) {
      result -> left = extend(make_pair(interval.first, mid1), v);
    } else {
      result -> right = extend(make_pair(mid1 + 1, interval.second), v);
    }
    // Update node value.
    if (result -> right != nullptr && result -> left != nullptr) result -> value = max(result -> right -> value, result -> left -> value);
    else if (result -> right != nullptr) result -> value = result -> right -> value;
    else result -> value = result -> left -> value;
  } else {
    // Leaf node.
    result -> value = current_index;
    result -> left = nullptr;
    result -> right = nullptr;
  }
  // Add tree to the vector of trees to delete.
  to_delete.push_back(result);
  return result;
}

// Function to update a tree with a new element.
tree * update(tree * t1, pair < int, int > interval, int v) {
  tree * result = new tree;
  result -> interval = interval;
  result -> left = nullptr;
  result -> right = nullptr;
  // Copy tree structure.
  if (t1 -> left != nullptr) result -> left = t1 -> left;
  if (t1 -> right != nullptr) result -> right = t1 -> right;

  // Interval split.
  if (interval.first != interval.second) {
    long long mid = interval.first;
    mid = mid + interval.second;
    mid = mid - 1;
    mid = mid / 2;
    int mid1 = (int) mid;
    // Update left or right subtree.
    if (mid1 >= v) {
      if (result -> left != nullptr) result -> left = update(result -> left, make_pair(interval.first, mid1), v);
      else result -> left = extend(make_pair(interval.first, mid1), v);
    } else {
      if (result -> right != nullptr) result -> right = update(result -> right, make_pair(mid1 + 1, interval.second), v);
      else result -> right = extend(make_pair(mid1 + 1, interval.second), v);
    }
    // Update node value.
    if (result -> right != nullptr && result -> left != nullptr) result -> value = max(result -> right -> value, result -> left -> value);
    else if (result -> right != nullptr) result -> value = result -> right -> value;
    else result -> value = result -> left -> value;
  } else {
    // Leaf node.
    result -> value = current_index;
    result -> left = nullptr;
    result -> right = nullptr;
  }
  // Add tree to the vector of trees to delete.
  to_delete.push_back(result);
  return result;
}

// Add an element to the end of sequence X.
void pushBack(int v) {
  if (v >= 0) {
    if (positive_trees.empty() || positive_trees[positive_trees.size() - 1] == nullptr) {
      // Create a positive tree.
      pair < int, int > para = make_pair(0, INT_MAX);
      tree * v1 = extend(para, v);
      positive_trees.push_back(v1);
    } else {
      // Update the positive tree.
      tree * v1 = positive_trees[positive_trees.size() - 1];
      v1 = update(v1, v1 -> interval, v);
      positive_trees.push_back(v1);
    }
    // Initialize the negative tree.
    if (negative_trees.empty()) {
      negative_trees.push_back(nullptr);
      to_delete.push_back(negative_trees[negative_trees.size() - 1]);
    } else {
      negative_trees.push_back(negative_trees[negative_trees.size() - 1]);
    }
  } else {
    if (negative_trees.empty() || negative_trees[negative_trees.size() - 1] == nullptr) {
      // Create a negative tree.
      pair < int, int > para = make_pair(INT_MIN, -1);
      tree * v1 = extend(para, v);
      negative_trees.push_back(v1);
    } else {
      // Update the negative tree.
      tree * v1 = negative_trees[negative_trees.size() - 1];
      v1 = update(v1, v1 -> interval, v);
      negative_trees.push_back(v1);
    }
    // Initialize the positive tree.
    if (positive_trees.empty()) {
      positive_trees.push_back(nullptr);
      to_delete.push_back(positive_trees[positive_trees.size() - 1]);
    } else {
      positive_trees.push_back(positive_trees[positive_trees.size() - 1]);
    }
  }
  current_index++;
}

// Initialize sequence X with the values of vector x.
void init(const vector < int > & x) {
  current_index = 0;
  to_delete.clear();
  positive_trees.clear();
  negative_trees.clear();

  for (size_t i = 0; i < x.size(); i++) {
    pushBack(x[i]);
  }
}

// Function to search the tree for the maximum in a given interval.
int search(tree * n1, int lo, int hi) {
  if (n1 != nullptr) {
    // Check if the tree interval is within the given interval.
    if (n1 -> interval.first >= lo && n1 -> interval.second <= hi) return n1 -> value;
    else if (n1 -> left != nullptr && n1 -> right != nullptr) return max(search(n1 -> left, lo, hi), search(n1 -> right, lo, hi));
    else if (n1 -> left != nullptr) return search(n1 -> left, lo, hi);
    else if (n1 -> right != nullptr) return search(n1 -> right, lo, hi);
  }
  return -1;
}

// Function to compute the value of prevInRange(i, [lo, hi]).
int prevInRange(int i, int lo, int hi) {
  if (lo < 0 && hi >= 0) return max(search(positive_trees[i], 0, hi), search(negative_trees[i], lo, -1));
  else if (lo >= 0) return search(positive_trees[i], lo, hi);
  else if (hi <= -1) return search(negative_trees[i], lo, hi);
  else return -1;
}

// Free memory used to handle sequence X.
void done() {
  for (size_t i = 0; i < to_delete.size(); i++) {
    delete to_delete[i];
  }

  positive_trees.clear();
  negative_trees.clear();
  to_delete.clear();

  current_index = 0;
}