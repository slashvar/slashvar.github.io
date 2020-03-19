---
Title: Pointers, references, iterators, views ...
---

I wanted to write a bit about `std::string_views` and realize that talking about C++ handles in general could be a good idea. It was also motivated by two posts by Herb Sutter about move and references (see [] and []).

## For the lazy readers

Here are my reinterpreted guidelines about values, pointers and references:

* Passing non immediate values for read-only purpose should always use `const` references.
* Value semantics (pass by value, returning objects ... ) is safe and not that expensive.
* If you want to give ownership of an object, use `std::unique_ptr`.
* If you want to share ownership or if memory management is a bit tricky, use `std::shared_ptr`.
* Passing object as in/out parameters, use references.
* Use raw pointers when the object may not exist or when the callee may store it address without taking ownership.
* Avoid as much as you can C-style array as pointer.
* Avoid as much as you can storing references.

## Passing by value or by address ?

In the history of Computer Science, there have been a lot of discussions and proposal about how to pass arguments to function, but in practice only two approaches are really used: pass by value and pass by address/reference (FP fans in the back, I've heard you screaming call by need). And to be honest, I often consider call by reference as a call by value of pointer in disguised. But let's revisit these notions.

Consider the following function:

```c++
int f(int x)
{
    return x + 1;
}
```

When we call it, we pass the argument value by copie, this is the traditional call by value model. This copy represents no special issue, as integers are the most simple data you can manipulate. Of course, if you need to modify the value of the argument outside of `f`, you will need a call by address (using pointer or reference), like in the following function:

```c++
void swap(int& a, int& b)
{
    int c = a;
    a = b;
    b = c;
}
```

Pretty simple, isn't it ?

If we were talking about C or Pascal, we could stop here as there's really nothing else to talk about. But, of course, in C++ things can get a little bit more complex. Working with _POD_ objects won't make any difference (just like it works with structs in C), the real problems start when your objects contains _indirect_ data and/or have specific constructors and destructors.

First, consider this dummy string class:

```c++
/*
 * an incomplete (broken) string imlem
 */
struct My_string
{
    My_string() = default;
    My_string(size_t n) : data_(new char[n]), size_(n) {}
    ~My_string()
    {
        if (data_ != nullptr) {
            delete[] data_;
        }
    }

    char* data_ = nullptr;
    size_t size_ = 0;
};
```

Note: this class is incomplete, it's just an illustration.

If we pass an instance of this class by copy, only the immediat content will be copied, that is the size and the pointer to the data. The copy is no longer a real copy as in my previous example, if I modify the content pointed by `data_`, it impacts the original object, just like if it was a reference. **This is a pointer semantics**.

But this example contains a dangerous memory issue. When such an object is passed by copy, the new instance (the copy) will get destroyed when leaving the function, invoking the destructor. This is the classic case of the rule of ~three~ five. If you never heard of this, all you have to understand here is that anytime an object (not a pointer nor a reference to an object) goes out-of-scope (and thus reach its end-of-life), its destructor is invoked. When passing an object by value, it is first copied to scop of the called function and when leaving that function it get destroyed. Let's see with an example what that means for our class:

```C++
void foo(My_string str)
{
    // do something
}

int main()
{
    My_string str(1);
    // valid we init data_ with an array of one char
    str.data_[0] = 'A';
    foo(str);
    // Use after free !
    // str.data_ has been deleted
    str.data_[0] = 'B';
}
```

You can try that code with sanitizer activated, it will explain you the problem.

How do we solve that ? Of course, it depends on your goals, the common solutions are:

* Forbid the copy;
* Adopt a real _value semantics_ for your object;
* Try to implement a safe way to share the ownership of `data_`.

_Value semantics_ is probaly the simplest choice (from an abstraction point of view). So what we want to do is the copy to act as the copy of a POD, copying the whole data, in our case it means copying the content of `data_` into a new array. By the end of the day what we get is the same semantics as when we passed an integer or a POD.

The value semantics is the default choice for STL containers (`std::string`, `std::vector` ... ) as it is the most convenient while being pretty straightforward to implement. The pointer semantics can be regain in a lot of cases by using good old pointer or reference.

## Usage of references

Value semantics works for a lot of cases, but it's not always what we want. References (or pointers) are here for that. Here is stupid function on string:

```C++
void up_the_a(std::string& str)
{
    for (auto& c : str) {
        if (c == 'a') {
            c = 'A';
        }
    }
}

This function needs to modify the string in place, otherwise it is even more useless. So, we can pass it by reference. Note that we are also using reference for the range variable of the for-loop, in the same spirit.

Modification is not the only case where we don't want (at least don't need) copy.

```C++
size_t count_a(const std::string& str)
{
    size_t count = 0;
    for (const auto& c : str) {
        if (c == 'a' or c == 'A') {
            count += 1;
        }
    }
    return count;
}
```

In that case, we don't need to modify the string, but if could avoid the cost of the copy it would be great. In that case, we can safely use `const` reference. The `const` qualifier indicate, of course, that we won't modify the string, but it also brings another usefull aspect.

Let's go back to my first function `up_the_a`. In order to pass a reference, we should be able to take a reference on the object, and we can only do that on _left values_ (historically, they are called left values because they may appear on the left side of an assignment operator). This is pretty obvious as if we want to modify the string, this string needs to live somewhere.

But what happens in the case of `count_a` ? Maybe we want to call it on a temporary string constructed at tha call point, do we need to give it a lifetime (using a variable) ? Or are we doomed to use only call by value ?

Fortunately, `const&` (convenient shortcut for `const` qualified references) works on _right values_. So calling `count_a(std::string("aaaa"))` is valid.

## What about move semantics ?

Move semantics has been introduced with C++11. Let's start with an example:

```
struct StringBox
{
    StringBox(std::string str) : str_(str) {}
    std::string str_;
};

int main()
{
    StringBox box(std::string("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
}
```

When calling the constructor of `StringBox`, we're building a temporary string (long enough to avoid the short string optimization) and then we copy it to the constructor scope. In fact the string is copied two times, for nothing. What we wanted to do is to allocate one storage and give the ownership of this storage to the newly constructed box object.

We can avoid one copy by passing the constructor as `const&`, but we still have a copy.

Since C++11, we can use move for that:

```
StringBox::StringBox(std::string&& str) : str_(std::move(str)) {}
```

The `&&` stands (here) for _rvalue reference_. It states that we want to move a right value in our scope rather than copying or referencing it. The `std::move(str)` used in initialization will tell the compiler to consider `str` as a right value, even if it is a variable.

What does it change ? It will call a different constructor, the move constructor. By default, this constructor behaves like a copy constructor but unlike the copy constructor it doesn't require to have a `const` input and thus can alter the original object (examples of such constructors later).

The move constructor of a string (or a vector) will _move_ the data from the original string to the constructed one (it copies the immediate content and then reset the original as if it where empty). This will avoid the copy of the content but without the issue of my first example (the original string no longer owns the data).

## Why views ?

So, strings (and vectors) enjoy a useful value semantics and when we need a pointer, we should use ... a pointer ! Simple, isn't it ? In fact, we will prefer a reference.

Let's illustrate that with a dummy example:

```C++
size_t count_a(const std::string& str)
{
    size_t count = 0;
    for (const auto& c : str) {
        if (c == 'a' or c == 'A') {
            count += 1;
        }
    }
    return count;
}
```

We don't need (and we don't want) to copy the string passed to this function, so we use constant reference (`const std::string&`), the data is not copied **and** we guarantee that we won't alter the content.

Now, I want to write a function that exploit the previous function (counting all occurrences of `a`, whithout regards to the case) and count the number of `a` after the first space in the string.

```c++
size_t count_a_after_space(const std::string& str)
{
    // Search for the first space
    auto cur = begin(str);
    for (; cur != end(str) && *cur != ' '; ++cur) {
        continue;
    }
    if (cur != end) {
        // We found a space before the end of the string
        // We need a new string object to be able to call count_a
        std::string tmp(cur, end);
        return count_a(tmp);
    }
    return 0;
}
```

So, we avoid copy in `count_a` but we need to create a new string (and thus copy the data) in order to call our function, so we pay for an extra useless copy. Of course, we can solve the problem by overloading `count_a` with a version that takes iterators. The code is a bit different as we no longer have a `std::string` object. This version can be enhanced by templating over the iterator types. This is the original STL approach.
