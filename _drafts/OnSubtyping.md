---
Title: Objects and Subtyping
---

Today, I will question some notions at the heart of Object Oriented Programming. It is wide topic, and you may find a lot of resources (web, books ... ) and it is not my intention to close the topic. I just want to lay down my thought. You may found that this post sounds like a _rant against OO polymorphism_ but it isn't my goal.

## Subtyping?

We can define subtyping in a lot of ways, but coming from a type theory background, I wil stick to types. In short, given two types `t1` and `t2`, `t1` is said to be a subtype of `t2`, noted `t1 < t2`, if in any context where a value (or an expression) of type `t2` is expected, providing a value of type `t1` does not break the properties of that context. In other words, any value of type `t1` can safely be viewed as of type `t2`.

By extension of this definition, it means that the set of values of type `t1` is a subset of the set of values of type `t2`.

Subtyping in types systems is a very interesting topic by itself, with good challenges, but that's beyond the scope of this post.

While subtyping can be used outside of the OO world, both topic are often connected. In fact, subtyping is at the heart of OO polymorphism and is often associated with inheritance. The set interpretation of subtyping links it to the famous _is-a_ relationship used in OO design. I will come back on that topic later. But, first, let's discuss the nature of the subtyping based polymorphism.

Polymorphism is the generic term to describe the idea that one can use the same code for values of different types. When I want to discuss the expressiveness of polymorphism, I tend to use two toy functions: `ignore` and `identity`. While they are very basic, they are good examples of what we can do (or not).

`ignore` is a function that ignores its input, we can write it in pseudo C:

```c
void ignore(T x)
{
    return;
}
```

This is a good candidate for polymorphism, you don't want to write the same function for integers, floating point numbers, strings, objects ... Using subtyping, it is possible to do so, provided that your language has a _top type_, that is a type (let's call it `top`) such that for all type `t` we have `t < top`. For example, this is true for classes in Java (all classes are subtypes of `Object`) and for types supporting interface in Go. In C, `void*` can be used as a kind of top types for all pointers type, but in C++ there is no such type.

`identity` is a function that return its input unchanged. We also expect that we can reuse that value with its original type (without a type cast for example). The pseudo code look like this:

```c
T identity(T x)
{
    return x;
}
```

This is an abstraction of the concept of container: we put _something_ in the container and we expect to get it back later. Without surprise, this can not be expressed using subtyping. This is the reason why _generics_ where introduced in Java, for those like me that have done Java before 2004, you may remember their vector class where one had to cast element to their original type when accessing them. ML like polymorphism (or _generics_, or C++ template) can express this as the sytem is able to link the input type and the output of the function.

In OO context, subytping is thus not used for _pure_ genericity (like when designing containers) but to model the idea that instances of the different classes may share a common interface, like in this naive example (C++):

```c++
size_t size_of(const SizeableType& obj)
{
    return obj.size();
}
```

Where `SizeableType` defines an interface like this:

```c++
class SizeableType
{
public:
    virtual size_t size() const = 0;
};
```

Subtyping expresses the idea that we don't need to know all the details of `obj`, only that it provides a method `size` (with the proper signature) so that we can use it in that context.

Subtyping can be designed in the language in multiple ways. The _class_ -ical way is often called name based subtyping: when classes are defined they explicitly tell the names of the classes or interfaces they extend or implement. But, some languages provide structural subtyping where a type is a subtype of an interface as long as it provides the required members. This is the case in Go or OCaml for example. In fact, in OCaml, you don't even need to define an interface, the interface is infered from the context, the `size_of` function will look like this:

```ocaml
let size_of obj = obj#size;;
```

Which has type:

```ocaml
val size_of : < size : 'a; .. > -> 'a = <fun>
```

This type describes objects providing a method `size` without argument that returns something of a generic type `'a`.

Subtyping can also be _dynamic_ or _static_. All the previous examples describes cases of _dynamic_ subtyping, we define a function that will be translated in a single callable piece of code without regards to the input type. But languages supporting templates can provide a static form of subtyping. Templates alone do not really define subtyping, without type-traits or concepts, the validation is done by instantiating the template and see if it is well typed.

### Subtyping for function types

Before going back to the OO world and discuss implementations, let see how we can apply subtyping to function types.

So first, the type `t -> s` is the type of functions that if given a value of type `t` will return a value of type `s`. So, what does it mean to say `t1 -> s1 < t2 -> s2` ?

