---
title: What Makes A Good Programmer ?
---

So, you've learn to code and you want to know if you're good enough ? Are you
still a beginner or can you qualify yourself as an expert, or something in
between ?

If you're a C or C++ programmer, I've got an answer that may sound like a
philosophical joke: You'll know enough of C once you realize that you'll never
know enough and it'll come later for C++.

But maybe C is not your language (how about switching, C is good for you).
Anyway, the language does not really matter, programming is not knowing all the
details of a language, it's much more complex.

Let's explore that question, what makes a good progammer ?

**Disclaimer:** I'll focus on the technical side, not that other aspects are not
important, but I've just chosen this aspect today.

## The project syndrom ##

If you start asking yourself about how good you are in programming, it's
probably because you start doing code by yourself outside of all those shiny
tutorials on the web. You've probably done some nice projects that your proud
of.

The trouble, and you may already have face it, is that a project that looks good
and awesome for a beginner, will look basic and dirty for a more experienced
programmer. Even worce, when you look back at what you've done in your first
projects, you'll probably be disapointed by the low quality of the code.

The size or the complexity of the project is no more relevant, if you are (or
    was) a student in CS, you've probably done some hard stuff that are more
complex than the everyday project of most programmers. Most students have to
write things like compilers, shells ... Those projects, even in their
pedagogical form, are not basic stuff. And all those students are not (yet) good
programmers.

No, we need a better metrics.

## Time and Effort ##

Another idea is to look at how many time does it take you to write that project
? That's a little bit more interesting. One of the common reaction when you look
back at your code is: it was long and painful at that time, but now I should be
able to do that faster.

But time is relative, maybe you've done that project in three days, but in fact
you've spent the equivalent of a week of work (you know, 8 hours a day plus the
    meeting and all the nois.)

And if you think of it, you may type faster now, you know all the nice litte
features of your editor, and you've learnt how to use all those libraries that
will simplify the code of this project.

So again, not time ? But we've got something here, just like we judge programs
and algorithm using a relative abstraction of time (the so called time
    complexity), we can use an abstraction of programming time: the effort
needed to write the code.

Let's take a simple and basic example. Rather than a full program, let's
consider a basic algorithm: searching for a value in a sorted array. The first
time you encounter that problem, you've probably get the idea of linear search
quite easily, then you try to write it. A lot of questions rise at that moment:
what do I return, what happen if the value is not present and I want to return
it's position, what happen if the array is empty ?

Let's fix that: we're in C and we want to return the position of the first
element that is not smaller than the one we're looking for, if it's not present
we will return the first position (still pointer) outside of the array. Here is
an artificially dummy version:

```c
int* find(int array[], size_t size, int value)
{
  if (size == 0)
    return array;
  else {
    size_t i;
    for (i = 0; i < size; i++) {
      if (array[i] >= value)
        return &array[i];
    }
    return array + size;
  }
}
```

OK, we can do better, here is how I'll write a linear search:

```c
int* find(int array[], size_t size, int value)
{
  int *cur = array;
  for (; cur < array + size && *cur < value; cur++)
    continue;
  return cur;
}
```

Note that the code is shorter, but what is more important is that I've far less
effort thinking to that solution, because I don't need to think about most
details (to be honest, I haven't even tested it). And, since I know a little bit
about algorithm, the real version I should write is this one:

```c
int* find(int array[], size_t size, int value)
{
  int *l = array;
  int *r = array + size;
  while (l < r) {
    int *m = l + (r - l) / 2;
    if (value == *m)
      return m;
    if (value < *m)
      r = m;
    else
      l = m + 1;
  }
  return l;
}
```

If you don't know the algorithm used here, I strongly encourage you to look for
binary search.

So, what's the point ? Writing those two functions doesn't require much effort
when you know what to do. But, I've **said** that it doesn't take much effort
and it's still a relative metrics, a better one, but still relative.

Of course, you can think that having less useless tests, a better algorithm and
all those nice pointer arithmetics is a sign of great programming. But, it's
only a matter of knowledge.

And I've silently ignore another factor: boredom. Some piece of code are not
hard, not even complex, but they take time just because there's no easy way to
simplify them, you just have to write all those stuff, line by line. But
programmers don't like to talk about that part of the job.

## Self evaluation of programmnig skills ##

No, we really want something that we can measure. Let's take the problem from
another point of view.

What's the job of a programmer ? Solve problems, write the code, make it works.

When I look at my students when they're coding. What really makes a difference
is the number of time they need to call the compiler and run their tests before
they get a working program. On small examples like the one above, full beginners
may need tens of compiler calls before their code compile, while for an average
experienced programmer, their code should work the first time.

Now we got our metrics ! We can simply measure the number of errors that need to
be corrected on a simple piece of code.

But not only do we have a metrics, but we also have a training strategy !

## Are you sure ? ##

Let's justify a little bit this point. Programming is somehow similar to
writing. You need to find what you want to express and you need to express it in
a way that will be understand by someone else (your reader or the
    compiler/computer). And just like with writting, the technical step is the
translation of ideas into text (or code). Having idea is another part of the
job. In fact, for programming, an important part of the job doesn't really need
ideas, but it needs to be written.

If you had to learn a foreign language, you've probably come to this point
where, you have something to say but found it difficult to express in this
foreign language. Then, after some practice, you've reach that point when you no
longer need to translate from your native language, you start thinking directly
in that foreign language. It may not be perfect, you may still need to get away
with expressions or construction that are specific to your native language, but
at least you can express your idea. As an example, I'm currently writing in
english while my native language is french, but over the years, I spend less
time on **how to say that**.

And just like, foreign languages, when you reach the point when you start
thinking code directly, it becomes easier to express complex stuff and more
natural to find ideas. In other words, you need to become fluent in programming.

## Training to get better ##

As I said, our metrics gives us a way to train ourselves. At least, it gives us
a way to evaluate our progress.

As I see it, if you're not coding every day (for your job or your studies) you
can set-up a quick and easy warm-up session: find a set of simple programming
problems, pick-up one and write the code. The amount errors is your progression
indicator. Once you can write all those simple piece of code correctly at first
shot, increase the difficulty.

Programming interview ressources provide a lot of training materials, some of
them even provide the envirronment to run and test your code smoothly (like
    harckerrank or leetcode).

## Conclusion ##

So, I've not completely defined what is a good programmer, but I think I've come
with a good metrics for the most technical side of the job.

But that's only one side of the problem. A programmer should also be good
at problem solving, code organization, modelisation ...

A nice aspect of training on classical samples of code is that you'll also learn
algorithms, data structures and performances. These aspects are important and
when you'll become fluent on this side, you'll have more time available for the
other aspects.

But don't forget that it's a never ending journey.
