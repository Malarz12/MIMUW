#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
using namespace std;

vector<vector<int>> SPB;

void generate_SPB(vector<char>& word) {
    int n = word.size();
    int logN = log2(n) + 1;
    SPB.resize(logN, vector<int>(n, 0));

    for (int i = 0; i < n; ++i) {
        SPB[0][i] = word[i] - 'a';
    }

    vector<pair<pair<int, int>, int>> to_sort;

    for (int j = 1; j < logN; ++j) {
        to_sort.clear();

        for (int i = 0; i + (1 << j) <= n; ++i) {
            pair<int, int> temp = make_pair(SPB[j - 1][i], SPB[j - 1][i + (1 << (j - 1))]);
            to_sort.push_back({temp, i});
        }

        sort(to_sort.begin(), to_sort.end());

        int rank = 0;
        SPB[j][to_sort[0].second] = rank;

        for (int k = 1; k < to_sort.size(); ++k) {
            if (to_sort[k].first != to_sort[k - 1].first) {
                ++rank;
            }
            SPB[j][to_sort[k].second] = rank;
        }
    }
}

void compare(pair<int, int>& interval1, pair<int, int>& interval2) {
    int len1 = interval1.second - interval1.first + 1;
    int len2 = interval2.second - interval2.first + 1;
    int min_len = min(len1, len2);

    int logLen = log2(min_len);

    pair<int, int> value1 = {
        SPB[logLen][interval1.first],
        SPB[logLen][interval1.first + min_len - (1 << logLen)]
    };

    pair<int, int> value2 = {
        SPB[logLen][interval2.first],
        SPB[logLen][interval2.first + min_len - (1 << logLen)]
    };

    if (value1 == value2) {
        if (len1 == len2) {
            cout << "=" << endl;
        } else if (len1 < len2) {
            cout << "<" << endl;
        } else {
            cout << ">" << endl;
        }
    } else {
        if (value1 < value2) {
            cout << "<" << endl;
        } else {
            cout << ">" << endl;
        }
    }
}

int main() {
    int n, q;
    cin >> n >> q;

    string input_word;
    cin >> input_word;

    vector<char> word(input_word.begin(), input_word.end());
    generate_SPB(word);

    for (int i = 0; i < q; ++i) {
        int l1, r1, l2, r2;
        cin >> l1 >> r1 >> l2 >> r2;

        pair<int, int> interval1 = {l1 - 1, r1 - 1};
        pair<int, int> interval2 = {l2 - 1, r2 - 1};
        compare(interval1, interval2);
    }

    return 0;
}