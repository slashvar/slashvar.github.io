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
cascade of returns without any computations. This version is a **tail recursive function**
!

A tail recursive function, is a function that returns a value without recursive call or
returns the result of a recursive call directly, nothing happen after the recursive calls.

## Optimizing tail recursive functions

The optimization is pretty obvious, once we reach a direct return, we want to skip
every frames of recursion and directly return from the original call.

**How do we do that ?**

Of course, it really depends on the way you run your language, but let's assume that we're
in rather classic context, with a stack used for saving caller context and return address.
Our context if of course composed of local variables (including parameters), the address
of the beginning of the frame for the current function (the top of stack when arrived in
        the function, the frame pointer) and our own return address. Saving context on the
stack means that we push the frame pointer and the return address and we then we can push
parameters and jump to the callee. When we return from a function, we restore the frame
pointer and jump back on the return address. The details are not that important, it may
even be partially done by processors instruction, we don't care.

So how do we perform the tail recursion optimization ? After the initial call, we just
don't save the context ! That way, when we return, we return as if we were in the original
call. It works because we are sure that nothing happens after the return and thus we don't
need our context anymore. If you want more details, open a compilation book, this is one
of the classical optimization and it's pretty easy to implement.

It has two main impacts: we do only half of the jump (only there and no back) and we use a
constant space on the stack.

OK, this is highly theoretical, isn't it ? Can we see this optimization in different way
without relying on the compilation strategy ? Let's try.

First, let's take a more accurate example, binary search, in C this time:

```c
int binary_search(int *begin, int *end, int x) {
    if (begin >= end) {
        return 0; // x is not in [begin, end[
    }
    int *mid = begin + (end - begin) / 2;
    if (x == *mid) {
        return 1; // we found it !
    }
    if (x < *mid) {
        return binary_search(begin, mid, x); // search in the firt half
    } else {
        return binary_search(mid + 1, end, x); // second half
    }
}
```

OK, the idea is to rather than call the recursive function, replace `begin` or `end`, and
then jump at the beginning of the function. Let's use `goto` for that (yes, I've heard
        your screams):

```c
int binary_search(int *begin, int *end, int x) {
  _start:
    if (begin >= end) {
        return 0; // x is not in [begin, end[
    }
    int *mid = begin + (end - begin) / 2;
    if (x == *mid) {
        return 1; // we found it !
    }
    if (x < *mid) {
        end = mid;
        goto _start;
    } else {
        begin = mid + 1;
        goto _start;
    }
}
```
Let's refactor the firt `if` a bit:

```c
int binary_search(int *begin, int *end, int x) {
  _start:
    if (begin < end) {
        int *mid = begin + (end - begin) / 2;
        if (x == *mid) {
            return 1; // we found it !
        }
        if (x < *mid) {
            end = mid;
            goto _start;
        } else {
            begin = mid + 1;
            goto _start;
        }
    }
    return 0; // x is not in [begin, end[
}
```

Hey, it's looks like a `while` loop ! Yes, this is exactly what this optimization is
about, transforming tail recursive functions into a loop !

```c
int binary_search(int *begin, int *end, int x) {
    while (begin < end) {
        int *mid = begin + (end - begin) / 2;
        if (x == *mid) {
            return 1; // we found it !
        }
        if (x < *mid) {
            end = mid;
        } else {
            begin = mid + 1;
        }
    }
    return 0;
}
```

