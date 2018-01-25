---
title : Reflexions on Teaching Programming
---

I've been teaching programming and computer science for almost two decades now.
But it's about to change and before starting a new life, I want to lay down some
reflexions on this topic.

## Abstraction or no abstraction ##

The first question you must consider when building a programming lecture is
about your target audience: what do they know, why do they want to learn
programming ? I will focus on the classical beginner CS students where a
programming lecture will form the fundation of futur software engineers.

At that point, comes the usual discussion about abstraction. Do you need
abstraction, do you hide the dirty reality behind high level programming
tools ?

That's not an easy question. More abstraction and higher level tools usualy
provide quicker results: your students are able to write code that do
_interesting things_ more easily, they may even get shiny little programs they
can be proud of. While quick result is often a good source of motivation for
students, the easy path is not always the best one.

On the other hand, ''the hard way'' can be frustrating, most students are not
excited by writing programs that print boring numbers on a black and white
terminal. But, on the long run, it simplifies the introduction of complex
concepts.

But, let's start now !

## False simplicity: hidding memory ##

Functions are one of the most fundamental concepts in programming, the concept
is pretty easy to begin with but some complexity can arise as long as you leave
the comfortable world of pure functions.

One of the usual issues of students is related to arguments passsing. Let's
illustrate that with a tiny example, a function that fill an array, first in C:

```C
void fill(int t[], size_t len, int val)
{
  for (size_t i = 0; i < len; i++)
    t[i] = val;
}
```

We can do the same in python:

```python
def fill(t, len, val):
        for i in range(len):
                t[i] = val
```

Pretty clear, isn't it. Now, let's forget C for a while and all you know about
pointers, and look at the python code. It seems that we're able to modify
arguments, does it work in any case ? We can test it with a tiny script:

```python
def update(x):
        x += 1
a = 0
print(a)
update(a)
print(a)
```

This code will only print 0 (twice), the variable is not modify ... OK, it's
an integer variable, maybe with a list like in the previous example ? The
next example will try to figure it out.

```python
def update2(l):
        l = [1, 2, 3]
t = []
print(t)
update2(t)
print(t)
```

Oups, again update2 has no effect ... If you're a C programmer, you've probably
already guessed what's happening here.

In order to explain this concept to people that have never manipulate memory,
pointers or adresses, you'll need a complex notion of object-reference passing
(or whatever you call it). In my experience, students get lost at that point and
have a lot of difficulties to handle correctly in which cases arguments can be
modified or not.

Now, if your students have been exposed to pointers and memory, you just need to
tell them that composed values are manipulated through a pointer, thus when you
pass a list, and modify its content you use that pointer but if you replace the
list itself, you replace the pointer which is passed by value.

It's also easier to explain why the following expression doesn't create the
expected matrix:

```python
m = [[0] * cols] * lines
```

If you don't know (or don't see) the problem here: the inner list construction
build a list value (thus a pointer) and then the outer construction repeat this
value. We then just get a list of lines time the exact same list.

This is an example of the false simplicity behind abstraction, the hard way may
seem difficult for the beginner, but it pays more in the long run.

## Keep it simple, stupid ! ##

Among mistakes I've seen during my teaching years, one of the most difficult to
eliminate is what I called ''corner case programming''.one of the most difficult
to eliminate is what I called ''corner case programming''.

In short, corner case programming describes the tendency that some students (and
even programmers) have to add a lot of tests for useless corner cases. Studying
algorithm can help, but it's an every day fight.

Let's have a look at some common examples.

Let's begin with the traditional useless subcases tests. Let's consider the
classic recursive implementation of factoriel with some useless tests:

```python
def fact(n):
        if n == 0:
                return 1
        if n == 1:
                return 1
        return n * fact(n - 1)
```

This is a pretty stupid version, we can catch both cases with one single test,
but it's not just about gaining few lines, the correct version does less tests
at each call, can resist to negative numbers ...

```python
def fact(n):
        if n < 2:
                return 1
        return n * fact(n - 1)
```

Let's see a similar naive example with an iterative version:

```python
def fact(n):
        if n < 2:
                return 1
        r = 1
        while n > 0:
                r *= n
                n -= 1
        return r
```

For this version, again, the tests are useless, there's also a useless loop run
(for n == 1), a better version:

```python
def fact(n):
        r = 1
        while n > 1:
            r *= n
            n -= 1
        return r
```

Of course, these are pretty simple examples, but as I said, we want to eliminate
all bad ideas ...

Let's have a look at a more complex example. We consider a classic B-tree
implementation (in C) a function that just check if the keys are correctly
ordered. For the sake of simplicity, we suppose basic integer keys.

