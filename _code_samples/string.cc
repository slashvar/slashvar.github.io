#include <iostream>
#include <string>

int main()
{
    std::string foo = "";
    foo += std::to_string(12);
    std::cout << foo << std::endl;
}
