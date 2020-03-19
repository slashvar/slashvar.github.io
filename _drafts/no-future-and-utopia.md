---
Title: No-futur and Utopia
---
**Disclaimer:** despite the title, this is **not** an ideological nor political post. Just to be sure.

Design and development cover a large part of software engineering, my day to day job in essence. Of course, that's not all I do, but it drives most of my activities. As any software engineer, I care about the efficiency of this process. In this post, I'll try to lay down some ideas and reflexions extracted from experience (as software engineer, but also as programming teacher).

## A software engineer dilemma

In order to illustrate my point, I'll use two archetypal images, representing the two classical approaches that we (almost) all talk about. They are a bit excessive, but that's the point, isn't it ? I call them the **No-future** and **Utopia** approaches. They form the two ends of the way we manage projects most of the time.

To set-up the stage, consider that we want to build a new feature. The preliminary works have been done, you have an long term goal and some intermediary milestones with some reasonable scope for each goal. So we know where we going, and we need to start.

Let's now draw our caricatures.

### No-future dev

Working in a fast growing environment, your main goal is to push the first version in production as soon as possible. The no-future approach drives to work as if this first shot is the only one.

This approach has many positive incomes:

* It maximizes the time spent on the short term goal;
* You don't loose time designing aspects that may not be usefull later;
* If the feature is abandonned after the first evaluation, or deeply redesign, the lost is minimal;
* You are more likely to succeed and satisfy your manager expectations.

Of course, there are also drawbacks:

* You will probably rewrite most the code in the next iteration;
* Even if you keep that first implementation, you will accumulate technical dept;
* The time gained in the first iteration will probably be smaller than the time lost in later iterations;
* The project may failed in later step due to all the previous points.

This approach is often the choice in early stage, and thus in the start-up culture and in the web world. In fact, since the technical stack, in some fields, changes faster than you iterate on your projects, rewritting is not really an issue. But it sounds a bit as a no brainer approach.

### Utopia dev

In the traditionnal development model, the goal is to build a perfect design before doing any development. Your goal is to anticipate the future and to work as if only the last version exists.

We have some positive incomes:

* You will build a strong and stable software;
* A good design will help you iterate on sub-components without breaking the whole project;
* The code written won't need rewritting or refactoring other than for enhancement;
* You will be proud of your perfect program.

We have drawbacks too:

* By the time the design is finished, you can forget the first version;
* In fact, the project may be abandonned before the design is ready;
* Any change in the project scope or goals may (will) ruin the design;
* The amount of code needed to implement may result in a monster.

To make justice, there exists successful projects in the history of software engineering that was designed that way. But, let be honest, we are not all Donald Knuth, and adopting this kind of model is very likely to fail.

### Stuck in a greyscale world

For a long time, the world of software design was targetting Utopia. We can try to understand why, but it seems pretty obvious that Utopia is appealing. The goal was not only to build perfect design, but also perfect softwares. The dream was that with a perfect design, the code will flow almost automatically and software bugs could be avoided by design. Nobody found Utopia.

In a more pragmatic approach, most projects tends to live in between those two caricatural models. Usually, we choose a more realistic bound, that is not the next version, but not the perfect unreachable goal.

But, in my opinion, all we have from this is a no-future model with iterations before the first version. We accumulate technical debt and face the same question at each iteration: should we throw away the legacy implementation, refactor it or try to patch it once again.

The fact is that, both models are just two points of view of the same model. They don't address the real issue, which is ability for the code to evolve and mutate, not to satisfy a design. I will try to discuss those points in the rest of this post.

## Modern software cycles

In the last 20/25 years, successful software companies have set-up new standards on the life cycles of softwares. While releases cycles expressed in years was the standard, now we all expect them to be way shorter. For a SaaS (Software as a Service) company, the release cycle is only bound by the deployment speed, at Algolia, we deploy a new version of the engine every week. In the web world, deploying new version on a daily basis is not uncommon.

This has a huge impact on software development, somehow, we are stuck in a no-future model, the cycles are too short for long and fixed designs. This is not always an issue, dynamic languages and rich frameworks, for example, allow *rewrite often* iteration model, where throwing away previous code is not issue. But, even in those model, when projects reach a certain form of maturity, this is no longer a way to go.

