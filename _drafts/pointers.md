---
Title: Pointers
---

Let's talk about pointers and memory. It's one of my favorite subject and in my
opinion one of the most important notion in programming.

Of course, talking about pointers implies talking about C or C++, they are the
two languages where you need them most and you can avoid them, but pointers are
everywhere in programming, that just that sometimes, they're hidden by the
language.

This post requires at least some basic knowledge of C, mostly its syntax and the
most obvious part of its semantics. If you're not a C programmer (but you're
    familiar with languages that use C syntax like Java) don't be afraid you'll
probably be able to understand most the text and, I hope, you'll learn something
useful.

## A bit of theory ##

For the sake of clarity, the rest of the text will assume a simple memory model
which more or less maps to what you could find on modern PC running Linux, that
is a model where memory addresses are just unsigned integers indexing bytes.

The semantics of the C programming language, in order to support (almost) any
kind of architectures is more complex. This model explains some particular
undefined behavior.

In short, what is defined is that you got addresses and you don't know they're
nature, the language only guaranteed the expected behavior of arrays and of
pointers used as array. The whole definition of pointers is a bit obscure, it
defined mostly when pointers are supposed to be equivalent and defined.

That is to say, you can live with it as long as you don't try to rely to much on
the numerical nature of pointers outside of arrays. But keep in mind, that
anything that is not explicitely defined (or defined as implementation
    dependant) may be compiled differently than what you expect.

So far for the theory.

## There's no arrow ##

The most common illustration of pointers involves boxes and arrows. After
several years of teaching C, it appears to me that this is the worst way of
presenting pointers.

A pointer is a value (often numerical) like any others. In order to manipulate
it, you need to store it (in a variable, a field of a structure, an array cell
    ... ) and you can copy it in another storage, it can be valid or
uninitialised, it can become invalid ...

```c
int x = 42;	// we an address to init our pointer
int *p;		// p is a variable that can contain a pointer to an int
p = NULL;	// p contains the null address, an existing but unusable pointer
p = &x;		// p now contains the address of x
int *q = p;	// q is a variable containing the same address as p
int y = 0;	// another variable in order to have an address
p = &y;		// p contains the address of y, q still contains the one of x
printf("%d %d\n", *p, *q);
```

When declaring a pointer as in `int *p`, you define a storage (a variable) where
you can put an address, but until you initialize it with a proper address, it is
unusable, just like any other variable.

Just like an address in real life, if you write it in you contact book, it
doesn't mean that it will be updated when the people living here moves or if the
house is destroyed, and if you have a line for the address of a contact but don't
put anything inside (or put something that is not an address) you won't be able to
send any letter to that address, it is not valid !

Once you got this in mind, you'll be able to understand most errors related to
pointers. It doesn't mean that you won't make errors or that you'll find them
easily, but at least you what you're playing with.

Let's now do the basic.

## Basic usage of pointers ##

The following piece of code illustrate classic usage of pointers, read it and
read the comments !

```c
#include <stdio.h>
#include <stdlib.h>

/*
 * reset(a) puts 0 at the address a
 */
void reset(int *a)
{
  *a = 0;
}

/*
 * swap(a, b) swap the integers stored at address a and b
 */
void swap(int *a, int *b)
{
  int c = *a; // copy in C the value stored at address a
  *a = *b;    // copy at the address in a the value at b
  *b = c;     // copy the value from c at the address in b
}

/*
 * divide(a, b, m) returns a/b (as int) an stores at m the rest
 * if b is null, returns 0 and don't touch what m points to
 */
int divide(int a, int b, int *m)
{
  if (b == 0) {
    return 0;
  }
  *m = a % b;
  return a / b;
}

int main()
{
  int x = 27;
  printf("x = %d\n", x);
  reset(&x); // send to reset the address of x
  printf("x = %d\n", x);

  int a = 2, b = 42;
  printf("a = %d\nb = %d\n", a, b);
  swap(&a, &b);
  printf("a = %d\nb = %d\n", a, b);

  int m = -1;
  printf("a/b = %d\n", divide(a, b, &m));
  printf("m = %d\n", m);

  return 0;
}
```

