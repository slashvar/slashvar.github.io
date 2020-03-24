#include <iostream>
#include <type_traits>

// Define a detail namespace to indicate that the following code are implementation details
namespace detail {
// unsigned are never negative
template <typename T>
std::enable_if_t<std::is_unsigned_v<T>, bool> not_negative_implem(const T&, int)
{
    std::cout << "unsigned version\n";
    return true;
}

// if comparison against 0 works, it's ok
template <typename T>
auto not_negative_implem(const T& val, float) -> decltype(val >= 0)
{
    std::cout << "signed version\n";
    return val >= 0;
}
}

// Now the generic function
template <typename T>
bool not_negative(const T& val)
{
    return detail::not_negative_implem(val, 0);
}

// Does T provide a size member ?

// Fallback if T::size() is not defined
template <typename T, typename = std::void_t<>>
struct has_size
{
    static constexpr bool value = false;
};

// Specialization when T::size() is defined
template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>>
{
    static constexpr bool value = true;
    using type                  = decltype(std::declval<T>().size());
};

int main()
{
    unsigned short un  = 42;
    long           sig = -12;
    if (not_negative(un)) {
        std::cout << "not_negative is true\n";
    }
    if (not_negative(sig)) {
        std::cout << "not_negative is true\n";
    }
}
