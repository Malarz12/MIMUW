#include <iostream>
#include <vector>
#include <climits>
using namespace std;


int shortest_distance(vector<char> design) {
    int res = INT_MAX;
    int prev = 0;
    while(design[prev] == '*' && prev < design.size()) {
        prev++;
    }
    if(prev == design.size()) {
        return res;
    }
    for(int next = prev + 1; next < design.size(); next++) {
        if(design[next] != design[prev] && design[next] != '*') {
            if(next - prev < res) {
                res = next - prev;
            }
            prev = next;
        } else if(design[next] == design[prev]) {
            prev = next;
        }
    }
    return res;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string text;
    cin >> text;
    vector<char> design(text.begin(), text.end());
    if(shortest_distance(design) == INT_MAX) {
        cout << 1;
    } else {
        cout << design.size() + 1 - shortest_distance(design);
    }
    cout << "\n";
    return 0;
}