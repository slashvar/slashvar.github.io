---
Title: Growing fast, growing strong
---

For a year and half, I'm now working for Algolia, a SaaS start-up. Even if it's growing too big to still be considered as a start-up, the company keeps the pace of a start-up a fast growing and evolving entity that continuously change and adapt. I love it.

But as a software engineer this is a challenge, I'm working on the core components of our service, a search engine and the corresponding indexing service. Such software are pretty large, requires performances, stability, safety ... But we're also a small team, less than ten engineers, and we were even less when I started.

This post is about how we manage (or try to) this challenge, and what ideas both from our processes to the code itself, we are trying to leverage. I don't promise groundbreaking techniques or totally new methods of software engineering, just my personnal reflexion on the subject. Also, I won't try to relate the way we work with any exiting methods, for one good reason: I don't care about established methods.

I wrote this article as a journey toward some technical idea (in C++ as usual), but I'll start talking about organisation and software delivery.

## Deliver fast and iterate

We deploy a new version of our search engine every week. Considering that it takes at least three days to fully deploy, we are almost continuously deploying. This is our rythme, Monday morning one of us tags and builds the new release, and start the tools that will deploy it all around the world.

It means that as long as a feature is ready, it can be used in production in a week. It usually takes a bit more, but this is still fast for that kind of software. It also means that if your new code breaks something, you'll know it pretty fast, and this is good.

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

Narrowing as much as possible is the key point here, being able to jump from initial discussions to a first version running in production in few weeks, we will be able to evaluate the accuracy of the feature before invest more time in it.

The benefits of such an approach are many:

* Early stops: often a feature looks good until you try to use it, being able to test it at scale at an early stage is important.
* Avoid over-design: you don't know how far you need to go until you tried your feature at scale.
* Evaluate and drive future iterations: what points lack efficiency ? what are the real usage profiles ? Once again, nothing replace evaluation at scale.
* Avoid tunnel effect: don't spend endless months without visible results.

Of course, there're drawbacks. First, as any design process, it relies on our ability to establish a realistic scope. On the design side, there's also some risk related to the accuracy of the first iteration. There are some many ways to miss the point. But those risk are not surprising nor different than what we usually face in other design and developpement processes.

From my technical perspective, the evil lies, as usual, in the details. And this time, details are code quality and maintanibility.

## Good code, bad code ...

Code quality is vaste subject, with very different opinions by a lot of different people. I'm pretty sure that almost any developper has its own definition of code quality. Here is mine, I've built a pragmatic approach, narrowing the concept to what I think is important:

* Good code works: in particular it does the expected job.
* Good code contains no obvious bugs: perfection is not easy, but at least easily spotted bugs have been removed.
* Good code can evolve, easily.
* Good code is frugal with resources: even if you don't write for performances, you don't need to waste cycles and bytes.
* Good code looks simple: no over complicated patterns,
