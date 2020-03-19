---
Title: Growing fast, growing strong
---

For a year and half, I'm now working for Algolia, a SaaS start-up. Even if it's growing too big to still be considered as a start-up, the company keeps the pace of a start-up a fast growing and evolving entity that continuously change and adapt. I love it.

But as a software engineer this is a challenge, I'm working on the core components of our service, a search engine and the corresponding indexing service. Such software are pretty large, requires performances, stability, safety ... But we're also a small team, less than ten engineers, and we were even less when I started.

This post is about how we manage (or try to) this challenge, and what ideas both from our processes to the code itself, we are trying to leverage. I don't promise groundbreaking techniques or totally new methods of software engineering, just my personnal reflexion on the subject. Also, I won't try to relate the way we work with any exiting methods, for one good reason: I don't care about established methods.

I wrote this article as a journey toward some technical idea (in C++ as usual), but I'll start by talking about organisation and software delivery.

## Deliver fast and iterate

We deploy a new version of our search engine every week. Considering that it takes at least three days to fully deploy, we are almost continuously deploying. This is our rythme, Monday morning one of us tags and builds the new release, and start the tools that will deploy it all around the world.

It means that as long as a feature is ready, it can be used in production in a week. It usually takes a bit more, but this is still fast for that kind of software. It also means that if your new code breaks something, you'll know pretty fast, and this is good.

Guided by this early delivery we try to follow these steps when designing new feature:

1. Gather all available inputs and search for common patterns.
2. Try to think the final feature at large.
3. Reduce the scope to what could be an acceptable version 1 from user perspective.
4. Filter this version against technical criterion to narrow it down again.
5. Narrow it further to obtain an acceptable and testable beta version.
6. Design and implement this beta.
7. Deploy to a group of select users.
8. Evaluate feedbacks and iterate.

This process contains at least three steps where we narrow the feature to a core version that can be tested (_i.e._ a version containing the interesting elements for the final users) but also one that can be implemented in short cycle.

Narrowing as much as possible is the key point here, being able to jump from initial discussions to a first version running in production in few weeks, we will be able to evaluate the accuracy of the feature before investing more time in it.

The benefits of such an approach are many:

* Early stops: often a feature looks good until you try to use it, being able to test it at scale at an early stage is important.
* Avoid over-design: you don't know how far you need to go until you tried your feature at scale.
* Evaluate and drive future iterations: what points lack efficiency ? what are the real usage profiles ? Once again, nothing replace evaluation at scale.
* Avoid tunnel effect: don't spend endless months without visible results.

Of course, there're drawbacks. First, as any design process, it relies on our ability to establish a realistic scope. On the design side, there's also some risk related to the accuracy of the first iteration. There are so many ways to miss the point. But those risk are not surprising nor different than what we usually face in other design and developpement processes.

From my technical perspective, the evil lies, as usual, in the details. And this time, details are code quality and maintanibility.

## Good code, bad code ...

Code quality is vaste subject, with very different opinions by a lot of different people. I'm pretty sure that almost any developper has its own definition of code quality. Here is mine, I've built a pragmatic approach, narrowing the concept to what I think is important:

* Good code works: in particular it does the expected job.
* Good code contains no obvious bugs: perfection is not easy, but at least easily spotted bugs have been removed.
* Good code can evolve, easily.
* Good code is frugal with resources: even if you don't write for performances, you don't need to waste cycles and bytes.
* Good code looks simple: no over complicated patterns,

So, good code should be simple, do the job and is somehow ready for the next iteration. Keeping this three aspects together could be a hard task.

## A toy use-case: printing some data

*I tried to put down a very naive example to illustrate how write such a good code. As any toy example, it is a bit artificial and there's many ways to make it clean. Just don't forget that it's a toy demo.*

So, we have a timers, probably accumulate while doing some computation and we want to log them. The timers themselves are pretty simple and not subject to evolution (in this example) and described by the following C++ implementation:

```c++
/*
 * Toy example: a collection of timers
 */

#pragma once

#include <chrono>

namespace example {
using std::chrono::seconds;
using namespace std::chrono_literals;

/*
 * A simple POD storing a set of duration in seconds
 */
struct Timers
{
    seconds init        = 0s;
    seconds comput      = 0s;
    seconds postprocess = 0s;
};
}
```

We need to log and later perform some analysis on the logged data. Analysis is out of scope, we only care about logging the data in a proper way:
* Each set of timers have an extra name (available in the code)
* The team analysing the data want to retrieve a CSV file for a given period
* The actual process is run on a single machine but plans are to have multiple instances in a near future.
* Another team is building a pipeline to gather and store several kind of logs and expose a retrieving API later.

As a first pass, we consider that it will be enough to directly output the data in CSV format directly.

So we come with a first version:

```c++
/*
 * Toy example: log timers
 */

#pragma once

#include <chrono>
#include <ostream>

#include "timers.h"

namespace example {

class log_timers
{
public:
    log_timers()  = default;
    ~log_timers() = default;

    log_timers(std::ostream& out) : output_(out) {}

    /*
     * Log a set of timers with a name
     */
    void operator()(const std::string& name, const Timers& timers);

private:
    // output stream default to std::cout
    std::ostream& output_ = std::cout;
};

inline void log_timers::operator()(const std::string& name, const Timers& timers)
{
    // Hardcoded CSV format
    // Assume no string ambiguity
    output_ << '"' << name << "\"," << timers.init.count() << "," << timers.comput.count()
            << "," << timers.postprocess.count() << std::endl;
}

}
```

We have our feature implemented and it satifies our requirements and we are happy, are we ?
