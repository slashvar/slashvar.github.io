#include <iostream>

struct Demo
{
    // The constructor
    Demo()
    {
        std::cout << "calling ctor\n";
    }

    // The destructor
    ~Demo()
    {
        std::cout << "calling dtor\n";
    }
};

int main()
{
    Demo d;
}
