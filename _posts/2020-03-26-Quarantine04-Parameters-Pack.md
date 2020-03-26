---
title: Quarantine post 04  - Parameters pack
---

Today in quarantine post I will talk about parameters pack. So, it's again a template subject, but I'll do it softly.

## Variadic templates

Parameters pack are the way to make template variadic, that is defining template with an open number of arguments. The syntax is sometimes a bit disturbing at first, but once you understand it, it could really useful.

So let's start with a naive and basic example, just to see the syntax in action:

```c++
template <typename ... Args>
void example(Args ... args)
{
    std::cout << sizeof...(args) << std::endl;
}
```

So, it applies to templates as I said, we start by defining a variadic list of type parameters `Args`. We can now define a list of parameters to our example function, `args`. We can do a lot of things with it, but just for the example we will use `sizeof...()` the version of the operator `sizeof()` to get the size of the parameter pack.

What we can do with `args` ? Almost anything we want, but we need to either have a uniform operation to perform or we should play with recursivity to decompose the parameters pack.

Let see an example using fold expressions, fold expressions are one way to use parameter packs. They basically let you apply uniformely an operation a bit in the same spirit of reduce or fold in functional language.

```c++
// Example from cppreference
template<typename ...Args>
void printer(Args&&... args) {
    (std::cout << ... << args) << '\n';
}
```

This function will print all its arguments and close the output with a new line. The syntax `(std::cout << ... << args)` (parentheses are part of it) expresses the unfolding of `args`, the expression is repeated with the content of the pack in place of the `...` by repeating the operator between each elements.

But let's have a look at the recursive approach.

## Recursive decomposition

Templates are in essence functional, and thus we must adopt functional idioms. We can play with parameter packs like we would with list in functional languages. We will decompose cases with at least one for the empty pack and one for the recursion.

As a matter of example, let's build `print` function similar to pyhton's `print` (a very simplified version). This function will takes arguments and print them separated by space with a new line at the end. A bit like the previous example, but in a more subtle way.

First, we need a case for the empty pack:

```c++
void print()
{
    std::cout << std::endl;
}
```

Next, as we don't want a useless space before the new line, we need a case for a lonely parameter:

```c++
template <typename T>
void print(const T& val)
{
    std::cout << val;
    print(); // we could have just print a new line here
}
```

And finally the recursive case:

```c++
template <typename T, typename... Args>
void print(const T& val, Args&&... args)
{
    std::cout << val << " ";
    print(args...);
}
```

Simple, isn't it ? We just split the pack into a head element and a rest, for the rest of the pack.

Let's put all that in a file and test it.

```c++
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
```

That's a pretty simple example, but I hope I gave you the taste of digging it further.

## That's all folks

OK, this is it for today's post (again, I've tried to stick to 30min, not counting the cat interruptions). Since the quarantine will probably continue for some weeks, I will probably have time to wrote more examples using parameter packs.
