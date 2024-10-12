#include <iostream>
#include <vector>

using namespace std;

vector<int> main_v;
vector<vector<vector<int>>> memo;

pair<int, int> rec(int start, int end, int start_end) {
    if (main_v.size() == 1) {
        return make_pair(0, 1);
    }
    if (main_v.size() == 2) {
        return make_pair(0, main_v[start] > main_v[end] ? 0 : 2);
    }
    if (end - start == 1) {
        if (main_v[start] > main_v[end]) {
            return make_pair(start, 0);
        }
        int boundaryValue = start_end == 0 ? main_v[start - 1] : main_v[end + 1];
        bool validLeft = start_end == 0 ? main_v[start] > boundaryValue && main_v[end] > boundaryValue
                                          : main_v[start] < boundaryValue && main_v[end] < boundaryValue;
        bool validRight = start_end == 0 ? main_v[start] < boundaryValue && main_v[end] < boundaryValue
                                           : main_v[start] > boundaryValue && main_v[end] > boundaryValue;

        return make_pair(boundaryValue, validLeft ? 2 : validRight ? 0 : 1);
    }

    if (memo[start][end][start_end] != -1) {
        int boundaryValue = start_end == 0 ? main_v[start - 1] : main_v[end + 1];
        return make_pair(boundaryValue, memo[start][end][start_end]);
    }

    int res = 0;

    if (start != main_v.size() - 1) {
        pair<int, int> help = rec(start + 1, end, 0);
        if ((start_end == 0 && help.first > main_v[start - 1]) || (start_end == 1 && help.first < main_v[end + 1])) {
            res = (res + help.second) % 1000000000;
        }
    }
    if (end != 0) {
        pair<int, int> help = rec(start, end - 1, 1);
        if ((start_end == 0 && help.first > main_v[start - 1]) || (start_end == 1 && help.first < main_v[end + 1])) {
            res = (res + help.second) % 1000000000;
        }
    }

    memo[start][end][start_end] = res;
    return make_pair(start_end == 0 ? main_v[start - 1] : main_v[end + 1], res);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    cin >> n;
    main_v.resize(n);
    memo.resize(n + 1, vector<vector<int>>(n + 1, vector<int>(2, -1)));
    for (int i = 0; i < n; i++) {
        cin >> main_v[i];
    }
    cout << rec(0, main_v.size() - 1, 0).second;
    cout << '\n';
    return 0;
}
