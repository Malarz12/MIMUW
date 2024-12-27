#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <queue>
#include <limits>
using namespace std;

struct Node {
    int index;
    int x, y;
    vector<pair<Node*, int>> neighbors;
    Node(int idx, int xCoord, int yCoord) : index(idx), x(xCoord), y(yCoord) {}
};

void addEdge(Node* a, Node* b) {
    int length = min(abs(a->x - b->x), abs(a->y - b->y));
    a->neighbors.push_back({b, length});
    b->neighbors.push_back({a, length});
}

int dijkstra(vector<Node*>& nodes, int startIdx, int endIdx) {
    int n = nodes.size();
    vector<int> distances(n + 1, numeric_limits<int>::max());
    distances[startIdx] = 0;

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
    pq.push({0, startIdx});

    while (!pq.empty()) {
        auto [currentDistance, currentNodeIdx] = pq.top();
        pq.pop();

        if (currentDistance > distances[currentNodeIdx]) continue;

        Node* currentNode = nodes[currentNodeIdx - 1];
        for (const auto& neighbor : currentNode->neighbors) {
            Node* neighborNode = neighbor.first;
            int weight = neighbor.second;
            int newDistance = currentDistance + weight;

            if (newDistance < distances[neighborNode->index]) {
                distances[neighborNode->index] = newDistance;
                pq.push({newDistance, neighborNode->index});
            }
        }
    }

    return distances[endIdx];
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    int n;
    cin >> n;

    vector<Node*> nodes;

    for (int i = 0; i < n; ++i) {
        int x, y;
        cin >> x >> y;
        nodes.push_back(new Node(i + 1, x, y));
    }

    vector<Node*> sortedByX = nodes;
    vector<Node*> sortedByY = nodes;

    sort(sortedByX.begin(), sortedByX.end(), [](const Node* a, const Node* b) {
        return a->x < b->x;
    });

    sort(sortedByY.begin(), sortedByY.end(), [](const Node* a, const Node* b) {
        return a->y < b->y;
    });

    for (int i = 0; i < n - 1; ++i) {
        addEdge(sortedByX[i], sortedByX[i + 1]);
        addEdge(sortedByY[i], sortedByY[i + 1]);
    }

    int shortestPath = dijkstra(nodes, 1, n);

    if (shortestPath == numeric_limits<int>::max()) {
        cout << -1 << endl;
    } else {
        cout << shortestPath << endl;
    }

    for (auto node : nodes) {
        delete node;
    }

    return 0;
}