```C
#define RANK ...
struct btree
{
  unsigned nbkeys;
  int keys[RANK * 2 - 1];
  struct btree *children[RANK * 2];
};

int btree_check(struct btree *t, int low, int high)
{
  if (t != NULL) {
    for (unsigned i = 0; i <= t->nbkeys; i++) {
      if (i == 0) {
        if (low > t->keys[i] || (t->children[i] != NULL && !btree_check(t->children[i], low, t->keys[i])))
            return 0;
        continue;
      }
      if (i < t.nbkeys) {
        if (t->keys[i - 1] > t->keys[i] || (t->children[i] != NULL && !btree_check(t->children[i], t->keys[i-1], t->keys[i])))
          return 0;
        continue;
      }
      if (t->keys[i - 1] > high || (t->children[i] != NULL && !btree_check(t->children[i], t->keys[i-1], high)))
        return 0;
    }
  }
  return 1;// An empty B-tree is well ordered
}
```

This code is inspired from students codes, and in fact, I've already simplified
part of it ...

What are the issues here ? The most obvious problem is the tests on the index
inside the loop, this is a classic clue that's something is going wrong. The
second problem is little more subtle: are you shure that code works, does what
we expect ? Even if you know the algorithm, it's difficult to check. And if go a
little bit deeper, there's performance issues also ...

Here is a better version:

```C
int btree_check(struct btree *t, int low, int high)
{
  if (t) {
    if (t->keys[0] < low || t->keys[t->nbkeys] > high)
      return 0;
    for (unsigned i = 0; i < t->nbkeys - 1; i++) {
      if (t->keys[i] > t->keys[i + 1])
        return 0;
    }
    if (t->children[0]) {
      for (unsigned i = 0; i < t->nbkeys; i++) {
        if (!btree_check(t->children[i], low, t->keys[i]))
          return 0;
        low = t->keys[i];
      }
      return btree_check(t->children[t->nbkeys], low, high);
    }
  }
  return 1;
}
```

OK, this looks better, we can still improve some details, but this one is far
more acceptable.

What are the important aspect ? We've pulled out loop invariants that the
compiler can't detect (in a B-tree, a node as 0 or exactly `t->nbkeys+1`
    children). We've split the loop in two, keys and children don't need the
same number of iterations, thus it simpler to have them in separate loops. This
is also a performance improvement, since if the keys are not well ordered, we
don't continue in the rest of the tree. There's still a lot of "if", but the
code is more understandable that way.

One last example, let's have a look at linked lists. I really like that one,
it's the simplest demonstration that we should prefer smarter data and simpler
code. We begin, with a classic implementation of lists of integers with external
allocation (list cells are not allocated by the list operations) and we provide
two classic operations: a push front and an ordered insert.

```C
struct list
{
  struct list *next;
  int data;
};

void push_front(struct list **list, struct list *elm)
{
  elm->next = *list;
  *list = elm;
}

void insert(struct list **list, struct list *elm)
{
  struct list *cur = *list, *prev = NULL;
  for (; cur && cur->data < elm->data; cur = cur->next)
    prev = cur;
  elm->next = cur;
  if (prev) {
    prev->next = elm;
  } else {
    *list = elm;
  }
}
```

Now, if we just add a sentinel (you know, that fake cell at the beginning of
the lists), we obtain something a little bit simpler:

```C
void push_front(struct list *list, struct list *elm)
{
  elm->next = list->next;
  list->next = elm;
}

void insert(struct list *list, struct list *elm)
{
  for (; list->next && list->next->data < elm->data; list = list->next)
    continue;
  push_front(list, elm);
}
```

Of course, my first version can be simplified to a similar point (using
    iteration through double pointer).

One of the idea that I've tried to pass to my student, is that when you write
some code or design an algorithm, you should always begin with the general
case, and then see if some subcases requires some specific code. This is a
variation of the classical *keep it simple, stupid*.

We have various reasons to follow this strategy:

* in most cases, the general case already cover correctly corner cases, adding
extra tests is useless and will probably waste computing time;
* since the general case already covers those subcases, the corresponding code
is redundant;
* tests for corner cases are often more complex than what it looks like at the
beginning, if the corresponding code is specific and works only under the
assumptions of that corner case, these tests can introduce subtle bugs;
* redundant code and useless tests make the maintainance of the code more
difficult;
* and finally, admit it, we're lazy and prefer shorter code.

Issues with redundant code are pretty obvious, dangerous extra tests are a
little more complex. For example, in students' code for binary tree, there's a
tendendcy to try to access children or even grand-children of current node, of
course accessing those nodes requires complex tests and a lot of combinations,
it's pretty easy to forget some cases.

## Reverse pedagogy ##

On a less technical side, teachers face another difficulties: students not
seeing the interesting aspect of what you're talking about. This is frustrating,
you've spent a lot of time building a lecture, finding a way to explain complex
notions, and students just don't care. And worst, later (during project defense
for example), they admit that it could have been useful to pay a little more
attention ...

With other fellow teachers, we've discusted that issue a lot of time. One of the
idea that finally came out: reverse pedagogy. The idea is to let them try and
probably fail first, and then explain them how they should have done it.

I must admit that I haven't found a correct process, probably because doing it
correctly requires an important reconfiguration of the cursus. Anyway, there's
some ideas that can be apply to a programming cursus.

What is also important, is the nature of notions that may benefit from reverse
pedagogy. From my own experience, notions related to code organisation,
modelisation, project management … Those notions are somehow indirect, you can't
build some simple exercise to demonstrate them.