In my current job, I work on large piece of C++. The tech stack, the size of the project and its scope, and the amount existing code are not compatible with a no-future approach. But, still, we need to have rapid cycles and a goto production time as as short as possible. In the past, we accumulate a lot of technical debt. The focus was on the product, so everything related to main features are pretty well designed, but around it, the code is dense with a lot of duplication patterns, large piece of similar code that differs only by few details, very long hub-functions ... Recently I was refactoring a key feature, most of the processing was done in a function of more than 3000 lines. This is the symptom of a project that was forced in the no-future model. The team was small (3/4 engineers for a project of 500K lines of code) and rewriting was never an option. Since then, the team is bigger, the rate of new features is lower and we have the time to refactor and reorganize the project, things are getting better.

Are we doomed to reproduce this situation ? The short goto time for new feature brings so much to a project (product-wise at least) that it is now the defacto model for most projects.

But maybe we didn't look at the problem the right way, we were focusing on the fight between immediat results and anticipation. In my opinion, the key is in the way we think code. I'll try to expose some ideas that may (hopefully) help reaching this goal.

## Don't predict the future, design to evolve

The idea is pretty simple, rather than trying to build a perfect design with all the forthcoming changes, we want to build code that can easily evolve. Sound easy, isn't it ?

One of the key problems in the no-future model is that we need to rewrite large part of (if not all) code at each iteration. But if we have code blocks that won't get impacted by design changes, we should be able to minimize the amount of code rewrite and refactoring implied by each change. The benefits will even be multiple, we will have smoother changes, less rewriting and less tech debt, without having to rely on an utopia model.

So, what do we have at hand to achieve this goal ? There are few key concepts, most of them already known and emphasis in the software engineering world. Derived from those concepts, we have technical _tricks_ that can be used to ease our implementation. And of course, it will always pay to have a good engineering discipline.

### Orthogonality

The first concept, and probably the only one, is what is refered as orthogonality (like [1]), separation of concerns, loose coupling ...

The key idea is to build standalone blocks with minimal dependencies or at least only external dependencies (_i.e._ dependencies on iterfaces, not on implementations), Those blocks should be, as much as possible self contained and small.

#### A simple example

Let's pick a simple example. I want to add a logging feature in my application. I need to log well some key operations on the console (that's my first version goal) but later the list of actions will be expanded, I may decide to log on disk, or through a dedicated service. A quick review on the logging process helps identifying some generic requirements: we need some uniform formating, log entries should be tagged with a date and probably a topic and a level (even if v1 only defines one of each).

The no-brainer/no-future design will probably identifies the one or two spots in the code and add a printing statement, doing most formatting in place. Of course we are not that stupid, and we come up with a function like this one:

```c++
/*
 * Assume we have an overload of operator<< for the class Date, Topic and Level
 */
void emit_log(Date date, Topic topic, Level level, const std::string& message)
{
    std::clog << date << ":" << topic << ":" << level << ":" << message << std::endl;
}
```

This looks good for our v1, and may stay like this quiet sometimes. But, we can already spot some issues. We are specifying the date, do we really need to emit log with a date that is not the current time ? But, the issues are yet to come. What if we want to control the output stream ? Maybe, in order to emit the log on a managed logger system, we won't be able to use the stream that way. And what if we want to be able to emit logs on different support ? We may also need to emit logs using different formats (JSON, colon or comma separated ...)

Even for a simple example like this one, we can see that our quick solution is not _future proof_.

Despite the code simplicity, this function is doing two different things: formatting and printing. At this stage, it may seems overkill to split this code, but the cost is still low. I will discuss later several way to rewrite this.

#### Identifying orthogonality issues

In the previous example, the problem was logical and the code itself does not exhibit any sign of tight coupling. It is not always easy to identify orthogonality issues, in particular at very early stage of the code, as some of the problems are inherent to the future changes.

To beginnig with, there are some tricks that you can use, in the form of question you could ask yourself about your code or design are some examples, I often use:

* Can I describe this function with a simple sentence ?
* Could this method be reused in a different part of the code ?
* What does it take to reuse this object/function/method ?
* What will be the impact of changing _X_ on this code ?
* Is this class movable, copyable, default instantiable ?
* Can I make this method `const` ?
* Is it thread safe ?
* If I write a usage comment for this function, will it resist implementation changes ?
* What does it take to test this code ? Is it possible to write standalone tests without moking ?
* How source files are impacted by my change ? What did I broke that has nothing to do with it ?
* If I was reviewing this change, is the diff enough to understand it ?
* Is the entity name enough to understand its purpose ? Is it possible to find such a name ?

This is a non exhaustive list. Some questions are C++ oriented but you could find similar ones in all languages.

It could be interesting to question how the work can be split between multiple contributors and how much of the job can they do without synchronisation.

From a pure programming perspective, there's also some aspects that are signs
