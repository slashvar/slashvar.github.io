#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main()
{
    fs::path path = "/a/b";
    std::cout << path << std::endl;

    auto path2 = path / "c" / "d";
    std::cout << path2 << std::endl;
}
