#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void my_ls(const fs::path& path)
{
    std::error_code ec;
    if (fs::is_regular_file(path, ec)) {
        std::cout << fs::relative(path, ec).string() << std::endl;
        return;
    }
    for (auto&& p : fs::directory_iterator(path, ec)) {
        std::cout << fs::relative(p.path(), ec).string() << std::endl;
    }
    if (ec) {
        std::cerr << ec.message() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    fs::path path {};
    if (argc < 2) {
        path = ".";
    } else {
        path = argv[1];
    }
    my_ls(path);
}
