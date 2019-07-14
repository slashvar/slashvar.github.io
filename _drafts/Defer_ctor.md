Let's play with C++
===================

C++ is full of interesting aspects, and when it comes to meta-programming, one can really enjoy the
trip. I was (re)reading excellent book *Modern C++ Design: Generic Programming and Design Patterns
Applied* by Andrei Alexandrescu, and triggers a lot of ideas.

I'll focus on tiny piece of code I wrote to verify a concept. After reading the chapter on
singleton, I thought of a missing approach to the issue of KDL (Keyboard, Display, Log). This
book exposes a classic issue: we have two singletons handling resources (K and D) and a singleton
handling logs (`Log`), we want to avoid constructing the Log singleton when we don't use it, but we
want it to available at any moment and properly destroyed when we leave.

The book offers various solutions like the Phoenix Singleton (guess what, we rebuild the object when
        needed) or management of longevity (a pretty good idea for sure). But, what if we can
construct partially the Log instance very early with only the cheap part of the construction and
only do the expensive job when needed ? Early construction can be performed by using a value
instance rather than a pointer and is safe as long as no other singleton of the same kind (value
        instance) refers to it.

Of course, this has to be a bit generic and what I want is a generic object that is able to
postponed the creation of another object a provide a form of proxy to it. Let's call this object
`Defer_constructor`.

**Disclaimer:**: all this is purely experimental and I didn't explore all the implications and all
the risks of such a construction. So, don't use my code in production.

## `Defer_constructor` is a kind of smart pointer !

Deferring execution sounds like command/functor, but the important part is the proxy idea, outside
of our object the fact that the inner object is constructed on-demand if hidden (and some how
        irrelevant).

It's only a POC, so I choose a simple model for my smart pointer, no ref-counting and thread safety.
I will stick to a very simple concept supporting only move-semantic and providing object creation
directly.

The starting point of my class look like this:

```
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

So, we support default move semantics (constructible and assignable), we explicitely forbid copy and
we store an instance pointer, and even if we haven't provide their implementation yet, we have the
expeceted overloading of `*` and `->`.

Note the use of `std::add_pointer_t<DeferedType>`, we could have simply write `DeferedType*`, but
there's a type traits doing that, if needed it may solve some obscure corner cases.

## Constructing the instance

We now need to provide a way of constructing the instance, and of course be prepared to support any
form of constructor. This where I realized how the changes on the last decade are so important. In
Alexandrescu's book (remember, it's book of 2001) there was no parameters pack and thus no variadic
templates, no tuples, no lambdas ... But we're in 2019 and we have all of this now.

What we need are:

* a way of accepting any parameters for the constructor of the instance
* a way to store them and pass them later to this constructor.

Hopefully for us we don't need to support multiple way constructing the instance since we will
accept these parameters at creation time and thus know their types statically.

So, here is the squeleton updated to support all parameters for our instance, the choice made here
may not be the best at hand (we will see later why) but it works (only the important additions):

```
template <typename DeferedType, typename... Args>
class Defer_constructor
{
public:
    Defer_constructor(Args&&... args);
};
```

So, we want an undefined number of arguments, we just use parameters pack (and `&&` for perfect
        forwarding). Nothing fancy.

But how do we store that ? we can't have an attribute holding `Args... args`, do we ?

Anthore recent addition will rescue us, tuples ! We can store that in a tuple, we will later find a
way to pass it to the constructor.

I hear someone say "why not use `std::bind` ?", yes, that was my first idea, but you can't bind a
constructor, and searching for a solution I ended on some template related to tuples that could be
helpful. But yes, we could have store a bind to a function that creates the instance for us, you
have your subject now !

Back to my tuple, here is the first version storing data:


```
template <typename DeferedType, typename... Args>
class Defer_constructor
{
public:
    Defer_constructor(Args&&... args) : _args(std::forward<Args>(args)...) {}
private:
    std::tuple<Args...> _args;
};
```

Looks good ? Yes, we probably miss something. What happen if one of the parameters is a reference ?
And even worce, what happen if we get a reference to an object with a shorter lifetime ? Bingo, a
dangling reference !

Here, just like any other form of closure, we need to store copies of parameters. This is a choice,
we could also accept dangling references as a risk. But the code change to store copies is
rather small, so let's go for it.


```
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

The title of the previous section was a bit misleading as we did not provide a way of constructing
the instance, just a way of storing parameters.

So how do we construct it ? We need a way to go back to an expression of the form:

```
    new DeferedType(std::forward<Args>(a)...)
```

Where `a` is again a parameters pack.

I've told you that I've ended with tuple due to some templates that I found usefull ? Here we are,
we have something called `std::apply` made especially for this. It takes a callable object and tuple
and call the object with elements of the tuple as parameters.

I've decided to wrap the `new` in a lambda, but it's probably possible to directly pass the operator
`new` of the class.

Another detail: we are supposed to create the instance as long as it used, and it's used only
through `operator*` and `operator->`, and the way they work is so similar that it makes sens to
abstract the code in a private member (let's call it `acces_`). Here is the implementation:

```
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

You will recognize the classic pattern of a singleton with lazy instantiation: if the instance is
not yet constructed, we create it, otherwise we just return the pointer.

## Putting everything together and testing

OK, here is a full listing with the complete class `Defer_constructor`, two toy classes for the demo
and a main.

```
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

And let's try it, I'm confident so I'll use `clang` with `-fsanitize=address` and `-Weverything` !

```
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

Note `-Wno-c++98-compat` since `-Weverything` contains really everything.

Do you rember, I said that the way we get the parameters types as template parameters was not
perferct ? Here we're forced to write `Defer_constructor<Foo, int, int>` since `Foo` can not be
infered from the call and thus we must all parameters. I let this for a further improvement.

## Conclusion

First it wasn't that hard, once we had the key ideas (tuple and `std::apply`) the rest follows
smoothly. There's room for improvement and I am not confident at all about this code.

I did not checked the generated code yet, but I guess that we can expect a lot of inlining.

## Bonus

I have one more little C++ stuff, it's completely different, but since I am rather slow to push new
articles and this little *pattern* is small, I wanted to show it you.

You may have noted the arrival of the `[[nodiscard]]` attribute in C++17. This is a very usefull one
(in fact most compilers offered a similar one before) when you have function returned values that
must not be discarded (think of a boolean indicating an error for example).

This is cool, but, as usual, we sometimes need to skip those return values for good. The problem is
not to find a way of discarding it, but have a very explicit way that can't be checked when reading
code or even better can be *grepped*.

Here is my little solution:

```
template <typename T>
constexpr void ignore_value(T&&)
{}
```

I did not invent it, in OCaml for example, the standard library provide a similar `ignore` function.
But I found it very handy in this particular context. And since it's explicit and easy to use, you
will no longer have to decipher `void` cast or empty `if` during code review. Oh, and it work also
if you need to ignore variables or parameters.

Another way to do this, is to assign the result to `std::ignore`, but I found it less explicit.
