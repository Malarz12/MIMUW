/* g++ @opcjeCpp main.cpp prev.cpp -o main.e*/

#include <cassert>
#include <vector>

#include "prev.h"

int main() {
    std::vector<int> seq = {6, 2, 9, 1, 4, 6};
    init(seq);
    assert(prevInRange(5, 7, 10) == 2);
    assert(prevInRange(5, 12, 14) == -1);
    assert(prevInRange(5, 6, 6) == 5);
    assert(prevInRange(0, 3, 7) == 0);
    pushBack(3);
    assert(prevInRange(5, 1, 3) == 3);
    assert(prevInRange(6, 1, 3) == 6);
    pushBack(6);
    done();
}
