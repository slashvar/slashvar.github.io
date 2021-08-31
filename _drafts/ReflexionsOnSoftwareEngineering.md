---
Title: About Software Engineering
---

I was reading [Goals and priorities for C++](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2137r0.html) amd I was impressed on how this aligned with my vision, not only about C++, but on my job. Since I've moved out of the academic world, I have been questioning several aspects that are not just technical, without being part of the project management. This post is a snapshot of where I stand today. As it is triggered by some C++ readings, some parts will be C++ oriented, but I hope I will be more generic.

## Utopia and No-future

When I was a student, we've been presented with what was supposed to be top of the art software engineering methodologies. We were in the 1990's, and that time the key point was still about doing perfect design with sufficient descriptions so that programmers will just have to translate it.

This is what I call **utopia**, a vision of software engineering where we should be able to think of every possible aspects of the software we are building and from there anticipate all the difficulties. Theoretically, with such perfect designs, projects must succeed!

Of course we all know now that it does not work. It may work for a toy project, with an horizon of a few weeks. What are the problems?

* With large projects, exploring at scale all aspects in sufficiently effective manner is untractable;
* Requirements evolve, ideas that sounded good at the beginning may finally not satisfy the user expectations;
* Technical considerations strike back in the design, this is probably the biggest failure of the software engineering approach of the 1980's and 1990's.

The opposite approach, is usually to _code and see what happen_. This is what I call **no-future**. Basically, you peak a short term goal and quickly draw the requirements and implement it without further considerations. This has a lot of interesting aspects, as you can validate (or invalidate) ideas pretty soon, you can expose your software to users, gather feedback and even make money out of it faster than the utopia model would have let you do. Of course, this approach has a price:

* Early versions may be less than satisfactory to users and lead to useless negative feedback;
* In the long term, your project may cumulate **technical debt**;
* To avoid tech debt, you may spent more time in refactoring, missing opportunities to bring new features.

So, are we doomed, stuck between **utopia** and **no-future**? The error would be to consider that the solution lies as in a compromised between the two approaches. My opinion is that what is missing in both cases is the notion of evolution. Once you accept that there will never be such thing as a final version, and that you can not know today what will be the next step, you need to integrate the ability to change.

## Code with the door open

> The belief that nothing changes comes either from poor eyesight or bad faith. the first one corrects itself, the second one fights. -- Friedrich Nietzsche

I believe that the best practice that a software engineer should apply is to build code that is open to change. This is a difficult task, but we can find ways to go toward that goal.

### Separation of concerns

This is probably one of the most common and useful topic. You should try to organize your code and your design in small components with a single responsibility.

The best advice I have heard on this topic is about functions: **if you need more than one simple sentence to describe the purpose of you function, you need to split it**.

The idea is that each component having a clear responsibility, you can drive changes by modifying the component or even replacing it. When responsibility is not clear and spread in multiple components, changes are harder to achieve as you need to track every places where it may be impacted. On the other hand, when a component mixes multiple responsibilities, you will need to identify what should change and carefully anticipate the impact on the other aspects.

That does not mean that each component should one simple thing, but its purpose needs to be a logical single element. If this requires several actions, those actions are implementations details of the logical goal. For example, in order to _normalize_ a textual query, you need to transform it to lower case, eventually remove diacritics, replace words with their root form ... All these steps are part of the logical action of normalization, but the component single responsibility is to normalize.

### Objects and free functions

In OO languages, the notion of objects (or classes) adds an extra layer of organization for the code. But, if the language permits it, we can still use free functions (functions that do not belongs to a class). While free functions can be helpful in writing code that support changes, they also can be a problem.

The first point to look at is how the interface of function describes its purpose. If the function name should be a _verb_, its arguments should be the complement of the verb and the whole interface should read as a good description of the function. Take for example a sort function, the interface should be something like `sort(range)` (where range is either an array, a pair of iterators or whatever describes a range over a sortable container), this says all you need to know about the function purpose, it even sounds like providing a comment will be useless.

Unfortunately, that's not enough. You should then consider how the function interact with its arguments and the context of the caller. The caller should be able to replace the function with any function that takes the same parameters and is supposed to perform the same action. In the sort case, the sort function does not need to know how the range provided can be grown or shrink, and the caller does not need to know what sort algorithm is used.

Another point is about _private_ free functions, that is free functions that only exists in a single translation unit (TU). They are often tools for the code in the TU and it seems logical to not expose them outside. But there's a problem lying behind this assertion. If the function is only used by methods of the same class, acting on components of that class, it means that it belongs to the class.

The last point has more impact than what you may think. Recently, I was adding some information to the result of queries in our search engine, and in the query execution implementation there was a special free function used only to build the answer in cases of no results. What happen is that it was missing some of the components I needed to add this new field. And in fact, among all its parameters, most of them where members of the class where the function was used. What happen is that it started as a free function because, it was looking as if it does not need to be part of the class, then each time something was missing someone added new parameters. Refactoring thus required to pass more and more context, updating call points, taking care of the way context was passed (reference, `const` or not in some cases). Moving the free function in the class solved not only the missing context issue but also removed hard dependencies on the type of various members of the class. It also solved some trouble linked to the way parameters was passed.

Usually, my line of thought is to see things that way:

* Is the function will only be used by members of that class? if yes, then it is a member of the class as it participates to its internal behavior.
* If it may be used (even if it's not the case) by other components outside, then why is it restricted to the current TU? Shouldn't it belong to some toolbox?

## Bugs, failures and errors

We all know that even if we try hard, our code will never be perfect. And in fact, there are sources of errors out of our control. So, even with a language protecting you from run-time errors, your program may crash one way or another.

Separation of concerns and in general well split code make tests and bug tracking easier. You can expect that most obvious programming problems errors can be eliminated during the dev process. Integration tests (end-to-end, production simulation, staging env ...) can catch another part of issues.

But, problems remain and in the end, programs will still crash in production. _Are we doomed?_ Maybe not!

I believe than instead of chasing the impossible bug free programs, what we need are programs that handle crashes properly. Handling crashes properly depends on the target of your code. I will focus on what I know most: services. For direct end-user applications, similar ideas apply but the constraints are a bit different.

First, you need to define your priorities. Even if you don't have availability constraints (like SLA), your application is excepted to provided some services and some are more important that others.

For example, at Algolia, search is the most important component. We expect that even with large incident, our users are able to search. So, we put priority on search at the expense of content indexing. It does not mean that we may loose data, but just that updates may take more time due the incident.

But, in some other scenarios, updates may be more important. Imagine, you are working on an application that receives information from some probe in outer-space, the transmission is expensive and you are the receiving end. You are also supposed to process the data so they can be used by other applications. Whatever happens, processing can be done later as long as the data are available.

These constraints impact your application design:

* Raw incoming data must be written down in a safe way
* Processing should be done async on a working copy of the original data

The first stage must be simple and direct, don't take the risk to add logic, just write down data to some replicated storage. If processing fails, you can run it again, but there won't be any way to recover the lost data.

Sometimes, you must trend performances for failure resilience. If your are performing long computation that may take hours, days or even months, you should consider a form of incremental algorithm where you are able to restart from a backup state at the expense of slower computation.

The next important step is how do you crash. Modern platforms (languages, systems ... ) let you catch unexpected events. What is important is to log information so you will be able to analyse the problem **later**. Usually, services should be designed in such a way that they can restart gracefully in most cases, in that case, you should consider keeping crash information (logs, code dumped, metrics) so you can track the issue async.

