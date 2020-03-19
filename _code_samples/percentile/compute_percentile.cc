#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <unordered_map>
#include <vector>

template <typename Container>
void cumulative_sum(Container& vect)
{
    for (auto it = begin(vect); it != end(vect) - 1; ++it) {
        *(it + 1) += *it;
    }
}

template <typename Container>
decltype(auto) percentile_(Container& vect, size_t value)
{
    assert(value <= 100);
    size_t threshold = (vect.back() * value) / 100;
    auto   pos       = std::lower_bound(begin(vect), end(vect), threshold);
    return pos - begin(vect);
}

size_t percentile(std::unordered_map<size_t, size_t>& data, size_t value)
{
    std::vector<size_t> scores;
    for (const auto& [key, value] : data) {
        scores.emplace_back(key);
    }
    std::sort(begin(scores), end(scores));
    std::vector<size_t> counts;
    for (const auto& key : scores) {
        counts.push_back(data[key]);
    }
    cumulative_sum(counts);
    auto pos = percentile_(counts, value);
    return scores[pos];
}

std::unordered_map<size_t, size_t> make_data(const std::vector<size_t>& raw_scores)
{
    std::unordered_map<size_t, size_t> data;
    for (const auto& key : raw_scores) {
        data[key] += 1;
    }
    return data;
}

int main()
{
    std::vector<size_t> raw_scores = { 15, 20, 35, 40, 50, 90, 120, 220, 400, 450 };

    auto data = make_data(raw_scores);
    std::cout << percentile(data, 90) << std::endl;
}
