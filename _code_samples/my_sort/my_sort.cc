// Algorithms (for sort)
#include <algorithm>
// Stream input and output
#include <iostream>
// Vector containers
#include <vector>

// The program entry point
int main()
{
    // A vector of strings
    std::vector<std::string> lines;

    // Read and store the lines
    for (std::string line; std::getline(std::cin, line);) {
        // the line is copied in the vector
        lines.push_back(line);
    }
    std::sort(lines.begin(), lines.end());
    // Print the lines
    for (const auto& line : lines) {
        std::cout << line << std::endl;
    }
}