We first need to think a bit more the context where you may need to apply subtyping on function. Let say we are typing an expression like this:

```c++
    y = f(x);
```

This context defines a function type that `f` must have to be applied here as `x` and `y` should have been defined with a specific type. For `f` to be used in this context, it needs to accept `x` and returns something that can be assigned to `y` (we will ignore possible conversions as it's not the topic). If we have these type information: `x : t2`, `y : s2` and `f: t1 -> s1`, what we need to verify is that `t1 -> s1 < t2 -> s2`. For the return type, the conclusion is pretty straightforward, we must have `s1 < s2`.

But for `t1`, we know that `f` must accept any value that `x` can have, that is any value that has type `t2`, thus, it `t1` must be **larger** than `t2`, that is: `t2 < t1` !

This is **covariance** (`s1 < s2`) and **contravariance** (`t2 < t1`).

But, enough of type theory, let's look at how we can support (dynamic) subtyping.

## Implementations of subtyping

When I wrote the C++ example, I used the keyword `virtual` in my interface. This keyword indicates that we want to use _late binding_. Late binding is another term for _message passing_. In this case, when calling a method, rather than calling a predefined function, the object will provides the requested function. In most compiled languages, this is done by associating a table of function pointers to the object, called _vtable_ is C++.

I will not go into the details of how those tables work, as it depends on the language and on the presence of multiple inheritance or interfaces. There is plenty of resources on how to implement objects in C if you want to start to learn more on the topic.

In fact, calling the good function in case of subtyping is not the main issue. Let's look at a naive example:

```c++

class Point
{
public:
    Point(int x_, int y_) : x(x_), y(y_) {}
    virtual ~Point() = default;
    virtual int getX() const { return x; }
    virtual int getY() const { return y; }
private:
    int x = 0;
    int y = 0;
};

class ColoredPoint : public Point
{
public:
    ColoredPoint(int x, int y, std::string col) : Point(x, y), color(std::move(col)) {}
private:
    std::string color;
};
```

The virtual methods here a not really interesting (eventually the destructor), what is more interesting is the layout of the data and the size of the resulting object. You don't need to know much about C++ internal to understand that objects of class `Point` do no have the same size as objects of class `ColoredPoint`. So, I wrote a function like this:

```c++
int getX(Point p) { return p.getX(); }
```

And want to pass an instance of `ColoredPoint`, there may be problem, in this case a problem known as _slicing_. It's easy to get it, if I pass an instance by value, the object will copied into the scope of the called function, but since this function only know about `Point`, and thus it only has room for a `Point` (we are already fortunate enough so that the copy does not overflow the target scope).

And if you consider interfaces, this is even worst as an interface has no data members, and thus probably only the space for the vtable pointer.

How do we solve that problem? We use pointers (or references) !

So the function becomes:

```c++
int getX(const Point& p) { return p.getX(); }
// or
int getX(const Point* p) { return p->getX(); }
```

In fact all (almost all?) languages providing a form of dynamic polymorphism use pointers for that. If you've done some Java, you may have seen the infamous `java.lang.NullPointerException`, you may get this because in Java, all you have is pointers to objects.

One of the consequences of this _everything is pointer_ idiom, is the infinite debates around _call by value_ vs _call by address_. Those languages use _call by value_, **but** all their values are pointers.

## Impacts

As we have seen, you need pointers in order to support dynamic subtyping. In some cases, like in the previous example, this is not really a problem, but that's not always the case.

I will take a classical OO design example to illustrate the consequences on the code. Note that none of I will talked about is really a problem, we have solutions for all those points, but it complexifies the code and addd protential bugs.

So, let's say we want to compose a class with an internal member based on an interface. I will use the infamous car and engine example for that. I keep the code to the bare minimum:

```c++
class EngineInterface { /* some pure virtual methods */ };
class Car
{
    // Some code specific to cars
private:
    // internal engine
    EngineInterface* engine;
};
```

We have some questions lying around:

* How do we populate our `engine` pointer?
* What is the life time of this pointer?
* Who is the owner of the pointer?

At first, the first question is specific to your design. So, how do we handle the life time of `engine`? In this example, it seems obvious that engine life is bounded to the car (but it may not be the case, you may be able to replace the engine and reuse the old one somewhere else, who knows.) This answer the thirs question somehow, if the life of `engine` is bounded to the life of the car, the owner is `Car` instance.

As a modern C++ programmer, I want to rewrite the definition like this:

```c++
    std::unique_ptr<EngineInterface> engine;
```

That way we don't have to care about memory management of `engine`.

Note that it's the easy case. `engine` may have a life of its own, or maybe (not suited for the car/engine example), it's a reference to a more global entity shared by many `Car` instances. Those cases yields more questions on the management of the pointer. Do you see why all those dynamic languages come with a garbage collector?

But, now what if I want to be able to copy an instance of `Car` ? (I know examples where classes represent real life objects suck)

For good and obvious reasons, `std::unique_ptr<>` is uncopiable. Any way, we want a copy of the pointee, not of the pointer. So, we need to implement our copy constructor for `Car`, but how do we copy the engine ? We don't have its real type. If we want that, `EngineInterface` must provide a _clone_ method and this method must be specific to each implementation of the interfaces.

As I said, we have solutions for all those questions, but is subtyping worce the price?

## Do you need subtyping?

Recently, we had discussions at work about _trying to have simpler code_ in order to ease the on-boarding of new commers in the team (in particular junior engineers). We all agree that it's a needed move, our code base is ten years old, it has seen several engineers, and like any similar projects it contains obscure areas. The problem with that kind of discussions is that nobody agrees on what is simple. One of the points raised by some, was that we should rely on _inheritance_ a bit more, as most devs understand the concept.

The previous section is enough for me to refute this assumption. Relying on subtyping opens a lot of questions and adds a lot of constraints. Pointers management is only one out of many.

We didn't talk about OO design, but in general, building a class hierarchy requires a carefull design from bottom to top. Remember the virtual destructor in my `Point` example? A class can not be used as a base class without it, and that's one out of many constraints that you need to satisfy.

But the good question is: what does subtyping brings that we can not do without ?

Let's review some use cases.

### The easy cases

One classic usage of subtyping is polymorphic functions, _i.e._ functions that take as input an object implementing some interface.

I go back to my `size_of` function:

```c++
// we can use a reference here, it works the same as a pointer in that context
size_t size_of(const SizeableType& obj)
{
    return obj.size();
}
```

What is nice with this kind of functions is that despite the pointer/reference needs, you don't have all the burden of the pointer management as no pointers are stored, so you can use it that way:

```c++
   SomeSizeableClass obj{};
   auto sz = size_of(obj);
```

But this has several drawbacks:

* `SomeSizeableClass` needs to explicitly implements `SizeableType`, thus you can not use classes that provide a `size` member out of the box.
* Even if `size()` is elligible to inlining, the compiler won't do it as it needs to respect the late binding contract and select the function to call at run-time.

But, you know what, we're in C++, we have templates! The following generic is as generic as as our interface based one but without the drawbacks:

```c++
template<typename SizeableType>
size_t size_of(const SizeableType& obj)
{
    return obj.size();
}
```

With a bit of work on type traits, or with C++20 concepts we can provide better type checking than _template instantiation errors_ and thus better error message.

### Composition patterns

The previous exemple with the car and the engine is a classic composition pattern (it's not the name of an official design pattern). It solves a classical design issue: how to build objects with some inner elements (usually impacting it's behavior) selected at run-time.

Examples relying on _real life objects_ are often misleading. In that case, we may see the various engine as instances of different _models_ of engine, but in practice properties could give us the control on behavior that we expect.

So, a better example would be to consider the **Composite** pattern. Let's define a tiny example of Abstract Syntax Tree (AST):

```c++
namespace ast {
// The common interface
struct Node
{
    virtual ~Node() = default;
};

// binary operators
struct Operator : public Node
{
    Node*       lhs;
    Node*       rhs;
    std::string operator;
};

// values
struct Integer : public Node
{
    int value;
};
};
```

Here the need for pointers comes as much from the subtyping relations than the fact that `Operator` nodes can be composed from operators or integers. Of course, as we are in a too simple case, we could have define all in one node type, but you get the idea.

The next step is probably to apply the _Visitor_ pattern to implement evaluation or pretty printing. Here is an example of pretty printing (full code):

```c++
#include <iostream>
namespace ast {

struct Operator;
struct Integer;

struct VisitorInterface
{
    virtual void visit(Operator*) = 0;
    virtual void visit(Integer*)  = 0;
};

// The common interface
struct Node
{
    virtual ~Node()                        = default;
    virtual void accept(VisitorInterface*) = 0;
};

struct Operator : public Node
{
    Node*       lhs;
    Node*       rhs;
    std::string op;

    void accept(VisitorInterface* visitor) override
    {
        // let the visitor take responsibility of traversal
        visitor->visit(this);
    }
};

struct Integer : public Node
{
    int value;

    void accept(VisitorInterface* visitor) override
    {
        visitor->visit(this);
    }
};

struct Printer : public VisitorInterface
{
    void visit(Integer* i) override
    {
        std::cout << i->value;
    }
    void visit(Operator* op) override
    {
        std::cout << "(";
        op->lhs->accept(this);
        std::cout << op->op;
        op->rhs->accept(this);
        std::cout << ")";
    }
};
};
```

There are several issues with that code. In particular, I'm using raw pointers why I should have used `std::unique_ptr` (or some other kind of smart pointers) in order to ease the memory management.

We don't need an interface for the visitor. It is only required to provide a generic version of `::accept()` in all kind of nodes. But it's a case of polymorphic function and we can implement it using a template as we should always know the concrete type of the visitor used. You can even go a bit further, as we choose to leave the traversal responsibility to the visitor, the `accept` override of all sub-nodes are all the same, we should be able to implement it using the Curiously Recurring Template Pattern in `Node` directly (this can be a bit tricky as we still need a unique type representing all kind of nodes in the operator).

And if you really want to get rid of subtyping, you can use `std::variant` (left as an exercise for the reader).

```c++
#include <iostream>
#include <memory>
#include <variant>
namespace ast {

struct Operator;
struct Integer;

using Node = std::variant<Operator, Integer>;

struct Operator
{
    std::unique_ptr<Node> lhs;
    std::unique_ptr<Node> rhs;
    std::string           op;

    Operator(std::unique_ptr<Node> lhs_, std::unique_ptr<Node> rhs_, std::string op_) :
      lhs(std::move(lhs_)), rhs(std::move(rhs_)), op(std::move(op_))
    {}
};

struct Integer
{
    Integer(int v) : value(v) {}
    int value;
};

struct Printer
{
    void operator()(Integer& i)
    {
        std::cout << i.value;
    }
    void operator()(Operator& op)
    {
        std::cout << "(";
        std::visit(*this, *op.lhs);
        std::cout << op.op;
        std::visit(*this, *op.rhs);
        std::cout << ")";
    }
};

// Build example
Node example()
{
    return Operator { std::make_unique<Node>(std::in_place_type_t<Operator>(),
                                             std::make_unique<Node>(3),
                                             std::make_unique<Node>(4),
                                             "+"),
                      std::make_unique<Node>(6),
                      "*" };
}
};
```

It is interesting to note that the code is simpler without subtyping. Of course, part of the job is hidden inside `std::variant` and `std::visit`, but first they belong to the standard, and second what they provide in that example is not so complex and can be easily implemented (not that variant are so simple, but we don't need all the aspects nor the genericity).

But the most important aspect is that we remove a lot of boilerplate code (all the `::accept` implementations). Now, adding a new kind of node only requires to descripe the node content (and of course the visitor code). We also gain some flexibility, while we still need pointers for the recursive structure, the root of our AST can be a value and still be seen as generic `Node`.

## And, so?

As I said in the introduction, I wanted to lay down some reflexions on the topic.

The most important point that I wanted to show is that using the OO polymorphim (subtyping) adds constraints on you code and you should integrate those aspect in you design. My personal opinion is that you should always question the necessity of relying on those constructions. Maybe the flexibility is not needed at run-time and you can get the same genericity by using templates, you should also consider if you can rely on a single entity with the proper parameters.

I have chosen to not talk about performances, while it's a real topic, I see it as a fragile argument and probably the least problem here.

One final question is the relation between inheritance and subtyping. In almost all OO languages, inheritance is strongly linked with subtyping. This is a legacy mistake. If you dig a bit into the type theory, you will find that doing so may break important properties of type system. This is a known problem. I remember my typing teacher telling us that it was a choice as it seems more convenient and the cases are tricky. But, since then, people have pointed out that even from a design perspective this is a mistake.

Some languages don't force you into this paradigm. In OCaml for example, subtyping is not linked to inheritance. In C++, as long as you don't use `virtual`, you can use inheritance without subtyping as it is done in policy based design, mixins or CRTP.
