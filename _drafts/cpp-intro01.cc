#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    std::vector<std::string> lines;
    for (std::string line; std::getline(std::cin, line); ) {
        lines.push_back(line);
    }
    std::sort(lines.begin(), lines.end());
    for (const auto& line : lines) {
        std::cout << line << std::endl;
    }
}
