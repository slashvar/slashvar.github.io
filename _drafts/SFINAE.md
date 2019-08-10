---
Title: Detecting functions existance with SFINAE
---

Have you ever heard of SFINAE ? If not, you're missing one interesting feature of C++. As for many
other meta-programming aspect, it seems to rely on a specific behaviour of template to perform very
interesting compile operations.

Of course, there's a lot of presentation about SFINAE and the STL provides some useful tools based
on it. I don't want to write another basic introduction to it, so I decided to peak a topic for
which I didn't found any example easily.

That's lead us to the beginning of the title of this post: *Detecting functions existance*. We often
have to provide replacement for library functions that are not available everywhere, depending of
the OS or a library version. I'll use as an example `quick_exit(3)` which is supposed to be part of
the ISO standard but is not available on MacOS, we had the issue recently and while we're not
running our software on Mac, a lot of members of my teams are using Mac laptop as dev machine, so we
need to be able to at least compile our code on it, but we don't need a full-blown replacement and
using `_exit` or `_Exit` will be enough in that case.

Of course, I wanted something that was not based on macros and I wanted it relying on the presence
of the function. Hence the SFINAE approach.

But first, let see what we're talking about.

## Substitution Failure Is Not An Error

SFINAE stands for **Substitution Failure Is Not An Error**. Does it helps ?

In short, the compiler will not produce errors in some substitution cases and just ignore the faulty
code. It can be surprising that such a behaviour makes any sense in the first place, but it's in
fact a very powerfull feature. If you think of it, sometimes you instanciate a template but not all
operations provided by it make sense with the provided parameters. As long as you don't use them,
you're fine. This silent failure is very convenient in a lot of cases. The basic usage of this is to
forbid generic operations on type for which it doesn't make sense. Let's take a simple example:

```cpp
template <typename T>
auto sum(T&& x, T&& y) -> decltype(x + y)
{
    return x + y;
}
```

In this example, the return type of the function is the type of `x + y`, if `T` does not support
operator `+`, the expression `x + y` is ill formed, and the templated function won't be defined for
it. It's as simple as that.

But we can also use it to provide specialiased version based on similar property.

```cpp
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
```

It's a bit out-of-the-box and not really meaningful, but it shows you what can be done. Note the
usage of an extra parameter (`int` or `double`), it's here to guide the overloading choice since
both version are valid when `T` support `+`, we instruct the compiler to prefer the first version
since `int` is a better match for `0` than `double`.

The header `<type_traits>` contains several useful tools to leverage SFINAE more easily like
`std::enable_if` and various tests on types, let's see another example:

```cpp
template <typename T, typename U>
auto compare(const T& x, const U& y)
    -> typename std::enable_if<std::is_integral<T>::value && std::is_same<U, T>::value, bool>::type
{
    return x < y;
}
```

It's pretty straigthforward, this comparison function only accept integral types (integers) and the
same type for both parameters. We can complete it with a specialization to do proper comparison
(like checking for signedness).

Let's go back to my original topic, now.

## Detecting functions

OK, so what I want now is to be able write a wrapper for `quick_exit(3)` using `_exit` if it's not
available. And guess what, it appears to be pretty straigthforward ! Just check if the function
exists using `decltype` and you're good.

We will be using the same `int` vs `float` tricks to prioritize the version using `quick_exit`, add
the required attributes (`[[noreturn]]`) and lock detail implementation in its own namespace.

```cpp
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
```

The name of your wrapper have to be non ambiguous, you can easily end with infinite recursion
otherwise.

Of course, havind a `#if __My_OS__` test looks simpler, but now you only rely on the existance of
the function, not the possible version you're aware of.

This can be very useful, people using OpenSSL may have suffer from their API breaks between various
versions, you can have a proper wrapper hidding it this way.

