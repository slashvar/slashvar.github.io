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
probably be able to understand most of the text and, I hope, you'll learn something
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
easily, but at least you know what you're playing with.

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

**Note:** in many higher level programming languages (dynamic or not like Python,
    Java, OCaml ... ) constructued values are always manipulated through pointers,
this means that when you pass them to a function, you pass them by reference.
This often call *pass by object reference*. In my experience for beginners never
exposed to pointers before, the induced behavior is often disturbing: you are
able to do some operations that modify the parameters in the caller scope, but
some modifications are still local. Explained through pointers, this is far more
simple: you have this address on an object, you can modify what the address
point at but any operations modifying the address itself act only at the local
scope.

To illustrate this last point (still in C), let's write a function that swap two
pointers, not the content pointed to but really the address:

```c
void swap_pointer(int **a, int **b)
{
  int *c = *a;
  *a = *b;
  *b = c;
}
```

We just add a level of stars, this way we can change the address sine we have a
pointer to a location containing an address. The only differences are on type,
	the rest of the swap is similar.

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
    example provides a flag `-Weverything` that activates all possible warnings).
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
you can write (but that's naughty) `i[array]` !

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

**Always use half-open range with left bound included and right bound excluded.**

### What's the difference between arrays and pointers ? ###

In most a lot of cases, there's almost no difference between arrays and
pointers, in the sens that a pointer to a memory area containing several objects
of the same type can be use as an array.

But, arrays (or static arrays) in C have a different nature, if you declare a
variable that way (at local or global scope, it doesn't matter):

```c
int array[8];
```
`array` is not a pointer, in fact what you've just created is a block of 8
integers. **But** in most cases, if you use the variable `array` it will be
replaced by the address of the first element, that is `&array[0]`. Thus, you can
pass it to a function expecting a pointer to an int, you can assign a pointer with
it, but, and this is important, it's not a left-value, it can't be reassigned.

Array variables are not replaced by the address of the first element in cases
where they are used for their types like when passing them to the operator
`sizeof` (which will then return the size of the array in bytes) or when taking
their address. Here are some examples illustrating the difference:

```c
#include <stdio.h>
#include <stdlib.h>

int main()
{
  // an int a pointer in order to have an address
  int a = 0;
  int *p = &a;
  // a traditional array
  int array[4] = {1,2,3,4};

  // Let's print some sizes
  printf("sizeof (p) = %zu\n", sizeof (p));
  printf("sizeof (array) = %zu\n", sizeof (array));

  return 0;
}
```

Compile and run:

```
shell> cc -Wall -Wextra -std=c11 -o array array.c
shell> ./array 
sizeof (p) = 8
sizeof (array) = 16
```

OK, you can see that the size of `array` is as expected 16 (4 times 4) and `p`
being a pointer it is 8 (I'm on a 64bits machine).

C99 introduces the notion of VLA (Variable Length Arrays). They are not strictly
speaking dynamic arrays, but arrays whose size will be known at run-time. But
once declared, they behave exactly as other arrays.

### Dynamic ? ###

Arrays is the topic to introduce dynamic memory allocations. Since the beginning, I've
only used address of variables (or context where you didn't know where the
    pointer came from). We also seen arrays, but only static ones.

Sometimes, we need entities that have a longer life than a function scope, a
size that is not compatible with the size of the stack (where our local
    variables go) or simply a size that will only be known after some
computations. For that, we are able to ask for memory using the function
`malloc(3)` from the standard library (the 3 in the parenthesis refers to the
    section in the Unix man pages, not a parameter to `malloc(3)`).

This function takes a number of bytes and returns a pointer to an area of this
size that you can use as you want. Where it is exactly depends on your system
and you standard library, we often say that it *on the heap* by opposition of
the stack (what is exactly the heap ? where `malloc` allocate memory of course ...
    )

`malloc(3)` doesn't know what usage you'll make of this pointer, for that it
returns the *generic type* of pointer, namely `void*` which can be seen as
pointer to nothing or to anything ... You can safely assign a generic pointer to
any pointer variables and you can assign any pointer to a variable of type
`void*`. But, you can't dereference a generic pointer, nor you can do arithmetic
with it.

`malloc(3)` guarantees that the memory area pointed to by its return value will
be your until you free it with `free(3)`, you reallocate it with `realloc(3)` or
the end of the program.

The most common usage of dynamically allocated memory is arrays, strings (which
    are in fact arrays of characters) and structures. You can
allocate memory for a single basic object like an integer, but it's probably a
waste of space ...

Let's go for an example:

```c
#include <stdio.h>
#include <stdlib.h>

int sum(int *begin, int *end)
{
  int res = 0;
  for (; begin != end; ++begin) {
    res += *begin;
  }
  return res;
}

int main()
{
  // a dynamic array of 8 integers
  int *a = malloc(8 * sizeof (int));
  // malloc(3) can fail
  if (a == NULL) {
    fprintf(stderr, "malloc(3) has failed !\n");
    exit(1);
  }
  // Fill the array with 1,2,3,4,5,6,7,8
  for (int i = 0; i < 8; ++i) {
    a[i] = i + 1;
  }
  // Compute and print the sum
  printf("sum of a: %d\n", sum(a, a + 8));
  // Free your memory !
  free(a);
  return 0;
}
```

And we get:

```
shell> cc -Wall -Wextra -std=c11 -o dyn_arrays dyn_arrays.c 
shell> ./dyn_arrays 
sum of a: 36
```

Note the test on the return value, `malloc(3)` can fail, mostly because you
don't have enough memory (or you hit the limit granted to you program). For
various reason, on most modern system, `malloc(3)` succeeds in most cases even
if not enough memory is available, this is because modern system only binds
memory to your program when you concretly access it. But anyway, **always check
the return value of `malloc(3)`**! *(at least in real programs)*

Other functions related to `malloc(3)` are `calloc(3)` and `realloc(3)`.
`calloc(3)` as a different interface, it takes a size and a number of objects,
  and the most important thing, it enforces that the memory is filled by zeros
  (it may also check that the product between the size and the number of objects
   doesn't overflow or is not greater than a possible upper bound).

`realloc(3)` is a little bit interesting, it resizes the memory area. In fact,
  it's not exactly a resize but really reallocation. In the general case, it
  search for an area of the new size, copy the content of the old area into it
  and free the old area. In some cases it may be able to resize in place, or
  fails like `malloc(3)`.

Examples are often better than words:

```c
#include <stdio.h>
#include <stdlib.h>

struct vector
{
  size_t size, capacity;
  int *data;
};

/*
 * init an already allocated vector
 * with initial size and capacity set to 0
 */
void init_vector(struct vector *this)
{
  this->size = 0;
  this->capacity = 0;
  // capacity 0, then no array so far
  this->data = NULL;
}

/*
 * adds an element at the end of the vector
 * resizes it if necessary
 * we choose to double the size each time
 * returns false if something wrong happen
 */
int vector_push_back(struct vector *this, int x)
{
  if (this->size == this->capacity) {
    int *newdata = realloc(this->data, 2 * (this->capacity + 1) * sizeof (int));
    if (newdata == NULL) {
      // realloc is fair enough to not invalidate data in that case
      return 0;
    }
    this->data = newdata;
    this->capacity = 2 * (this->capacity + 1);
  }
  this->data[this->size] = x;
  this->size += 1;
  return 1;
}

/*
 * resets the vector
 * size and capacity go back to 0
 * data is free, thus the vector can be destroy transparently
 * ... or reuse ...
 */
void vector_reset(struct vector *this)
{
  free(this->data);
  // Important: otherwise we may try to reuse the invalid address
  this->data = NULL;
  this->capacity = 0;
  this->size = 0;
}

// Demo
int main()
{
  // no need to allocate dynamic memory for the struct, we use it in place
  struct vector vec;
  init_vector(&vec); // now we can use it
  // Add some element
  for (int i = 1; i <= 10; ++i) {
    if (!vector_push_back(&vec, i)) {
      fprintf(stderr, "something goes wrond !\n");
      vector_reset(&vec); // free the memory for us
      exit(1);
    }
  }
  // Print the content
  for (unsigned i = 0; i < vec.size; ++i) {
    printf("%d ", vec.data[i]);
  }
  printf("\n");
  vector_reset(&vec);
  return 0;
}
```

I let you read and run the code, here are some important details:

* `struct` in C are basic heterogenous *box*, it's just a way to pack together
some values;
* `realloc(3)` and `free(3)` accept null pointers;
* `realloc(3)` when it fails returns `NULL` like `malloc(3)` you need to catch
the result before replacing your pointer, otherwise you'll loose it.

I choose a very C++ style here (I know, C++ comes from C, so this C style too).
In particular, the init function doesn't allocate the structure of the vector,
   only the data (and if fact, not even the data since we choose to start with
       an empty vector). This way, we can have a temporary vector on the stack
   without allocating dynamic memory for it, we only have to care about freeing
   the inner pointer once we're done.

## One more ##

There is so many other topics to explore and this post is starting to be long.
I will play with a last example (the choice was hard ... ) to illustrate pointer
manipulations.

Let's first start with the notion of linked list. A linked list is a collection
of values, a little bit like an array but more dynamic. The idea is rather than
store all the value in a contiguous area, you store them in little box connected
by pointers. This way you can easily add new element, insert them anywhere
(without having to move everything like with arrays) ...

I won't describe the whole list operations, but rather show you a little funny
piece of code.

First the definitions of list:

```c
// not, this is not the list, but the cell of the list
struct list
{
  struct list *next; // the next element
  int value;         // the data
};
```

Let's put some context for my example: you have an already built list, and the
pointer to one of the cell. Our task is to detach the cell from the list.
Problem, in order to do that we need to find the cell before our target in order
to detach it. Second problem, if the cell is the first one of the list, we need
to update the variable that contain the entry point of the list otherwise we'll
be in a inconsistent state (and probably loose access to the list).

Here is a first naive version:

```c
void detach(struct list **entry, struct list *cell)
{
  // save the previous element
  struct list *prev = NULL;
  for (struct list *it = *entry; it != cell; it = it->next) {
    prev = it;
  }
  // cell is supposed to be in the list
  // so now, prev is supposed to point to the element before cell
  // or NULL if it was the first one
  if (prev == NULL) {
    *entry = cell->next;
  } else {
    prev->next = cell->next;
  }
}
```

OK, even after removing the comments, this function doesn't look good. Why ?
Mostly because there's a `if` and worst, the code in both branch are very
similar. And there's this story of keeping a pointer to the previous element
that is not defined in all cases. Even if it is a rather small function, it
contains some traps.

How can we solve that ? Thinking more by pointers ! When looking at the final
`if`, we are updating a place containing a pointer, it can be the location
containing the entry point or the `next` field of the previous cell. If we have
a pointer to this location, it can contain the address we want ! Here is the
code, I let you read it as exercise:

```c
void detach(struct list **entry, struct list *c)
{
  for (; *entry != cell; entry = &((*entry)->next))
    continue;
  *entry = cell->next;
}
```

Just a little hint: `&((*entry)->next)` gives you the address of the field
`next` of the list cell pointed to by the address in `entry`.

While I really find this version interesting, there's a better trick in some
cases. This is pretty good illustration of the principle that states that it is
always better to have smart datastructures and simpler code.

The idea is that we can have a fake element at the begining of our list, we call
that a sentinel. The sentinel never moves, it doesn't contain a value (or we
    don't care about it). This way the empty list is no longer a null pointer
but a single element. This also means that when have to detach the head of the
list we have a previous element ! The new code:

```c
void detach(struct list *sentinel, struct list *cell)
{
  for (; sentinel->next != cell; sentinel = sentinel->next)
    continue;
  sentinel->next = cell->next;
}
```

Elegant, isn't it ? In fact, the sentinel play the same role as our double
pointer in the previous version.

## Goodbye ##

There's a lot more to tell, but I think that I've cover the important part of
the subject, at least an important part of the fundation of the subject. I could
have speak about how an allocator works, or how to implement ref-counters or
intrusive data structure (I've already done that in my old blog).

Now it's up to you to practice.

If you're not a C programmer but survived until
here, I hope that you've learn something that could be useful for you and maybe
will you try C ?