You can compile that code and run it:

```
> make basic.c
cc basic -o basic.c
> ./basic
x = 27
x = 0
a = 2
b = 42
a = 42
b = 2
a/b = 21
m = 0
```

Just to resume what we've seen: `reset(a)` is a function that use a pointer to
store a value outside of its scope; `swap(a, b)` exchange the content of 2
addresses and `divide(a, b, m)` computes integer division and store the rest at
an address provided. This three functions illustrate common (basic) use case of
pointers: be able to modify variables (or any storage) outside of the current
function.

Even if it's not the official vocabulary, you can see this as a reference call
semantics, you pass a reference to a local variable in order to modify it.

The traditional call semantics of almost every language is a call by value:
function's parameters values are copied into the parameter storage. If you want
to modify variables local to the caller, you must pass the address of those
variables. The callee always assumes that you pass valid addresses (it can't
    verify them anyway ... )

Note: in many higher level programming languages (dynamic or not like Python,
    Java, OCaml ... ) constructued values are always manipulated through pointers,
this means that when you pass them to a function, you pass them by reference.
This often call *pass by object reference*. In my experience for beginners never
exposed to pointers before, the induced behavior is often disturbing: you are
able to do some operations that modify the parameters in the caller scope, but
some modifications are still local. Explained through pointers, this is far more
simple: you have this address on an object, you can modify what the address
point at but any operations modifying the address itself act only at the local
scope.

```c
void f(int *x)
{
  int y = 0;
  *x = *x + 1; // change the foreign stored value
  x = &y;      // has no effect outside of f
}
```

## Common mistakes ##

There's an infinte number of ways to misuse pointers. Let's see some classical
errors.

### Returning pointers ###

A pointer is a value and like any other value you can return a pointer in a
function.

```c
// Global variable for the sake of the example
int my_global = 42;


/*
 * a useless function
 */
int* address_of_my_global()
{
  return &my_global;
}

```

when an address is returned by a function, you can expect that callers of this
function will use it. This means that: it must be valid (eventually `NULL`). To
be valid and dereferencable, it must point to a storage that has a sufficiently
long life time, like the example above (`my_global` will stay as long as the
    program runs). Returning the address of a local variable (or of a parameter)
will inevitably leads to bugs, in fact this an obvious case of **undefined
behavior**. As usual in C, it will seems to work in certain case: local
variables are stored on the stack which is a kind draft for functions, right
after the call, the content of the local variable may probably still be
available, but as soon as another part of the program use the stack it will be
overriden.

```c
/*
 * writing this code in real program deserve to be ashamed for life
 */
int* dont_do_that()
{
  int x = 42;
  return &x;
}
```

In obvious case like this one, the compiler will be able to warn you:

```
shell> cc -Wall -Wextra -std=c11 -c dont_do_that.c
dont_do_that.c: In function ‘dont_do_that’:
dont_do_that.c:7:10: warning: function returns address of local variable [-Wreturn-local-addr]
   return &x;
          ^~
```

**Note:** I've used the the flags `-Wall -Wextra`, they activate the *standard*
warnings (depending on you compiler, you can activated more warning, `clang` for
    example provides a flag `-Weverythin` that activates all possible warnings).
You should always do that, warnings are always the sign of something wrong,
sometimes you have a good reason, but it is more likely that the code will
break at any time later.

### Invalid and null pointers ###

Pointers (strictly speaking variables of type pointer) can be in three states:

* *initialized and valid*: the address stored in the variable points to a living
  object in memory
* *null*: the pointer contained the so called null pointer, usually the address
  `0`, represented by the macro `NULL`, in this case the pointer is valid but
  you can't dereference it.
* *invalid*: the pointer has not been initialized, it contains garbage or the
  object at the referenced address no longer exits.

Dereferencing an invalid or null pointer is a case of undefined behavior and may
lead to an error at execution (always for the null pointer, and for the invalid
    address you may have accidentaly an address of an existing object).

Using an invalid pointer in an other kind of operations (like pointer arithmetic
    or even passing it to a function) is a case of undefined behavior. Note that
