#include <array>
#include <charconv>
#include <iostream>
#include <limits>
#include <string_view>
#include <cstdio>

static void printNums()
{
    std::array<char, 24> buff;
    for (size_t value = 1; value <= std::numeric_limits<size_t>::max() / 2; value *= 2) {
        auto [p, err] = std::to_chars(buff.data(), buff.data() + buff.size(), value);
        std::cout << value << ": \"" << std::string_view(buff.data(), p - buff.data()) << '"' << std::endl;
    }
}

static void check()
{
    {
        std::array<char, 24> buff;
        auto n = std::snprintf(buff.data(), buff.size(), "%u", 1u << 31);
        std::cout << std::string_view(buff.data(), n) << " " << n << std::endl;
    }
    {
        std::array<char, 24> buff;
        auto [p, err] = std::to_chars(buff.data(), buff.data() + buff.size(), 1u << 31);
        std::cout << std::string_view(buff.data(), p - buff.data()) << " " << p - buff.data() << std::endl;
    }
}

int main()
{
    printNums();
    check();
}
