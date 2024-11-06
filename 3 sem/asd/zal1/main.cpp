#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>

using namespace std;

class MaskGenerator {
public:
    long long prev_sum;
    long long to_add;
    int currentRow;
    int new_mask;

    explicit MaskGenerator(long long prev_sum_val = 0, long long to_add_val = 0, int currentRow_val = 0, int new_mask_val = 0)
        : prev_sum(prev_sum_val), to_add(to_add_val), currentRow(currentRow_val), new_mask(new_mask_val) {}
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k;
    cin >> n >> k;

    vector<vector<int>> v(k, vector<int>(n));
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < n; ++j) {
            cin >> v[i][j];
        }
    }

    long long res = 0;
    vector<long long> cur(1 << k, LLONG_MIN);
    cur[0] = 0; // Initialize with 0 for the starting condition
    vector<long long> next(1 << k, LLONG_MIN);

    for (int column = 0; column < n; ++column) {
        ranges::fill(next, LLONG_MIN); // Reset the next column state

        // Process each mask in the current state
        for (int mask = 0; mask < (1 << k); ++mask) {
            if (cur[mask] != LLONG_MIN) {
                vector<MaskGenerator> masks;
                masks.emplace_back(cur[mask], 0, 0, 0);

                for (int i = 0; i < masks.size(); ++i) {
                    if (MaskGenerator help = masks[i]; help.currentRow < k) {
                        if ((mask & (1 << help.currentRow)) == 0) {
                            // Horizontal combination if within bounds
                            if (column < n - 1) {
                                if (const long long sum = v[help.currentRow][column] + v[help.currentRow][column + 1]; sum > 0) {
                                    masks.emplace_back(help.prev_sum, help.to_add + sum, help.currentRow + 1, help.new_mask | (1 << help.currentRow));
                                }
                            }
                            // Vertical combination if within bounds
                            if (help.currentRow < k - 1 && (mask & (1 << (help.currentRow + 1))) == 0) {
                                if (const long long sum = v[help.currentRow][column] + v[help.currentRow + 1][column]; sum > 0) {
                                    masks.emplace_back(help.prev_sum, help.to_add + sum, help.currentRow + 2, help.new_mask);
                                }
                            }
                        }
                        // Move to the next row without combining
                        masks.emplace_back(help.prev_sum, help.to_add, help.currentRow + 1, help.new_mask);
                    } else {
                        res = max(res,next[help.new_mask] = max(help.to_add + help.prev_sum, next[help.new_mask]));
                    }
                }
            }
        }
        // Update the current state with the results from this column
        cur = next;
    }
    cout << res;
    return 0;
}