#include <iostream>
#include <type_traits>

#include <cstdlib>
#include <unistd.h>

template <typename T>
auto sum(T&& x, T&& y) -> decltype(x + y)
{
    return x + y;
}

namespace details {
template <typename T>
auto print_sum_implem(const T& x, const T& y, int) -> decltype(x + y)
{
    auto res = x + y;
    std::cout << "Sum of " << x << " and " << y << " = " << res << std::endl;
    return res;
}

template <typename T>
int print_sum_implem(const T&, const T&, double)
{
    std::cout << "Sum is not possible !\n";
    return 0;
}

}

template <typename T>
auto print_sum(const T& x, const T& y)
{
    return details::print_sum_implem(x, y, 0);
}

struct Foo
{};

template <typename T, typename U>
auto compare(const T& x, const U& y) ->
    typename std::enable_if<std::is_integral<T>::value && std::is_same<U, T>::value,
                            bool>::type
{
    return x < y;
}

template <typename T, typename U>
auto compare(const T& x, const U& y) ->
    typename std::enable_if<std::is_integral<T>::value && std::is_convertible<U, T>::value,
                            bool>::type
{
    std::clog << "using conversion" << std::endl;
    return x < static_cast<T>(y);
}

namespace details {
template <typename T>
[[noreturn]] auto quick_exit_impl(T status, int) -> decltype(quick_exit(status))
{
    quick_exit(status);
}

template <typename T>
[[noreturn]] auto quick_exit_impl(T status, double) -> void
{
    _exit(status);
}
}

[[noreturn]] static void my_quick_exit(int status) noexcept
{
    details::quick_exit_impl(status, 0);
}

int main()
{
    print_sum(1, 2);
    print_sum(Foo {}, Foo {});
    int  x = 10;
    char y = 15;
    std::cout << compare(x, y) << std::endl;
    my_quick_exit(0);
}
