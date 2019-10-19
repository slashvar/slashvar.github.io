#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

template <typename STREAM, typename CONTAINER>
decltype(auto) join(STREAM&          stream,
                    const CONTAINER& list,
                    const char*      sep  = " ",
                    const char*      endl = "\n")
{
    auto first = begin(list);
    auto last  = end(list);
    if (first != last) {
        std::copy(first, last - 1, std::ostream_iterator<decltype(*first)>(stream, sep));
        stream << *(last - 1);
    }
    return stream << endl;
}

int main()
{
    std::vector<int> v {};
    for (int i = 0; i < 10; ++i) {
        v.push_back(i);
    }
    std::copy(begin(v), end(v) - 1, std::ostream_iterator<int>(std::cout, ", "));
    std::cout << v.back() << std::endl;
    join(std::cout, v);
    return 0;
}
