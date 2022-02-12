---
Title: The infamous code readability question
---

# The infamous code readability question #

Code readability is a tricky question and often a source of conflicts. In this post, I wanted to lay down some thoughts around the topic. As a software engineer and before that as programming teacher, I have read a lot of code, I have wrote my own part and I have built some reflections on the topic.

## Style and readability ##

As an introduction, let's have a look at some code, let's consider that we are in the middle of some large function, and we found this tiny subset:

```c++
    // some code ...
    unsigned r = x;
    while ( x / r<r) r= (r+x/r )/2 ;
    // more code ...
```

Are you able to tell what it does? If you don't know this small algorithm, you will need to take a deeper look at it, maybe run it on some values to understand what it does. Of course, you may have recognized it as it is a classic. Do you think that reformatting will help here? 

Now, consider this version:
```c++
unsigned int_sqrt(unsigned x)
{
    unsigned r = x;
    while (x / r < r) {
        r = (r + x / r) / 2;
    }
    return r;
}

    // some code ...
    unsigned r = int_sqrt(x);
    // more code ...
```

Same code, but the use of a function changes everything. The benefits is double, when reading the definition of `int_sqrt` we know what the function is supposed to do and thus it is easier to understand it. Even better, the role of the code in the original context is clearer.

Ths point here is that style and formatting is not as important as it seems. In front of a complex piece of code, without any context, as clean, as well formatted as it can be, it will be difficult to read. Have you ever read some code where you can tell what the code does but not why?

My opinion is that readability is not a local property, it is more global. Functions, classes, modules and any other constructions used to structure the code have way more impact on readability than code itself. When functions define simple logical components (your verbs), objects and classes define coherent entities and modules provide toolboxes, code becomes way more readable.

## But ... ##

We all have seen those ugly and unreadable code, we all have seen really nice piece of code highly readable. And we all forged this idea that formatting, uniformity and some usual guidelines, is an important aspect of code readability.

But, unfortunately, I can tell you that I have read as much unreadable clean codes and dirty readable ones.