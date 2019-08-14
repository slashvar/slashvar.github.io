#include <iostream>
#include <vector>

void increase(std::vector<int> v)
{
    for (size_t i = 0; i < v.size(); ++i) {
        v[i] += 1;
    }
}

int main()
{
    std::vector v = { 0, 1, 2 };
    increase(v);
    for (const auto& x : v) {
        std::cout << x << std::endl;
    }
    return 0;
}
