#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;


int f_control_number(vector<long>& even, vector<long>& odd, int help) {
    if(help == 1) {
        int control_number = odd[odd.size() - 1];
        odd.pop_back();
        return control_number;
    } else {
        int control_number = odd[odd.size() - 1] + even[even.size() - 1];
        odd.pop_back();
        even.pop_back();
        return control_number;
    }
}
vector<long> k_sufix(vector<int> v, int help) {

    vector<long> even;
    vector<long> odd;

    for(int i = 0; i < v.size(); i++) {
        if(v[i] % 2 == 0) {
            even.push_back(v[i]);
        } else {
            odd.push_back(v[i]);
        }
    }
    if(odd.size() == 0 || (help == 0 && even.size() == 0)) {
        vector<long> result;
        result.push_back(-1);
        return result;
    }
    int control_number = f_control_number(even, odd, help);

    vector<long> bubbles;
    for(int i = odd.size() - 1; i >= 0; i = i - 2) {
        if(i > 0) {
            bubbles.push_back(odd[i] + odd[i - 1]);
        }
    }

    for(int i = even.size() - 1; i >= 0 ; i = i - 2) {
        if(i > 0) {
            bubbles.push_back(even[i] + even[i - 1]);
        }
    }
    sort(bubbles.begin(), bubbles.end());

    vector<long> result(bubbles.size() + 1);
    result[result.size() - 1] = control_number;

    for(int i = bubbles.size() - 1; i >= 0; i--) {
        result[i] = bubbles[i] + result[i + 1];
    }

    return result;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    int n, m;
    vector<int> v;
    cin >> n;
    for(int i = 0; i < n; i++) {
        int temp;
        cin >> temp;
        v.push_back(temp);
    }
    vector<long> odd_sufix = k_sufix(v, 1);
    vector<long> even_sufix = k_sufix(v, 0);
    vector<long> result;
    cin >> m;
    for(int i = 0; i < m; i++) {
        int k;
        cin >> k;
        if(k % 2 == 0) {
            if(even_sufix[0] == -1 || k / 2 > even_sufix.size()) {
                result.push_back(-1);
            } else {
                result.push_back(even_sufix[even_sufix.size() - (k / 2)]);
            }
        } else {
            if(odd_sufix[0] == -1 || (k - 1) / 2 > odd_sufix.size()) {
                result.push_back(-1);
            } else {
                result.push_back(odd_sufix[odd_sufix.size() - ((k + 1) / 2)]);
            }
        }
    }
    for(int i = 0; i < result.size(); i++) {
        cout << result[i];
        cout << "\n";
    }
    return 0;
}