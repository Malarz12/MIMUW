#include<vector>
#include<iostream>
#include<fstream>

int generate_random_number(int max, int min) {
    return min + rand() % (max - min + 1);
}
std::vector<int> generate_random_sequence() {
    std::vector<int> result;
    int size = generate_random_number(50, 100);
    result.push_back(size);
    int prev_distance = 0;
    for(int i = 0; i < size; i++) {
        result.push_back(generate_random_number(1, size));
        result.push_back(generate_random_number(prev_distance, prev_distance + 1000));
        prev_distance = result[result.size() - 1];
    }
    return result;
}
int main() {
    srand(time(NULL));

    std::vector<int> sequence = generate_random_sequence();
    std::ofstream file("input.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return 1;
    }

    // Zapisywanie liczb do pliku, oddzielonych spacjami
    for (size_t i = 0; i < sequence.size(); ++i) {
        file << sequence[i];
        if (i < sequence.size() - 1) {
            file << " "; // Dodaj spację między liczbami
        }
    }

    file.close(); // Zamknięcie pliku

    return 0;
}
