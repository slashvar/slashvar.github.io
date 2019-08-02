---
Title: Let's play with C++
---

C++ is full of exciting aspects. When it comes to meta-programming, one can truly enjoy the trip. I was (re)reading the excellent book *Modern C++ Design: Generic Programming and Design Patterns Applied* by Andrei Alexandrescu, and it triggered many ideas.

I'll focus on a tiny piece of code I wrote to verify a concept. After reading the chapter on singletons, I thought of a missing approach to the KDL (Keyboard, Display, Log) issue. This book exposes a classic issue: we have two singletons handling resources (`K` and `D`) and a singleton handling logs (`Log`). We want to avoid constructing the `Log` singleton when we don't use it, but we want it to be available at any moment, and for it to be properly destroyed when we leave.

The book offers various solutions, such as the Phoenix Singleton (we rebuild the object when needed), or longevity management (an excellent idea for sure). However, what if we could partially construct the `Log` instance early on, only handling the cheap part, and keep the expensive job only when needed? We can perform an early construction by using a value instance rather than a pointer, which is safe as long as no other singleton of the same kind (value instance) refers to it.

Of course, this should be generic. What I want is a generic object that can postpone the creation of another object and provide a kind of proxy to it. Let's call this object `Defer_constructor`.

**Disclaimer:** this is purely experimental. I didn't explore all the possible implications and risks of such a construction. **Don't use my code in production.**

## `Defer_constructor` is a kind of smart pointer!

Deferring execution sounds like command/functor, but the important part is the proxy idea. Outside of the object, the fact that we construct the inner object on-demand is hidden (and somehow irrelevant).

We're only building a POC, so I chose a simple model for my smart pointer, no reference counting or thread-safety. I'm sticking to a straightforward concept that only supports move semantics and provides object creation.

The starting point of my class looks like this:

```c++
template <typename DeferedType>
class Defer_constructor
{
public:
    using type = std::add_pointer_t<DeferedType>;

    Defer_constructor() = default;

    Defer_constructor(const Defer_constructor&) = delete;
    Defer_constructor& operator=(const Defer_constructor&) = delete;

    Defer_constructor(Defer_constructor&&) = default;
    Defer_constructor& operator=(Defer_constructor&&) = default;

    ~Defer_constructor()
    {
        if (instance_ != nullptr) {
            delete instance_;
        }
    }

    type operator*();

    type operator->();

private:
    type instance_ = nullptr;
};
```

We're supporting default move semantics (constructible and assignable), we explicitly forbid copy, and we store an instance pointer. Even if we didn't provide implementations yet, we have the
expected overloading of `*` and `->`.

Note the use of `std::add_pointer_t<DeferedType>`. We could have written `DeferedType*`, but we have a type trait handling this. It may solve some obscure corner cases.

## Constructing the instance

We now need to provide a way of constructing the instance and be prepared to support any form of constructors. That's when I realized how significant the changes that happened during the last decade are. In
Alexandrescu's book (back from 2001), there were no parameters pack, thus no variadic templates, no tuples, no lambdas. We have all this now.

We need:

* a way of accepting any parameters for the instance's constructor,
* a way to store them and pass them later on.

Fortunately, we don't need to support multiple ways of constructing the instance, since we accept these parameters at creation time. We, therefore, know their types statically.

Here is the updated skeleton to support all parameters for our instance. The choice we're making here may not be optimal (we'll see why later on), but it works:

```c++
template <typename DeferedType, typename... Args>
class Defer_constructor
{
public:
    Defer_constructor(Args&&... args);
};
```

We want to be able to accept an unknown number of arguments. For this, we can use parameters pack (and `&&` for perfect forwarding). Nothing fancy. Yet, how do we store that? We can't have an attribute holding `Args... args`, do we?

Another recent C++ addition can save us: tuples! We can store the parameters in a tuple, and later find a way to pass it to the constructor.

You may be thinking, *"why not use `std::bind` ?"*. That was my first idea, but you can't bind a constructor. While searching for a solution, I ended up finding some template related to tuples and thought it could be
helpful. However, we could definitely have stored a bind to a function that creates an instance for us. You can give it a try!

Back to the tuple, here is the first version that stores data:

```c++
template <typename DeferedType, typename... Args>
class Defer_constructor
{
public:
    Defer_constructor(Args&&... args) : _args(std::forward<Args>(args)...) {}
private:
    std::tuple<Args...> _args;
};
```

Looking good? Yes, we're probably missing something. What happens if one of the parameters is a reference? Even worse, what happens if we get a reference to an object with a shorter lifetime? Bingo, a dangling reference!

Here, like with any other form of closure, we need to store copies of parameters. That's a choice; we could also accept dangling references as a risk. However, the necessary code change to store copies is
rather small, so let's go for it.

```c++
template <typename DeferedType, typename... Args>
class Defer_constructor
{
public:
    Defer_constructor(Args&&... args) : _args(std::forward<Args>(args)...) {}
private:
    std::tuple<typename std::decay<Args>::type...> _args;
};
```

## When we finally construct the instance

The previous section title was misleading: we only store parameters, but we didn't provide a way to construct the instance.

How can we do this? We need a way to go back to an expression of the form.

```c++
    new DeferedType(std::forward<Args>(a)...)
```

