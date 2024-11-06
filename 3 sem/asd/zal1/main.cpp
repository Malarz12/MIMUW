#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>  // For std::fill

using namespace std;

class MaskGenerator {
public:
    long long prev_sum;
    long long to_add;
    int current_row;
    int new_mask;

    explicit MaskGenerator(long long prev_sum_val = 0, long long to_add_val = 0, int current_row_val = 0, int new_mask_val = 0)
        : prev_sum(prev_sum_val), to_add(to_add_val), current_row(current_row_val), new_mask(new_mask_val) {}
};

// Vertical combination function
void vertical_combination(MaskGenerator& help, int mask, int column, const vector<vector<int>>& v, vector<MaskGenerator>& masks) {
    if (help.current_row < v.size() - 1 && (mask & (1 << (help.current_row + 1))) == 0) {
        long long vertical_sum = v[help.current_row][column] + v[help.current_row + 1][column];
        if (vertical_sum > 0) {
            masks.emplace_back(help.prev_sum, help.to_add + vertical_sum, help.current_row + 2, help.new_mask);
        }
    }
}

// Horizontal combination function
void horizontal_combination(MaskGenerator& help, int column, const vector<vector<int>>& v, vector<MaskGenerator>& masks) {
    if (column < v[0].size() - 1) {
        long long horizontal_sum = v[help.current_row][column] + v[help.current_row][column + 1];
        if (horizontal_sum > 0) {
            masks.emplace_back(help.prev_sum, help.to_add + horizontal_sum, help.current_row + 1, help.new_mask | (1 << help.current_row));
        }
    }
}

// Move to the next row without combining
void move_to_next_row(MaskGenerator& help, vector<MaskGenerator>& masks) {
    masks.emplace_back(help.prev_sum, help.to_add, help.current_row + 1, help.new_mask);
}

// Helper function to process each column
void process_column(int column, const vector<vector<int>>& v, vector<long long>& cur, vector<long long>& next, long long& res, int k) {
    ranges::fill(next, LLONG_MIN); // Reset the next column state

    // Process each mask in the current state
    for (int mask = 0; mask < (1 << k); ++mask) {
        if (cur[mask] != LLONG_MIN) {
            vector<MaskGenerator> masks;
            masks.emplace_back(cur[mask], 0, 0, 0);

            for (int i = 0; i < masks.size(); ++i) {
                if (MaskGenerator help = masks[i]; help.current_row < k) {
                    if ((mask & (1 << help.current_row)) == 0) {
                        // Process vertical combination
                        vertical_combination(help, mask, column, v, masks);

                        // Process horizontal combination
                        horizontal_combination(help, column, v, masks);
                    }

                    // Move to the next row without combining
                    move_to_next_row(help, masks);
                } else {
                    res = max(res, next[help.new_mask] = max(help.to_add + help.prev_sum, next[help.new_mask]));
                }
            }
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k;
    cin >> n >> k;

    vector<vector<int>> v(k, vector<int>(n));
    long long res = 0;
    vector<long long> cur(1 << k, LLONG_MIN);
    cur[0] = 0; // Initialize with 0 for the starting condition
    vector<long long> next(1 << k, LLONG_MIN);

    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < n; ++j) {
            cin >> v[i][j];
        }
    }

    for (int column = 0; column < n; ++column) {
        process_column(column, v, cur, next, res, k);  // Process the current column using the helper function
        cur = next;  // Update the current state with the results from this column
    }

    cout << res;
    return 0;
}
