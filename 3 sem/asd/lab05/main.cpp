#include <unordered_map>
#include <queue>
#include <vector>
#include <iostream>
using namespace std;

struct Node {
    int index;
    int minimal_number;
    int size_of_inputs = 0;
    int size_of_outputs = 0;
    unordered_map<int, Node*> inputs;
    unordered_map<int, Node*> outputs;

    Node(int idx, int minimal_number) : index(idx), minimal_number(minimal_number) {}

    static Node* create(int idx, int minimal_number) {
        return new Node(idx, minimal_number);
    }
};

auto compare = [](Node* a, Node* b) {
    return a->minimal_number > b->minimal_number;
};
priority_queue<Node*, vector<Node*>, decltype(compare)> minQueue(compare);

void update(Node* temp) {
    for (auto& output : temp->outputs) {
        Node* target = output.second;
        target->inputs.erase(temp->index);
        target->size_of_inputs--;
        if (target->size_of_inputs == 0) {
            minQueue.push(target);
        }
    }
}

int main() {

    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int numVertices, numEdges, k;
    cin >> numVertices >> numEdges >> k;

    vector<Node*> nodes(numVertices + 1);
    for (int i = 1; i <= numVertices; ++i) {
        int minimal_number;
        cin >> minimal_number;
        nodes[i] = Node::create(i, minimal_number);
    }

    for (int i = 0; i < numEdges; ++i) {
        int a, b;
        cin >> a >> b;
        nodes[b]->outputs[a] = nodes[a];
        nodes[a]->inputs[b] = nodes[b];
        nodes[a]->size_of_inputs++;
        nodes[b]->size_of_outputs++;
    }

    int result = 0;
    int cur = 0;
    for (int i = 1; i < numVertices + 1; i++) {
        if (nodes[i]->size_of_inputs == 0) minQueue.push(nodes[i]);
    }
    while (cur < k) {
        Node* temp = minQueue.top();
        minQueue.pop();
        result = max(temp->minimal_number, result);
        update(temp);
        cur++;
    }
    cout << result;
    return 0;
}