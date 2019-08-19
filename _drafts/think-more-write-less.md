---
Title: Think more, write less
---
In this post I want to explore a bit more various ways of organizing code. I'll stick to C++ in most example, but some idea could be reuse in a lot programming languages.

An important aspect of good software engineering is to be able to ship features early. For example, at Algolia, we deploy new release of the core search engine every week. This has several benefits, we can measure impacts of changes very early and decide to roll-back or continue based on the observed results. For new features, we can live test early versions and verify their usability with groups of selected users before moving on.

This form of regular releases is good to avoid tunnel effect or to correct a wrong design early.

The main difficulty is to be able to ship a usable feature in a short periode of time without accumulating (too much) technical debt. Of course, theoretically, you're supposed to have multiple iterations and thus clean the code as much as possible before it is to late. Honestly, I never see it happen unless a strong constraint shows up.

The idea is to find quick and easy ways to write good quality flexible code.

Sounds a bit utopic ? Yes and no. Of course, good design and good code don't come easy, they require work and iterations, we're not chasing dragons, we want pragmatic solutions. For me, the trick lies in the difference between anticipating the future (irrealistic) against designing for evolution.

Don't expect magical solutions or ground breaking methods, I just want to show some of the coding practives I used when I want to write flexible code.

## The good side of Object Oriented Programmatic

When I was a CS student in the end of 1990's, OOP was the ultimate software engineering technics. OOP was often presented through design approach based on inheritance and class relationships. While some good results can come through this, inheritance often induces rigid designs or complex and irrealistic architectures.

Does it mean that OOP is not good choice ? I've seen a lot of articles arguing that OOP is a bad idea or fails at providing a good framework for propoer software developpement, often advocating for some alternative paradigms and thus probably missing the point. I'm pragmatic when it comes to languages' features, if it works, it works. And objects work, there're some pitfalls, but OOP is not that bad finally.

My take is that inheritance based polymorphism (with or without interfaces) is mostly useless. Generics, templates or ML polymorphism are way better for genericity and much more flexible. Once you accepted that you can have a fresh new look at inheritance and use it in more efficient way.

Good OOP texts, often advocate for composition and delegation against traditional inheritance. The issue with composition, and even more with delegation, is the amount of empty code you need to write. By empty code, I mean all those small functions that just serve to delegate the call to some inner object.

Putting all that together doesn't seem to lead us to a solution does it ? Somehow, yes.

## Mixin through template and inheritance

Mixin is often described as a replacement to inheritance where rather than deriving from another class, the class is included. C++ is rarely described as a language provding mixin, but in fact, the way inheritance works is purely a form of mixin. Combined with templates, we can build an interesting model where our code is flexible and yet simple.

But enough blabla, let's go for a concrete example. Say we want to build a simple log class providing a uniform formatting of logs for our app. The main responsibility of the this class will be to format the logs as expected and add some extra info (date, process id ... )

For first version, we know that we only need to send log to the standard error output. But, it makes sense that later, we should be able to send logs to something else like a file or an external service.

How to implement the simple version and get ready for future evolution ? Here is a possible approach, first we design our log class this way:

```C++
template<typename T>
struct Logger : public T
{
    void print(const std::string& msg)
    {
        auto ts = std::chrono::system_clock::now().time_since_epoch();
        auto out = std::to_string(ts) + ":" + std::to_string(getpid()) + ":" + msg;
        this->write(out);
    }
};
```

OK, the `print` function is pretty simple, it gets the current timestamp, the pid and put all that with the provided message in a string separated by colons. The it calls an apprently undefined method `write`.

The only way to get this method is by inheritance (since it's not declared nor defined in the class), and we're inheriting from some class `T`. This class is also a template parameter, and thus we know nothing about it. This is where we introduce our form of mixin. The `Logger` class is parametrized by an external class (`T`) providing the `write` method.

Here is my complete `logger.h` header defining my logging facility with a possible backend. I will just rename the parameter `T` with a more adapted name (`Backend`) and add a namespace for clarity.

```C++
#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <unistd.h>

namespace Logger {
template<typename Backend>
struct base : public Backend
{
    void print(const std::string& msg)
    {
        auto ts = std::chrono::system_clock::now().time_since_epoch().count();
        auto out = std::to_string(ts) + ":" + std::to_string(getpid()) + ":" + msg;
        this->write(out);
    }
};

struct std_err_backend
{
    void write(const std::string& str)
    {
        std::clog << str << std::endl;
    }
};

using ErrOut = base<std_err_backend>;

};
```

I let you read the code, it's pretty straightforward.

If you need to extend your implementation and send log to some redis service or print them in a file, all you need is to implement a `redis_backend` or `file_backend`. Of course, the example is a bit short, we need to properly add constructors and destructor, eventually consider isolating some aspects through visibility ...

Let cover the steps that lead to this implementation:

* Identify responsibilities (formatting logs, printing)
* Attach main responsibilities to your base class (formatting logs)
* Other aspects should go to *policy* classes
* Define a simple API for extern responsibilities
* Code

This idea has been developped in *Modern C++ Design: Generic Programming and Design Patterns Applied* by Andrei Alexandrescu, under the name *Policy*. The book covers way more aspects, but the main idea is here.

The previous process can be described in another (making it available for refactoring):

* Apply separation of concern on your code (formatting and printing)
* Once code is split in multiple functions, identify the stable one (formatting) and those open to latter changes (printing)
* Put functions open to future changes in separate classes using separation of concern again to regroup them meaningfully.
