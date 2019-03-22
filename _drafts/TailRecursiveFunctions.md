---
title: Tail Recursive Function
---

This is a back to basic post motivated by some comments I've recently read on-line about
recursive functions and the tail recursion optimization. Before explaining this notion of
optimization, let's see what is a tail recursive function.

## There and Back Again

Let's use a toy useless function as example. Say, you want to compute addition of
integers, but you're language only know how to compute `+1` and `-1`, and of course, we
want to do it recursively. Here is a quick implementation in python:

```
def add(x, y):
    if y == 0:
        return x
    return 1 + add(x, y - 1)
```

Forget about negative numbers, or cases where `x == 0`, we can solve them separetly.

If run my function with `3` and `2`, here are the execution steps:

```
add(3, 2) -> 1 + add(3, 1)
    add(3, 1) -> 1 + add(3, 0)
        add(3, 0) -> 3
              -> 1 + 3
          -> 1 + 4
-> 5
```

You can see that once you get the result of a recursive call, you can compute the `+1` and
then return your current result. We're going *there* until we reach the case, and then
*back* to compute the result.

Now, let's use magical math tricks, and write a similar function:

```
def add(x, y):
    if y == 0:
        return x
    return add(x + 1, y - 1)
```

It looks pretty similar, it has the same complexity (linear on `y`), is it different ?

```
add(3, 2) -> add(4, 1)
    add(4, 1) -> add(5, 0)
        add(5, 0) -> 5
              -> 5
          -> 5
-> 5
```

Yes ! Once we reach `y == 0`, we have the final result, the rest of the steps are just a
cascade of returns without any computations. This version is a _tail recursive function_ !

A tail recursive function, is a function that returns a value without recursive call or
returns the result of a recursive call directly, nothing happen after the recursive calls.

## Optimizing tail recursive functions

The optimization is pretty obvious, once we reach a direct return, we want to skip
every frames of recursion and directly return from the original call.

*How do we do that ?*


