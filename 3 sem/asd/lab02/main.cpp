#include <iostream>
#include <unordered_map>
using namespace std;

unordered_map<string, int> frequencyMap;
unordered_map<int, string> binaryMap;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m, temp;
    cin >> n >> m;

    for(int i = 0; i < n * m; i++) {
        cin >> temp;
        binaryMap[temp] += (i % n < n / 2) ? '0' : '1';
    }

    int maxFrequency = 1;
    for(const auto& pair : binaryMap) {
        maxFrequency = max(maxFrequency, ++frequencyMap[pair.second]);
    }

    cout << maxFrequency;
    return 0;
}