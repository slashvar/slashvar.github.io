---
title:
---

I've been away from this blog for a while (nothing new) but I'm back and now we got some tiny gnomes to help us writing down text or even code.

In fact, those gnomes are the main characters for this new post as we're going to talk about vibe coding (yes, _yet another vibe coding blog post_).

## Context, forewords, introduction ...

First, let's state that I won't be precise on vocabulary. There are thousands of posts everywhere about what is vibe coding, AI assisted code writing ... But all of this falls into the same trend: leverage new AI model ability to work with code.

Now, for more context: I am a software engineer but I have been a computer science teacher (and programming was one of my main lectures for years) and I have been writing code for decades (I was about 8 or 9 years old when I wrote my first lines of BASIC on a zx81 in the mid 1980s). The biggest impact for today's topic is that writing code is something natural for me, to a point where it's sometimes (even often) easier to write the code than to explain what it should do. When I was building my programming lectures, to cover a new topic, I was usually starting by writing code and then derive my teaching materials from it.

But we are in 2026, and I work as a software engineer, and as such, using AI tools is no longer an option, for a lot of reasons. I, too, use AI tools everyday, I am OK with it, I can't even say that I don't like using those tools. Now, we are in a transition period, and it's interesting to think about it, look at what we get from it and tries to identify the traps on the road.

Let's start by a reflection on the software engineer persona in order to help us think about this.

## The geek and the engineer, the dual mind of a coder

> _Disclaimer: I have no degree in psychology or whatever social science. I'm an engineer but as I said I've been a teacher and as such, I've spent a lot of time trying to understand the mechanism and relationship to code in my students and by extension to my colleagues._

Across the years, I've came to the observation that software engineers usually have a mix of two _personalities_: a geek and an engineer.

The **geek** is the passionate programmer, the guy that writes code just for the pleasure of doing it. This is the part of us that get exited by new programming languages, the part of us that push us to write smart and elegant code for the beauty of the result. This is also the part of us that get highly opinionated on tech topics at the expense of productivity and pragmatism.

On the other side the **engineer** is the professional guy who wants to deliver. Writing code is only a way to get the job done. A good engineer should not waste time on writing the best code, the goal is to bring a solution that meets the expectations and relevant choices are those heading in that direction.

I believe, most of us are a mix of a geek and an engineer. I also believe that both can bring value to our job. Pure pragmatism can make mistake on the long run, while over enthusiasm may lead to funnel effect, but combine them and you can get a very efficient software engineer. Of course, it could also lead to disastrous results ...

