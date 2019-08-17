#include <algorithm>
#include <iostream>
#include <vector>

template <typename T>
T iter_max(T a, T b)
{
    if (*a < *b) {
        return b;
    }
    return a;
}

template <typename T>
T iter_min(T a, T b)
{
    if (*a < *b) {
        return a;
    }
    return b;
}

template <typename T>
T choose_pivot(T begin, T end)
{
    auto mid  = begin + (end - begin) / 2;
    auto last = end - 1;
    auto a    = iter_min(begin, mid);
    auto b    = iter_min(mid, last);
    auto c    = iter_min(begin, last);
    return iter_max(iter_max(a, b), c);
}

template <typename T>
T partition(T begin, T end)
{
    auto pivot = choose_pivot(begin, end);
    auto pval  = *pivot;
    std::iter_swap(pivot, end - 1);
    pivot = begin;
    for (auto it = begin; it != end - 1; ++it) {
        if (*it < pval) {
            std::iter_swap(it, pivot);
            ++pivot;
        }
    }
    std::iter_swap(pivot, end - 1);
    return pivot;
}

template <typename T>
void my_sort(T begin, T end)
{
    if (begin < end) {
        auto pivot = partition(begin, end);
        my_sort(begin, pivot);
        my_sort(pivot + 1, end);
    }
}

int main()
{
    std::vector<int> v { { 5, 4, 3, 2, 1, 0 } };
    my_sort(v.begin(), v.end());
    for (const auto& x : v) {
        std::cout << x << std::endl;
    }
}
