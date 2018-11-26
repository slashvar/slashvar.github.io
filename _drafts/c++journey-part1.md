---
Title: A C++ journey - part 1
---

I decided to start a new serie about C++, my goal is not to begin a new
tutorial, not to present ground breaking C++ tricks. No, just the way I see the
language, what I found interesting and useful, what I found annoying, badly
design or missing.

Since it's the first post in the serie, I'll start with a quick introduction,
not on C++, but on why C++. And then, I'll attack what I found the most
important aspect of the language: scope, value semantic and RAII.

## Why you should love C++

C++, a little bit like C, has a long love-hate relationship with programmers.
It's a complex and very rich language, difficult to learn (and full of pitfalls)
and not appealing to beginners.

Presented that way, most new commers will probably run away and only mad techies
will want to learn that beast. But, the truth is that all those frigthnening
issues are the price of power and performances. At least, that's what a lot of
people think.

Back 10 years ago, I would have agreed on this. But, the language has evolved,
     but most of it, I've learnt to love it. A good friend of mine use to call
     this a kind of _Stockholm syndrome_, and I somehow agree a lot on this.

So, how could I convinced you that it worth giving a try to C++ ? I don't know.
I can only try to expose what I think make this language probably the best
available nowaday. But don't be naive, learning C++ is still difficult, there're
still dark corners of the language that should have never been added, and
sometimes (more often than needed) programming in C++ can be painful.

What are the good points ?

* Access to the lower level of programming.
* Performances (as in your code will run as fast as you can expect).
* Abstract constructions.
* Static meta-programming.
* Still evolving.
* No useless constraints (almost).
* A good standard library.
* Always something new to learn (common ... )

And the bad points ?

* Boring old fashioned OO stuff (classes, inheritance, visibility ... )
* Strange default behaviours (visibility ... )
* Some missing concepts (less and less ... )
* The stuff I forgot that will bite me tomorow ...

The good point is that most inconvenient aspects are avoidable, it's just that
the language provides features that I don't like. The bad point is that most
interesting stuff are also those that makes it hard to learn (some more
    templates ?)



## Values, values everywhere

## OOP or not OOP
