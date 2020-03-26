#include <iostream>
#include <random>

void print()
{
    std::cout << std::endl;
}

template <typename T>
void print(const T& val)
{
    std::cout << val << std::endl;
}

template <typename T, typename... Args>
void print(const T& val, Args&&... args)
{
    std::cout << val << " ";
    print(args...);
}

unsigned int_sqrt(unsigned n)
{
    unsigned r = n;
    while (n / r < r) {
        r = (r + n / r) / 2;
    }
    return r;
}

int main()
{
    std::string str("World");
    print("Hello,", str, "!");

    std::random_device              rd;
    std::mt19937                    gen(rd());
    std::uniform_int_distribution<> dis(27972, 755244);

    unsigned x = dis(gen);

    print("int_sqrt(", x, ") =", int_sqrt(x));
}
