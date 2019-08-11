#include <iostream>
#include <type_traits>

template <typename T, typename U>
struct is_same_signedness
{
    constexpr static const bool value = std::is_signed<T>::value == std::is_signed<U>::value;
};

template <typename T, typename U>
auto my_less(const T& x, const U& y)
    -> std::enable_if_t<is_same_signedness<T, U>::value, decltype(x < y)>
{
    return x < y;
}

template <typename T, typename U>
auto my_less(const T& x, const U& y)
    -> std::enable_if_t<std::is_signed<T>::value && std::is_unsigned<U>::value, bool>
{
    using uT       = std::make_unsigned_t<T>;
    using target_t = typename std::conditional<sizeof(uT) < sizeof(U), U, uT>::type;
    return x < 0 || static_cast<target_t>(x) < static_cast<target_t>(y);
}

template <typename T, typename U>
auto my_less(const T& x, const U& y)
    -> std::enable_if_t<std::is_unsigned<T>::value && std::is_signed<U>::value, bool>
{
    using uU       = std::make_unsigned_t<U>;
    using target_t = typename std::conditional<sizeof(T) < sizeof(uU), uU, T>::type;
    return y > 0 && static_cast<target_t>(x) < static_cast<target_t>(y);
}

struct A
{};
struct B
{};

struct C
{};

bool operator<(const A&, const B&)
{
    return true;
}

int main()
{
    int      x = -1;
    unsigned y = 1;

    if (my_less(x, y)) {
        std::cout << "Good\n";
    } else {
        std::cout << "Bad\n";
    }

    if (my_less(y, x)) {
        std::cout << "Bad\n";
    } else {
        std::cout << "Good\n";
    }
    A a;
    B b;
    if (my_less(a, b)) {
        std::cout << "Good\n";
    } else {
        std::cout << "Bad\n";
    }
}
