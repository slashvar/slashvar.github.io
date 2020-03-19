namespace message {

namespace details {
void print(const std::string& str)
{
    std::cout << str;
}

void print(int x)
{
    std::cout << x;
}

void print(float x)
{
    std::cout << x;
}
};

template <typename... Args>
void print(Args&&... args)
{
    (details::print(std::forward<Args>(args)), ...);
    std::cout << std::endl;
}
};

namespace message {
struct constants
{
    constexpr static size_t maxSize = 140;
};
};
