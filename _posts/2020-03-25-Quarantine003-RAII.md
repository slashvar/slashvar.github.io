---
title: Quarantine C++ - RAII
---

For today's post, I'm back to C++ basic with the famous Resources Acquisition Is Initialisation (RAII). Like the two previous, it was written in less than 30 min.

## Destructor

In C++, any entity that goes out of scope get destroyed properly. For immediate values (integers, floats, pointers ...) or POD (plain old data) this has basically no special consequences. But objects may provide a destructor.

Let start with a tiny demo to understand what we are talking about:

```c++
#include <iostream>

struct Demo
{
    // The constructor
    Demo()
    {
        std::cout << "calling ctor\n";
    }

    // The destructor
    ~Demo()
    {
        std::cout << "calling dtor\n";
    }
};


int main()
{
    Demo d;
}
```

When running we get:

```
shell> clang++ -Wall -Wextra -std=c++17 -O3 -o dtor dtor.cc
shell> ./dtor
calling ctor
calling dtor
```

You can track all the cases of destruction, but most of them are obvious. Those that are easily forgotten are: destruction of unamed objects and destruction of copy generated when calling function.

# The power of the destructor

The purpose of RAII idiom is to take advantage of the pair constructor/destructor to handle resources properly. The pattern is pretty straightforward, you acquire the resource in the constructor and let the destructor takes care of properly releasing it.

One of the main advantage of this idiom is that, unless the program just dies, you are certain that the destructor will be called and thus you resources properly released. Even, if an exception is raised, the destructor will be called. This is exactly what you want for resource handling and completely transparent.

```c++
#include <iostream>

struct Demo
{
    Demo() = default;
    ~Demo()
    {
        std::cout << "dtor called\n";
    }
};

struct Exn
{};

int main()
{
    try {
        Demo demo;
        throw Exn {};
    } catch (const Exn& e) {}
}
```

The STL is using this idiom in a lot of places, you can see it in action in the `<fstream>` header. Another good example is `std::scoped_lock`, when creating it, it locks the real lock provided at construction and then unlock it when leaving.

## Building a very simple smart pointer

Among the most useful things we can build with RAII are smart pointers. We will try to build a very simple and naive variation of `std::unique_ptr`. Let's set up the scene with the easy part:

```c++
template <typename T>
class my_pointer
{
public:
    my_pointer() = default;
    // Construct by copy of the original value
    my_pointer(const T& val) : data_(new T(val)) {}
    ~my_pointer()
    {
        if (data_) {
            delete data_;
        }
    }

    T& operator*()
    {
        return *data_;
    }

    const T& operator*() const
    {
        return *data_;
    }

    T* operator->()
    {
        return data_;
    }

    const T* operator->() const
    {
        return data_;
    }

    void reset()
    {
        if (data_) {
            delete data_;
        }
        data_ = nullptr;
    }

private:
    T* data_ = nullptr;
};
```

I won't go into the overload part (`operator*` and `operator->`), read a fine documentation.

The RAII idiom seems pretty obvious from the constructor (the one that take a value and perform a call to `new`) and the destructor.

But we have a problem: what happen if we do a copy construction of our smart pointer, what about move ? This code fails to one of the most important rule of C++. This rules can be summerized in: define all constructors and destructors, or defines none.

What is the problem here ? If you keep the default copy constructor, once copied, you have two object with the same pointer and after one of them is destroyed, the other one has a broken pointer. You have the same problem with the default move constructor. Since we are implementing a unique smarter pointer, we don't want copy constructor (nor copy assignement). But we still need the move counterpart.

```c++
    my_pointer(const my_pointer&) = delete;
    my_pointer& operator=(const my_pointer&) = delete;

    my_pointer(my_pointer&& other) : data_(other.data_)
    {
        // other must not keep the pointer, otherwise we may face use after free
        other.data_ = nullptr;
    }

    my_pointer& operator=(my_pointer&& other)
    {
        if (data_) {
            // don't forget to delete existing data
            delete data_;
        }
        data_       = other.data_;
        other.data_ = nullptr;
        return *this;
    }
```

Handling copy and move operations is mandatory for RAII objects. You can see it as a way to define the life cycle of your resource: you define how to create it, how to release it and how to share it.

The `std::vector` class is showing another interesting application of RAII. The vector (and its content) are destroyed and the allocated memory release when the object get destroyed. But unlike our smart pointer, the copy will perform a deep copy of the underlying array. On the other hand, the move operations are transmitting ownership of the array to the new vector while resetting the source.

## Good bye

This is it, it's time to end this third quarantine post.
