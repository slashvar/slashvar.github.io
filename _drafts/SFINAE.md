---
Title: Detecting functions existence with SFINAE
---

Have you ever heard of SFINAE ? If not, you're missing one interesting feature of C++. As for many other meta-programming aspect, it seems to rely on a specific behavior of template to perform very interesting compile operations.

Of course, there's a lot of presentation about SFINAE and the STL provides some useful tools based on it. I don't want to write another basic introduction to it, so I decided to peak a topic for which I didn't found any example easily.

That's lead us to the beginning of the title of this post: *Detecting functions existence*. We often have to provide replacement for library functions that are not available everywhere, depending of the OS or a library version. I'll use as an example `quick_exit(3)` which is supposed to be part of the ISO standard but is not available on MacOS, we had the issue recently and while we're not running our software on Mac, a lot of members of my teams are using Mac laptop as dev machine, so we need to be able to at least compile our code on it, but we don't need a full-blown replacement and using `_exit` or `_Exit` will be enough in that case.

Of course, I wanted something that was not based on macros and I wanted it relying on the presence of the function. Hence the SFINAE approach.

But first, let see what we're talking about.

## Substitution Failure Is Not An Error

SFINAE stands for **Substitution Failure Is Not An Error**. Does it helps ?

In short, the compiler will not produce errors in some substitution cases and just ignore the faulty code. It can be surprising that such a behavior makes any sense in the first place, but it's in fact a very powerful feature. If you think of it, sometimes you instantiate a template but not all operations provided by it make sense with the provided parameters. As long as you don't use them, you're fine. This silent failure is very convenient in a lot of cases. The basic usage of this is to forbid generic operations on type for which it doesn't make sense. Let's take a simple example:

```cpp
template <typename T>
auto sum(T&& x, T&& y) -> decltype(x + y)
{
    return x + y;
}
```

In this example, the return type of the function is the type of `x + y`, if `T` does not support operator `+`, the expression `x + y` is ill formed, and the templated function won't be defined for it. It's as simple as that.

But we can also use it to provide specialized version based on similar property.

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

It's a bit out-of-the-box and not really meaningful, but it shows you what can be done. Note the usage of an extra parameter (`int` or `double`), it's here to guide the overloading choice since both version are valid when `T` support `+`, we instruct the compiler to prefer the first version since `int` is a better match for `0` than `double`.

The header `<type_traits>` contains several useful tools to leverage SFINAE more easily like `std::enable_if` and various tests on types.

## Detecting functions

OK, so what I want now is to be able write a wrapper for `quick_exit(3)` using `_exit` if it's not available. And guess what, it appears to be pretty straightforward ! Just check if the function exists using `decltype` and you're good.

We will be using the same `int` vs `float` tricks to prioritize the version using `quick_exit`, add the required attributes (`[[noreturn]]`) and lock detail implementation in its own namespace.

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

The name of your wrapper have to be non ambiguous, you can easily end with infinite recursion otherwise.

Of course, having a `#if __My_OS__` test looks simpler, but now you only rely on the existence of the function, not the possible versions you're aware of.

This can be very useful, people using OpenSSL may have suffer from their API breaks between various versions, you can have a proper wrapper hiding it this way.

## Going further

There's a lot of possibilities when playing around with SFINAE and type traits in general. For example, we are using a wrapper template that injects if needed bound checking when passing unsigned sizes with proper types to function taking `int` as size (think of OpenSSL broken API).

As an example, let's build a `less` function that compare two integer of different types properly.  The promotion of integers type can go pretty wrong when comparing a signed integer with an unsigned one, just as a matter of example, try the following example:

```cpp
int main()
{
    int x = -1;
    unsigned y = 1;
    if (x < y) {
        std::cout << "Good\n";
    } else {
        std::cout << "Bad\n";
    }
}
```

Guess what ? It displays `Bad` (and you got a warning at compile time). The reason ? when comparing two different types, the compiler is forced to inject conversions, in this case, the rules state it must convert the `int` (which has a lower rank than `unsigned`) into `unsigned`. Despite the fact that as of today, nothing force the compiler to use two's complement, the conversion rules from signed to unsigned generate the same results, and `-1` becomes the maximum value for `unsigned`.

So, what we want is a generic function that returns the comparison safely. The comparison of `x < y` depends on signedness of `x` and `y`, assuming that `x` is signed but `y` is not, the comparison should be:

```cpp
    (x < 0) || static_cast<unsigned>(x) < y;
```

In the opposite case (`y` signed) we can build a similar test.

So, what do we need ?

* A version that return the comparison directly the two parameter have the same signedness and the comparison is defined for their type.
* A version when the first argument is signed but not the second
* A version when the second is signed

As a side effect, non numeric types have the same signedness (provided that we test it correctly) and thus the first version will be defined as long as we can compare the two types.

Here is the implementation

```cpp
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
```

We first define a type traits to test for same signedness (we could avoid it, but the code looks better that way) and we use it to construct the version that work for types with same signedness and defined comparison.

Then we have the two version depending on signedness of parameters. We use SFINAE to select the correct version and use other construction provided in `<type_traits>` to properly promote both side to the bigger unsigned type.

The test for same signedness is mandatory since (unfortunately) `x < y` is not ill-formed when `x` and `y` are numeric types even if they have different signedness, thus we need to ensure that this version is not defined in this case.

We could have used `if constexpr` and merge in a single specialization the second and third one, but I find this presentation more explicit. In the first specialization, we could also avoid the `decltype` in the return type and use a `static_assert` providing a more explicit error message.

While you may find this definition a bit complex, if you need safe comparison between signed and unsigned types, this kind of construction provides a reusable code and helps avoid repeating the testing pattern. And since it supports all comparable types, you can use it in templates and all form of generic code safely. Bonus, once instantiated, the code is sufficiently simple to be elected for inlining and you can expect it to be as efficient as writing the test directly.

## Conclusion

In this post, I've just scratched the surface of what can be done with SFINAE. I think this illustrate one of the most interesting aspect of C++ and modern C++: providing smart tools aimed at increasing abstraction with almost no impact at run-time, also called zero-cost abstraction.

The growing availability of `constexpr` constructions (evaluation at compile time of standard expression of the language) and the increasing accessibility of SFINAE open new horizons and the hope for a better language without boilerplate code, dangerous usage of preprocessor or the need of code generators.
