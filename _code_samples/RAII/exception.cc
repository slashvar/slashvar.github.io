#include <iostream>

struct Demo
{
    Demo() = default;
    ~Demo()
    {
        std::cout << "dtor called\n";
    }
};

struct Exn
{};

int main()
{
    try {
        Demo demo;
        throw Exn {};
    } catch (const Exn& e) {
    }
}
