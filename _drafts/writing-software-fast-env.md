---
Title: Growing fast, growing strong
---

For a year and half, I'm now working for Algolia, a SaaS start-up. Even if it's growing too big to still be considered as a start-up, the company keeps the pace of a start-up a fast growing and evolving entity that continuously change and adapt. I love it.

But as a software engineer this is a challenge, I'm working on the core components of our service, a search engine and the corresponding indexing service. Such software are pretty large, requires performances, stability, safety ... But we're also a small team, less than ten engineers, and we were even less when I started.

This post is about how we manage (or try to) this challenge, and what ideas both from our processes to the code itself, we are trying to leverage. I don't promise groundbreaking techniques or totally new methods of software engineering, just my personnal reflexion on the subject.

I wrote this article as a journey toward some technical idea (in C++ as usual), but I'll start talking about organisation and software delivery.

## Deliver fast and iterate

We deploy a new version of our search engine every week. Considering that it takes at least three days to fully deploy, we are almost continuously deploying. This is our rythme, Monday morning one of us tags and builds the new release, and start the tools that will deploy it all around the world.

It means that as long as a feature is ready, it can be used in production in a week. It usually takes a bit more, but this is still fast for that kind of software. It also means that if your new code breaks something, you'll know it pretty fast, and this is good.

When designing new feature, the biggest difficulty is to evaluate the usefulness of the feature. What happen most of the time, is that you have some feedback about missing possibilities.
