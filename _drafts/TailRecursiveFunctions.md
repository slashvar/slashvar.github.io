---
title: Tail Recursive Functions
---

This is a back to basic post motivated by some comments I've recently read on-line about
recursive functions and the tail recursion optimization. Before explaining this notion of
optimization, let's see what is a tail recursive function.

## There and Back Again

Let's use a toy useless function as example. Say, you want to compute addition of
integers, but you're language only know how to compute `+1` and `-1`, and of course, we
want to do it recursively. Here is a quick implementation in python:

```python
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

```python
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
Our context is of course composed of local variables (including parameters), the address
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
        return binary_search(begin, mid, x); // search in the first half
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
Let's refactor the first `if` a bit:

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

## Cool, we no longer need to bother, do we ?

Remember the first function ? You can't directly optimize it, you need to first transform
it into at taill recursive one, and that's not always obvious. The good news is that, it's
theoretically always possible, the bad news is that it's not always that simple. For
simple, single call functions, it's a matter of adding some parameters in order to carry
more information. But even in thoses cases, you need to be sure that you can reorder the
operations. Let's see a classic trap:

```python
def list_of_int(n):
    if n == 0:
        return []
    r = list_of_int(n-1)
    r.append(n)
    return r
```

We can pass the result list as a parameter and transform that into a tail rec version:

```python
def list_of_int2(n, r):
    if n == 0:
        return r
    r.append(n)
    return list_of_int2(n-1, r)
```

Now, we run both:

```python
print(list_of_int(5))
print(list_of_int2(5, []))
```

We get:

```
[1, 2, 3, 4, 5]
[5, 4, 3, 2, 1]
```

I let you correct it ...

## More calls, more problems

First, start with an easy one, Fibonacci. For that one, we need to add parameters and
inverse the order of computation:

```python
def fibo(n):
    if n < 2:
        return 1
    return fibo(n-1) + fibo(n-2)

# Becomes

def _fib(n, i, f_i, f_i1):
    if i == n:
        return f_i
    return _fib(n, i+1, f_i + f_i1, f_i)

def fibo(n):
    if n < 1: return 1
    return _fib(n, 1, 1, 1)
```

Note that while the function computes the same values, it is not a transformation of the first one but a different algorithm, a kind of recursive version of the loop based one.

Now, say I have a binary tree and I want to compute it's size:

```python
class Tree:
    def __init(self, key, left=None, right=None):
        self.key = key
        self.left = left
        self.right = right

def size(t):
    if t == None:
        return 0
    return 1 + size(t.left) + size(t.right)
```

It becomes a bit more tricky no ? Unlike Fibonacci, we can't avoid the call on each node
of the tree, we need to find a way to do them. For that, we can use a **stack** !

I'll give you directly the loop version, it makes more sense:

```python
def size(t):
    if t == None:
        return 0
    sz = 0
    stack = [t]
    while len(stack) > 0:
        n = stack.pop()
        sz += 1
        if n.right:
            stack.append(n.right)
        if n.left:
            stack.append(n.left)
    return sz
```

Note the order of the push, to be sure that we traverse the tree in the same order.

OK, but now, what if I want to print my tree in order (the root in the middle):

```python
def in_order(t):
    if t == None:
        return
    in_order(t.left)
    print(t.key)
    in_order(t.right)
```

The trick of stack is not enough, we need to simulate the there and back again, so we
really need to see each node at least twice !

```python
def in_order(t):
    if t == None:
        return
    stack = [(t, True)]
    while len(stack) > 0:
        n, cont = stack.pop()
        if cont:
            stack.append((n, False))
            stack.append((n.left, True)) if n.left
        else:
            print(t.key)
            stack.append((n.right, True)) if n.right
```

And if you need to do something in-order and post-order (so between the children and after
        the children), you will need to push each node three times in the stack. Of
course, with general trees, or graph, this becomes even more complex.

## Does it really matters ?

So, yes, it matters. Of course, with binary trees, maximum size of the stack is linear in
the depth of the tree which is most of the time not that big even for big trees. But what
about graph ?

Here is a classic algorithm that finds the cut-points of a connected undirected graph:

```python
def _cut_points(g, v, parent, c, pre, cuts):
    c[0] += 1
    pre[v] = c[0]
    high = c[0]
    children = 0
    for succ in g[v]:
        if pre[succ] == None:
            children += 1
            r = _cut_points(g, succ, v, c, pre, cuts)
            if r >= pre[v]:
                cuts.add(v)
            high = min(high, r)
        elif succ != parent:
            high = min(high, prev[succ])
    if parent == None and children > 1:
        cuts.add(v)
    return high

def cut_point(g):
    cuts = set()
    c = [0]
    pre = [0] * len(g)
    _cut_points(g, 0, None, c, pre, cuts)
    return cuts
```

OK, it's a DFS (depth first traversal) of an undirected graph, with pre-order counting
(the array `pre` using `c` to store the counter and simulate a reference). It works with a
similar idea as Tarjan's algorithm for finding strongly connected components, but
translated to undirected graphs. You can extend it to compute cut-edges and build
2-connected components. Open a good algorithms text book if you want to know more about
it.

What we see here is that we have multiple recursive calls (in a loop) and we perform
operation in pre-order, after each call and in post-order. So we need to meet each vertex
a first time, then do something after each of it's successors and then a last time. It's
easy to see that removing recursion won't be easy ...

Why this example ? Some years ago, for some experiments with huge graphs, I needed the
cut-points and I implemented the very same algorithm (in C++). On huge graphs, highly
connected, the recursion depth was pretty high and I ended up with a lot of stackoverflow.
The only way to get it to work, was to derecursify it. It's doable, it's finally not that
ugly (a big thanks to C++ iterators) and it saved my experiments !

I let you write your own as an exercise !

## Conclusion

To connect with my introduction, the first thing to remember is, yes, compilers are able
to optimize tail recursive functions, yes it makes some algorithms more tractable (a bit
        faster but mostly it avoids stackoverflows) but, no, it's not magical, you need to
do the hardest part, make your Function tail recursive in the first place !
