---
title: Inheritance and subtyping
---

For a long time, I wanted to talk about Object Oriented Programming, inheritance, subtyping and genericity in
general. This is a wide topic, so I'll start small and where it goes.

## Subtyping

Subtyping is both an OO concept and a technic used in type systems that can be applied outside of the OO world. The
intuition is rather simple, given two types `t1` and `t2`, we say that `t1` is a subtype of `t2`, noted `t1 < t2`,
if you can use any value of type `t1` where a value of type `t2` was expected. This concept is sometimes referred
as inclusion polymorphism, since you can understand `t1 < t2` as the set of values of type `t1` is included in the
set of values of type `t2`.

I won't explore the complexity of adding subtyping into a type system, nor go into the details of covariance and
contravariance, as it is not relevant for the present discussion. I am more interest in the relation between
inheritance and subtyping.

Even if it's not completly accurate, you can think subtyping as an attempt to model in type systems the _is a_
relationship of OO modelisation, and by consequence a model for inheritance. Unfortunately, those relations are
somehow ambiguous. First, there are several _technical_ issues that appear when you introduction inheritance as
subtyping in OO languages. Usually those issues, are considered armless and acceptable compared to the intuition we
have of inheritance. So, from a purely theoretical point of view, type systems of languages like Java are broken by
design.

Now, what about the intuition that subtyping and inheritance are equivalent concepts ? A quick search on the web
will give you way too much articles describing how broken is this relation. Most of those articles will involve the
Liskov Substitution Principle (which is somehow the definition of subtyping) and you can break it while considering
that a subclass is a subtype. One of the key of those examples, is that LSP is going beyond types, and adds
constraints on the program behavior.

I won't go that path, most of those examples and discussions are centered around perfect OO concepts and how to
have proper subtyping, while I believe that subtyping is the issue. Anyway, there is one example that I found
interesting, because you do not need to involve LSP to understand it.

Let's imagine you have an implementation for a double ended queue (a deque). With the proper interface, an deque
can be used where we expect a simple queue, so we can say that a deque _is a_ queue (subtyping). But now, if you
need to implement a queue,  you will probably derive it from the one of the deque (inheritance). As you can see, we
used the two concepts in opposite direction. My point of view is that both are different mechanism, and you can use
inheritance without subtyping and subtyping can exist without inheritance.

## Subtyping (again)

We have seen the potential issues of the relation between subtyping and inheritance. Now, let me show you that
subtyping, in fact, won't solve your problems.

For this example, let say that we have a simplified Java like language, so we don't have to care about `virtual`,
nor the difference between values and pointers and we have an implicit parent for all classes called `Object`.
We want to implement the simplest possible container, a box.

```
class Box
{
    Object content;
    void push(Object x) { content = x; }
    Object get() { return content; }
}
```

So, now we have this container, we want to use it:

```
class A {} // we don't need the details

// we're not in Java after all
int main()
{
    A a = new A();
    Box box = new Box();
    box.put(a);
    A b;
    b = box.get(); // type error!
}
```

You've probably get it, no ? The last statement is ill-typed. Why ? because we've lost the type of object we pushe
in the box and thus if we want to assign to an object of type `A` again, we will need to cast it. This is a
classic. Subtyping is not enough to support true genericity. In fact, adding parametric polymorphism (like generic)
without subtyping is way more accurate than subtyping alone.


