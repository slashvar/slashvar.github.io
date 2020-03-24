---
title: Quarantine Post 02 - Quick SFINAE
---

Today, I'm jumping into more a advanced topic: Substitution Failure Is Not An Error (SFINAE). So, we're gonna talk about templates, type traits and a bit of static programming.

## SFINAE: What's that ?

The first keyword is *Substitution*, so it's all about template. Substitution occurs when we instantiate a template and a variable of the template is replaced by it's actual _value_. The point is that unlike most of what may happen at compile time, if substitution fails for a template (or a part of it), it does not generate an error at compile.

SFINAE is a trick that became a standard. We can use the SFINAE to select a specific implementation of a template without relying on explicit specialization.

Let's start with an example. We want to implement a function that tells if a number is not negative, if we want it to work on all kind of numbers, without SFINAE we need to write a specialization for all types or write a blind template that accept any types. Let's go directly in jungle:

```c++
#include <type_traits>

// unsigned are never negative
template <typename T>
std::enable_if_t<std::is_unsigned_v<T>, bool>  not_negative(const T& val)
{
    return true;
}

// if comparison against 0 works, it's ok
template <typename T>
auto not_negative(const T& val) -> decltype(val >= 0)
{
    return val >= 0;
}
```

This is not yet functional, but first let's explore the code. The first version uses `std::enable_if_t<cond,type>` an SFINAE tools that is replaced by `type` if `cond` is true and not defined otherwise. So, if `std::is_unsigned_v<T>` is true (another SFINAE tool) this function is defined and always return true.

The second instance uses `decltype`, a static operator that evaluate the type of an expression, so here if `val >= 0` is well typed, the function will have the same return type, so this function will be defined for any type `T` that can be used in comparison.

As I say, this is almost working. The problem is that if I try to instantiate that template with a type like `unsigned int`, both versions will be defined, we need something else. We can do something similar as the first one to exclude unsigned types, or we can use overloading priority to select the right function. Let's see how to do that:

```c++
// Define a detail namespace to indicate that the following code are implementation details
namespace detail
{
// unsigned are never negative
template <typename T>
std::enable_if_t<std::is_unsigned_v<T>, bool>  not_negative_implem(const T& val, int)
{
    return true;
}

// if comparison against 0 works, it's ok
template <typename T>
auto not_negative_implem(const T& val, float) -> decltype(val >= 0)
{
    return val >= 0;
}
}

// Now the generic function
template <typename T>
bool not_negative(const T& val)
{
    return detail::not_negative_implem(val, 0);
}
```

Here we are using the fact that when selecting an overloaded verison, the compiler will select a version without type conversion first. So, if `T` is an unsigned type, since `0` is by default an `int`, the first overload will be selected, otherwise this version won't be defined and it will try to instantiate the second.

The following code will demonstrate that code in action. I have added some print statement to see which version is used:

```c++
#include <iostream>
#include <type_traits>

// Define a detail namespace to indicate that the following code are implementation details
namespace detail
{
// unsigned are never negative
template <typename T>
std::enable_if_t<std::is_unsigned_v<T>, bool>  not_negative_implem(const T&, int)
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

int main()
{
    unsigned short un = 42;
    long sig = -12;
    if (not_negative(un)) {
        std::cout << "not_negative is true\n";
    }
    if (not_negative(sig)) {
        std::cout << "not_negative is true\n";
    }
}
```

Compile and run:

```
shell> clang++ -Wall -Wextra -std=c++17 -o sfinae01 sfinae01.cc
shell> ./sfinae01
unsigned version
not_negative is true
signed version
```

We can do lot more with this, I have already wrote a post on SFINAE: https://slashvar.github.io/2019/08/17/Detecting-Functions-existence-with-SFINAE.html

## Defining type traits

The standard provide a lot of type traits, but we can define our own using SFINAE too. Here is a quick example, a type traits that evaluate to true if instances of a type provide a function member `size()`. And as a bonus, it can give you the return type of this function.

```c++
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
```

As I have reach the time limit for this post, I will let you explore that code by yourself !

## That's all for today

This is the end of this quarantine post, if you didn't know this topic before, you probably have something new to explore.
