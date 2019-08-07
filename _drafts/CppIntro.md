---
Title: A gentle introduction a to C++
---

I can't promise to start a real serie of articles forming a form of C++ lecture, so I decided to write an
introduction to the language first.

When someone wants to learn C++, there's a lot of material available on the net, good and bad, for
beginner and confirmed programmers. So, I wanted to write something with a different approach. I'm guided
by two main idea: the modern C++ introduced by C++11 and later standards and C++ can be easy.

Most people that don't like C++ and think that this language is not for them, have a bad start with it.
My first attempt at learning it at the end of the 1990's was a good experience on how not to learn a
language. About a decade later, I was again at it, but this time with a huge interest into what seems to
be a weird beast named template. And then C++11 was released and I was catched.

I think now that the language can be approach in an easy way (at least an easier way) and no longer
requires a multi-core brain working at lower level, abstract object model level, contextual parser level
and why *I'm doing that level*, all at the same time.

But don't get me wrong, this language is still a strange beast and probably requires several life to
learn, but at least you can start softly, following this gentle introduction.

This post is not an introduction to programming, so I expect that you know basic programming stuff like
loops or functions and a bit of algorithm. Knowledge of Object Oriented Programming (OOP) is not really
required, but basic notions can help.

## C++ is not (that) hard ##

I consider that a language is hard to learn when it's difficult to do the kind of simple stuff you want
to write when trying it out, as long as it makes sense (like trying to do loops in a pure functionnal
programming language). This is vague, but in my teacher's experience, I've identified some pain points,
here they are:

* Static validation
* Explicit typing
* Pointers and memory management
* Weird syntax details

For *static validation*, I can't do anything for you. It's a feature, not an issue. C++ belongs to the
familly of languages that have been designed to scale, and one important thing you need is static
validation. So, yes, you need to declare variables before using them, and unused (bad) code can prevent
compilation, so be it. That's what we want !

*Explicit typing* is another beast. Some aspects are wanted (they are part of the static validation
process) and fortunately the recent changes started with C++11 tends to let you remove explicit typing
when it's not needed.

*Pointers and memory management*, this is usually the hard one, this is also the main argument for those
that see C++ as a dangerously unsafe language and want you to use a language with an automatic garbage
collector, or even worse they want you to use rust and its boworring semantics. Again, in modern C++, you
only need to care when you ... need it ! That is, a lot of code can be written with any special
consideration to memory.

For the syntax, I don't know, I no longer see C++ code as chaotic output of a random generator biased
over ponctuation symbols, but I'm probably used to it now.

So, finally, among four pain points, two seem solved and another is not really an issue. We're good to
start this gentle introduction.

## An example first ##

Showing code is for me the best introduction to a language. Of course, I will skip the stupid *Hello
world* that would only shows you that in C++ we have braces and semi-colon. No, let's do something a bit
more advanced, something that looks a simple coding exercise.

We want to implement a basic version of the `sort` command. This command read lines from the standard
input and output them sorted (using natural string lexicographic order).

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    std::vector<std::string> lines;
    for (std::string line; std::getline(std::cin, line); ) {
        lines.push_back(line);
    }
    std::sort(lines.begin(), lines.end());
    for (const auto& line : lines) {
        std::cout << line << std::endl;
    }
}
```

How does it looks ? Take sometimes to read it and try to understand how it works. Maybe you will find all
those `std::` prefixes a bit boring and for a short piece of code like, I agree, so let's get rid of
them:

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    vector<string> lines;
    for (string line; getline(cin, line); ) {
        lines.push_back(line);
    }
    sort(lines.begin(), lines.end());
    for (const auto& line : lines) {
        cout << line << endl;
    }
}
```

Note that it usualy considered as a bad practice to open the standard namespace, for a lot of good
reasons, but most of them don't apply to small piece of code like this.

What parts of this code may cause trouble ? Let's review it together.

* Includes (and the using statements) are pretty straightforward, `algorithm` contains stuff like `sort`,
  `vector` provides dynamic arrays and `iostream` one of the most common I/O tools.
* The `main` function is a classic, it's our entry point, it returns an int since the OS expect it
* We have a variable declaration, no surprise here we want a dynamic array of string
* The input loop: this a C like classical `for` loop without a *next* statement
    * we use the fact that whatever `getline` returns, it will be evaluated to false when we're finished.
    * `cin` is our standard input.
    * `push_back` does what it says.
* I think the `sort` line is obvious.
* And the output loop:
    * what the hell is `const auto&`, I should I've written something more explicit here, but it's a
    constant reference to whatever kind of content we found in `lines`, a more simpler `string` would
    have been enough here.
    * `cout << line << endl` ? Here comes the stream output ! Read it like this: send `line` and then
    `endl` (the end of line) to the standard output.

Did you notice ? There's not a single (visible) pointer, no allocation, no `new` nor `delete`. And still,
this program is doing something, and it's manipulating dynamic contents !

## Why we don't care about memory most of the time ##

A lot of modern languages comes with an automatic garbage collector, this is needed for some dynamic
behaviours. But do we really need it ?

C++ has a completely different approach. Most entity we're dealing with has an explicit lifetime, they
are bounded to a function or to scope. So, we should be able to throw them away without relying on a more
complex beast.

Take my variable `lines` in the previous example, when reaching the end of the `main` function, it no
longer exists and the compiler injects a call to its destructor. Of course, the memory management logic
is still there, inside the definition of the `vector` class, but as is the GC in other languages. The
same logic applied to `line` variable in the first loop, it only lives during the loop lifetime.

Instances of `vector` or `string` have a *value-semantics*, they behave like a basic value and handle
behind the scene the necesserary memory management. This is at the heart of one of the most important C++
concept: **Resource Acquisition Is Initialisation** (RAII).

When such objects are created, they acquire resources (like memory) and will dispose of it when
destroyed. This is also what happen to the strings inside my `lines` vector, when the vector is
destroyed, all the lines inside are. The language enforces this logic in a lot of tools provided by its
standard library (the Standard Template Library, STL).

### Reference, copy and move ###

C++ is liberal, you can control a lot of behaviours that are fixed in other languages.

The natural and standard way to pass arguments to functions is by value (thus by copy), but you can
decide to use references, explicit pointers, or an *r-value references* (more on that soon).