Where `a` is a parameters pack.

Remember when I said I've ended up using a tuple thanks to some templates I found useful? Here we are: we have something called `std::apply` that's made for this. It takes a callable object and tuple,
and calls the object with elements of the tuple as parameters.

I've decided to wrap the `new` in a lambda, but we can probably pass the `new` operator of the class directly.

Another detail: we want to create the instance only it's used. It's used only through `operator*` and `operator->`, and the way they work is so similar that it makes sense to abstract the code in a private member (let's call it `acces_`).

```c++
template <typename DeferedType, typename... Args>
class Defer_constructor
{
public:

    type operator*()
    {
        return acces_();
    }

    type operator->()
    {
        return acces_();
    }

private:

    type acces_()
    {
        if (instance_ == nullptr) {
            instance_ = std::apply(
                [](Args... a) { return new DeferedType(std::forward<Args>(a)...); }, _args);
        }
        return instance_;
    }
};
```

This is a classic singleton pattern with lazy instantiation: if the instance doesn't yet exist, we create it. Otherwise, we only return the pointer.

## Putting everything together and testing

Here is a full example with the complete `Defer_constructor` class, two toy classes for the demo, and a main.

```c++
#include <iostream>
#include <tuple>
#include <type_traits>

template <typename DeferedType, typename... Args>
class Defer_constructor
{
public:
    using type = std::add_pointer_t<DeferedType>;

    Defer_constructor() = default;

    Defer_constructor(const Defer_constructor&) = delete;
    Defer_constructor& operator=(const Defer_constructor&) = delete;

    Defer_constructor(Defer_constructor&&) = default;
    Defer_constructor& operator=(Defer_constructor&&) = default;

    ~Defer_constructor()
    {
        if (instance_ != nullptr) {
            delete instance_;
        }
    }

    Defer_constructor(Args&&... args) : _args(std::forward<Args>(args)...) {}

    type operator*()
    {
        return acces_();
    }

    type operator->()
    {
        return acces_();
    }

private:
    type                                           instance_ = nullptr;
    std::tuple<typename std::decay<Args>::type...> _args;

    type acces_()
    {
        if (instance_ == nullptr) {
            instance_ = std::apply(
                [](Args... a) { return new DeferedType(std::forward<Args>(a)...); }, _args);
        }
        return instance_;
    }
};

struct Foo
{
    Foo(int x, int y) : x_(x), y_(y)
    {
        std::cout << "Foo(" << x_ << ", " << y_ << ")\n";
    }

    int x_, y_;
};

struct Bar
{
    Bar(const std::string& str)
    {
        std::cout << str << std::endl;
    }
    void f()
    {
        std::cout << "f\n";
    }
};

int main()
{
    {
        std::cout << "Before creation of f2\n";
        Defer_constructor<Foo, int, int> f2(2, 2);
        std::cout << "After creation of f2\n";
        int& x2 = f2->x_;
        std::cout << "After use of f2 internal instance\n";
        std::cout << x2 << std::endl;
    }
    std::cout << "Now with the risk of dangling ref\n";
    Defer_constructor<Bar, std::string&>* later;
    {
        std::string str("Hello bar");
        later = new Defer_constructor<Bar, std::string&>(str);
    }
    (*later)->f();
    delete later;
    return 0;
}
```

Let's try it now! I'm confident, so I'll use `clang` with `-fsanitize=address` and `-Weverything`!

```sh
> clang++ -fsanitize=address -Weverything -Wno-c++98-compat -std=c++17 -O3 -o defer defer.cc
> ./defer
Before creation of f2
After creation of f2
Foo(2, 2)
After use of f2 internal instance
2
Now with the risk of dangling ref
Hello bar
f
>
```

Note that we need `-Wno-c++98-compat` since `-Weverything` really contains everything.

Do you remember when I said that the way we get the parameters types as template parameters was not perfect? Here we're forced to write `Defer_constructor<Foo, int, int>` because `Foo` can't be inferred from the call. Therefore, we must provide all parameters.

I kept it for future improvement.

## Conclusion

First, it wasn't that hard. Once we have the key ideas (tuple and `std::apply`), everything else follows smoothly.

Of course, there's room for improvement. I am not at all confident about this code. I didn't checked the generated assembly code, but I guess we can expect much inlining.

## Bonus

I have one more little C++ stuff. It's completely different, but since I am rather slow at pushing new posts, and this *pattern* is small, I wanted to share.

You may have noted the arrival of the `[[nodiscard]]` attribute in C++17. This is an advantageous one (in fact, most compilers offered a similar one before) when you have functions returning values that shouldn't be discarded (e.g., a boolean that indicates an error).

Neat, but, as usual, we sometimes need to skip those return values for good. The problem isn't to find a way of discarding it, but to do it in a way that's explicit enough so that you can check it when reading the code, or even *grep* it.

Here is my solution:

```c++
template <typename T>
constexpr void ignore_value(T&&)
{}
```

I did not create it. For instance, in OCaml, the standard library provides a similar `ignore` function. I found it quite handy in this particular context, and since it's explicit and easy to use, you no longer have to decipher `void` casts or empty `if` statement during code reviews.

Oh, and it also works when you need to ignore variables or parameters.

Another way to do this is to assign the result to `std::ignore`, but I find it less explicit.