Back to our topic, the geek and the engineer have a lot to do with our relations to those AI stuff. When I first read [The Unreasonable Effectiveness of Recurrent Neural Networks](https://karpathy.github.io/2015/05/21/rnn-effectiveness/), I was thrilled (geek side) by the part where the RNN is generating LateX or C code, and even today after using assistants for a while, I'm amazed by some results. At work, the things I gave to coding assistant was all those long and repetitive tasks on which we waste so much of our time (probably the engineer side, pushed by the geek).

But the geek persona is also the one complaining the most about vibe coding. **Afterall, where is the fun if an AI write the code for us!**

> On this topic, there is an excellent blog post by Sarah Dayan [What's left when agents write better code than you](https://www.sarahdayan.com/blog/whats-left-when-agents-write-better-code-than-you).

I won't go in that direction. You can do vibe coding for boring parts and use your time to focus on interesting bits, this is where the fun is. In fact, I think that the geek in us has more to gain than the engineer.

Our geek and our engineer have more to do on how we react to those new technologies than to what will do with them.

## The long story of programs writing programs

> _One of the privilledges of being old, is that you've always seen it before ..._

So, vibe coding is only the last incarnation on a long list of programs writing programs. Some were successful (as far as I know, we still use compilers) and some other less (where is Rational Rose now).

Looking back at the emergence point of those technologies, you can see that people always reacted in a similar way: some were excessively enthusiast while others rejected them. I'm no old enough to remember when first compilers were introduced, but when I was a student, we were still discussing the benefits of using compiled languages against writing assembly, that's probably why I was so interested in programming language design and compilers (and I'm still into it).

I can't tell if there's a pattern of successfull tools, but there's a trend that I want to dig.

So, when it comes to programs writing programs, we can class them into two big categories: those that tries to be _userfriendly_ and those that abstract technical complexity while remaining technical.

In the first class, you will found graphical UI to build software without writing code, products that produce code out of specifications or modelisation and so on.

On the other side, you have programming languages and their compilers, parser generators, templating engines, preprocessors ...

And as far as I can tell, almost all _userfriendly_ tools had a short life. They may have had some success, but they did not stay. On the other hand, languages created in the 1970's or 1980's are still being used widely and are still part of the top languages.

In which category do I put vibe coding tools? Surprisingly, the answer is not straightforward. If you consider the archetypal use case where a complete newbie prompts some AI tool, asking something like "build my website for me", then we're definitely in the _userfriendly_ class.

But that's use case is broken. There is this _80/20 rule_, that essentially say that our newbies will be able to get the first 80% of their projet and then they will struggle with the remaining 20% as it requires deeper understanding of software building.

In fact, if you look at how we are using those tools, you will see pattern that are not very different from what we're been doing. Most of the discussions that I've seen around are not really about what to put in the prompt, but how do you control the process, how do you enforce structured and reproducible behaviors. Somehow, coding assistants start to looks more and more like compilers, code generators and other transpilers and less like the _userfriendly_ click-click tools.

## Are natural languages suited to described programs?

> _If your code needs a comment to be understood, it would be better to rewrite it so it’s easier to understand._ Rob Pikes, Notes on Programming in C

I am back with my teaching experience, but this time on algorighms. There's a classical question that comes when building an algorithm class or even when you just want to explain an algorithm to students: how do you write algorithms.

Usually, the reasoning is:
* algorithms don't depend on implementation language, so you describe it with natural language;
* but then it becomes messy and unreadable, and even misleading;
* you add some structures to the text, then add more ...
* you end up with something that is almost code, not exactly, but still.

At Epita, while I was doing the programming class, I was also teaching assistant for the algorithmic class and when I started, we were using a form of pseudo-code in French. But this pseudo was so formalized that I've been able to write an interpreter without the need of any further clarifications.

In lots of papers presenting algorithm, you will find pseudo-pascal (or similar) code blocks. Those are way more readable than attempt in natural langues.

Let's take an example.

> We want to compute the lower integer part of a square root of an integer `n`. We define our candidate root `r` and initialize it with `n`. Then while `r` is greater than `n` divided by `r`, we replace `r` with the mean of `r` and `n` divided by `r`. The finally value of `r` is our result.

How is it readable?

Now, let see an implementation of it:

```C
unsigned sqrt(unsigned n) {
	unsigned r = n;
	while (r > n/r) {
		r = (r + n/r)/2;
	}
	return r;
}
// Yes, this code can divide by 0. This is an example.
```

I'm not saying that, given that code, you'll immediately see that we're computing a square root, but admit it (if you read code) reading the text was painfull. And you know what, **it was painfull to write too**.

Programming is not an exception, formal jobs require formal communication. Have you ever read equations before the introduction symbolic notations (you know `+`, `-` ...)

> _If a first magnitude and a third are equal multiples of a second and a fourth, and a fifth and a sixth are equal multiples of the second and fourth, then the first magnitude and fifth, being added together, and the third and the sixth, being added together, will also be equal multiples of the second and the fourth, respectively._ Proposition 2 from Book 5 of Euclid's Elements

OK, take some time to process it. You got it? This is `a(x + y) = ax + ay`.

Equations, algorithms or programs need precision and unambiguous descriptions. But, we also want it to be concise, clear and straightforward. Natural languages are not good at this while symbolic langues or proramming languages are designed just for this.

## Vibe coding or not vibe coding?

> _Let's put an LLM in my compiler ..._

If I were to imagine the future of software engineering, I would see a convergence between coding assistants and regular development tools, just like, IDE, compilers, linters and others are working together to help you on your tasks. Of course, this is already the case but this kind of convergence is asymptotic, it never ends.

The important points I can see:
* Chat-like interfaces are good to plan, launch tasks, ask questions and automate plumbing;
* Programming languages should continue to evolve towards less boiler plate, less verbosity;
* Coding assistant should integrate more structured configurations mechanism (how many time did you asked claude to run the checks before commiting code?)

Now, I have some concerns around those technologies:
* How do we teach programming in this world, what's the risk of having new engineers that are not able to write code by themselves?
* What is the impact on the open source ecosystem?

About the last point, there is one aspect that for me could have a huge impact on open source: the price. Running vibe coding tools has a price, and it's not cheap. When I was a student, with an old computer and linux distro (or FreeBSD, or OpenBSD ...) you were able to write code and contribute to any open source projects, it was even possible to do it without an internet connection as long as you could have access to it from time to time. There are attempts to provide models running on your own hardware, but be realistic, you need some expensive set-up to do so, it's not working on your average laptop.

## Endwords ...

> _I wrote this one shot, which did not happened for years. I will save it twice (to keep the original) and run some AI assistant on it to fix my poor English and improve my prose!_

This is glimpse of the state of my reflections on this topic. There are far more things to explore, we're just at the beginning of the journey. If you haven't try AI coding, do it, it's still fun.