you can use a null pointer in any operation not involving a dereference. In the
same idea, the address right after an object (or the last object of an array)
  can be used in non-dereferencing operations.

This is the perfect time to talk about arrays and pointers arithmetic.

## Arrays and pointers arithmetic ##

In most programming languages, an array is a contiguous area containing a
collection of object of the same type. They are of a fixed size (at some point
    at least, but you may be able to update their size).

In C, the implementation of arrays are related to pointers, but to the contrary
of what a lot of people think, arrays and pointers are not exactly the same, but
I'll show you that later.

If my address are numerical, accessing an element of an array is as simple as
taking the address of the beginning and add the amount of bytes corresponding to
the number of elements to skip. For example, if I have 4 bytes integers and I
want to access the third element, I will add shift the address by 8 bytes (skip
    the first two). Knowing the size of the elements is annoying, the compiler
know their type and thus their size. This is where pointer arithmetic shows up !

Adding an integer `n` to a pointer, moves the address by `n` objects, provided
that this pointer points to an array of at least `n` objects (the language
    guaranteeds the existing of the address of the last object plus one). So,
you can navigate in arrays using additions !

```c
/*
 * sums the content of an array using the classical [] notation
 */
int index_sum(int array[], unsigned size)
{
  int res = 0;
  for (unsigned i = 0; i < size; ++i) {
    res += array[i];
  }
  return res;
}

/*
 * sums the content using only pointers
 */
int pointer_sum(int array[], unsigned size)
{
  int res = 0;
  for (int *it = array; it != array + size; ++it) {
    res += *it;
  }
  return res;
}
```

These two functions are completely equivalent, and your compiler will probably
generate a similar machine code for both.

In fact, the expression `array[i]` is completely equivalent to `*(array + i)` or
for pedantic C programmers `*(&array[0] + i)`.

**Fun fact:** we all know that addition is commutative, that is `a + b` is
equivalent to `b + a`, in most algebra. This is true for pointers as well. It
turns out that `*(array + i)` is equivalent to `*(i + array)` which means that
you can (but that's naughty) write `i[array]` !

A lot of code are using pointers directly rather than arrays, for a lot of
reason and this leads also to the concept of iterators in C++. If I rewrote my
sum function this way:

```c
int iterator_sum(int *begin, int *end)
{
  int res = 0;
  for (; begin != end; ++begin) {
    res += *begin;
  }
  return res;
}
```

It may look familliar to C++ programmers !

Using pointers arithmetic are useful in a lot of cases. This is a classical
recursive implementation of a binary search (a lower bound in fact) using only pointers:

```c
/*
 * find(begin, end, x) find x in the sorted range between begin and end
 * returns the address of the first element not smaller than x if it is not
 * present
 */
int *find(int *begin, int *end, int x)
{
  if (end - begin < 1)
    return begin;
  int *mid = begin + (end - begin) / 2;
  if (x == *mid)
    return mid;
  if (x < *mid)
    return find(begin, mid, x);
  return find(mid + 1, end, x);
}

/*
 * iterative version
 */
int *find_iter(int *begin, int *end, int x)
{
  while (begin < end) {
    int *mid = begin + (end - begin) / 2;
    if (x == *mid)
      return mid;
    if (x < *mid)
      end = mid;
    else
      begin = mid + 1;
  }
  return begin;
}

```

In the previous functions, we've seen several interesting constructions:

* `end - begin`: pointers substraction yield the distance (in number of objects) between them
* `find(begin, mid, x)`: a way to call with a smaller array
* `find(mid + 1, end, x)`: a way to call a function in the middle of an array
* `begin + (end - begin) / 2`: computes the mid point, note that we use half of
  the distance rather than the classical arithmetic means (`(begin + end) / 2`)
  to avoid possible overflows (which, when manipulating pointers will occur).

In all the previous example, I've used pointers to designate a range of objects,
using the implicit convention that `begin` is the first element and `end` the
first element outside of the range (half-open range). This is a classical choice
in programming. Not doing things that way, you will have to play a bit more at
the risk of doing out-of-bounds access or forgetting elements.

  **Always use half-open range with left bound included and right bound
  excluded**


