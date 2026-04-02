---
title: "Of Geeks, Engineers, and Gnomes"
---

![A gnome writing code](/assets/images/gnome-coding.png)

I've been away from this blog for a while (nothing new) but I'm back and now we got some tiny gnomes to help us write text or even code.

In fact, those gnomes are the main characters for this new post as we're going to talk about vibe coding (yes, _yet another vibe coding blog post_). Along the way, we'll look at the dual personality of programmers, the long history of programs writing programs, why natural languages are a poor fit for describing code, and where all of this might be heading.

## Context, foreword, introduction ...

I won't split hairs on vocabulary: vibe coding, AI assisted code writing, whatever you call it, all of this falls into the same trend: leveraging new AI models' ability to work with code.

Now, for more context: I am a software engineer but I have been a computer science teacher (and programming was one of my main lectures for years) and I have been writing code for decades (I was about 8 or 9 years old when I wrote my first lines of BASIC on a zx81 in the mid 1980s). The biggest impact for today's topic is that writing code is something natural for me, to a point where it's sometimes (even often) easier to write the code than to explain what it should do. When I was building my programming lectures, to cover a new topic, I was usually starting by writing code and then derive my teaching materials from it.

But we are in 2026, and I work as a software engineer, and as such, ignoring AI tools is no longer an option. I, too, have my gnomes working for me everyday, I am OK with it, I can't even say that I don't like having them around. Now, we are in a transition period, and it's interesting to think about it, look at what we get from it and try to identify the traps on the road.

To make sense of how we react to all this, it helps to look at who we are as programmers.

## The geek and the engineer, the dual mind of a coder

> _Disclaimer: I have no degree in psychology or whatever social science. I'm an engineer but as I said I've been a teacher and as such, I've spent a lot of time trying to understand the mechanism and relationship to code in my students and by extension to my colleagues._

Across the years, I've come to the observation that software engineers usually have a mix of two _personalities_: a geek and an engineer.

The **geek** is the passionate programmer, the guy that writes code just for the pleasure of doing it. This is the part of us that gets excited by new programming languages, the part of us that pushes us to write smart and elegant code for the beauty of the result. This is also the part of us that gets highly opinionated on tech topics at the expense of productivity and pragmatism.

On the other side the **engineer** is the professional guy who wants to deliver. Writing code is only a way to get the job done. A good engineer should not waste time on writing the best code, the goal is to bring a solution that meets the expectations and relevant choices are those heading in that direction.

I believe, most of us are a mix of a geek and an engineer. I also believe that both can bring value to our job. Pure pragmatism can make mistakes in the long run, while over enthusiasm may lead to tunnel vision on the wrong things, but combine them and you can get a very efficient software engineer. Of course, it could also lead to disastrous results ...

Back to our topic, the geek and the engineer have a lot to do with our relations to those AI stuff. When I first read [The Unreasonable Effectiveness of Recurrent Neural Networks](https://karpathy.github.io/2015/05/21/rnn-effectiveness/), I was thrilled (geek side) by the part where the RNN is generating LateX or C code, and even today after using assistants for a while, I'm amazed by some results. At work, the things I gave to my gnomes were all those long and repetitive tasks on which we waste so much of our time (probably the engineer side, pushed by the geek).

But the geek persona is also the one complaining the most about vibe coding. **Afterall, where is the fun if we let the gnomes write the code for us!**

> On this topic, there is an excellent blog post by Sarah Dayan [What's left when agents write better code than you](https://www.sarahdayan.com/blog/whats-left-when-agents-write-better-code-than-you).

I won't go in that direction. You can do vibe coding for boring parts and use your time to focus on interesting bits, this is where the fun is. In fact, I think that the geek in us has more to gain than the engineer.

Our geek and our engineer have more to do with how we react to those new technologies than with what we will do with them. The engineer shrugs and asks "does it ship faster?"; the geek crosses their arms and asks "but where is the craft?" Understanding which voice is speaking helps us sort the signal from the noise.

And with that lens, let's look at the bigger picture.

## The long story of programs writing programs

> _One of the privileges of being old, is that you've always seen it before ..._

So, our gnomes are only the last incarnation on a long list of programs writing programs. Some were successful (as far as I know, we still use compilers) and some far less so (where is Rational Rose now?).

Looking back at the emergence point of those technologies, you can see that people always reacted in a similar way: some were excessively enthusiastic while others rejected them outright. I'm not old enough to remember when the first compilers were introduced, but when I was a student, we were still discussing the benefits of using compiled languages over writing assembly. That's probably why I was so interested in programming language design and compilers (and I'm still into it).

I can't tell if there's a pattern of successful tools, but there's a trend that I want to dig into. When I look at these technologies, they seem to split into two big categories.

So, when it comes to programs writing programs, we can class them into: those that try to be _userfriendly_ and those that abstract technical complexity while remaining technical.

In the first class, you will find graphical UI to build software without writing code, products that produce code out of specifications or modeling and so on.

On the other side, you have programming languages and their compilers, parser generators, templating engines, preprocessors ...

And as far as I can tell, almost all _userfriendly_ tools had a short life. They may have had some success, but they did not stay. On the other hand, languages created in the 1970s or 1980s are still being used widely and are still part of the top languages.

In which category do I put vibe coding tools? Surprisingly, the answer is not straightforward. If you consider the archetypal use case where a complete newbie prompts some AI tool, asking something like "build my website for me", then we're definitely in the _userfriendly_ class.

But that use case is broken. There is this _80/20 rule_, that essentially says that our newbies will be able to get the first 80% of their project and then they will struggle with the remaining 20% as it requires deeper understanding of software building.

In fact, if you look at how we are using those tools, you will see patterns that are not very different from what we've been doing for decades. Most of the discussions I've seen are not really about what to put in the prompt, but how to control the process: how do you enforce structured and reproducible behaviors? Think about it: we write rules files, configuration files, structured prompts, we set up hooks and checks -- we are building toolchains around our gnomes. Somehow, the gnomes start to look more and more like compilers, code generators and other transpilers and less like the _userfriendly_ click-click tools.

And that observation leads to a deeper question.

## Are natural languages suited to describe programs?

> _If your code needs a comment to be understood, it would be better to rewrite it so it’s easier to understand._ Rob Pike, Notes on Programming in C

This is not a new problem. Formal disciplines have always struggled with natural language. Consider this passage:

> _If a first magnitude and a third are equal multiples of a second and a fourth, and a fifth and a sixth are equal multiples of the second and fourth, then the first magnitude and fifth, being added together, and the third and the sixth, being added together, will also be equal multiples of the second and the fourth, respectively._ Proposition 2 from Book 5 of Euclid’s Elements

Take some time to process it. Got it? This is `a(x + y) = ax + ay`. Once mathematicians introduced symbolic notation (you know, `+`, `-`, `=`...), they never looked back. The symbols are not just shorter -- they are precise, unambiguous, and scannable.

Programming faces the exact same issue. I am back with my teaching experience, but this time on algorithms. There’s a classic question that comes when building an algorithm class: how do you write algorithms?

Usually, the reasoning goes:
* algorithms don’t depend on implementation language, so you describe them with natural language;
* but then it becomes messy and unreadable, and even misleading;
* you add some structure to the text, then add more ...
* you end up with something that is almost code, not exactly, but still.

At Epita, while I was doing the programming class, I was also teaching assistant for the algorithmic class and when I started, we were using a form of pseudo-code in French. But this pseudo-code was so formalized that I’ve been able to write an interpreter without the need of any further clarifications. In lots of papers presenting algorithms, you will find pseudo-pascal (or similar) code blocks. Those are way more readable than attempts in natural language, precisely because they borrow the structure and precision of code.

Let’s see why. Here is an algorithm described in plain English:

> We want to compute the lower integer part of a square root of an integer `n`. We define our candidate root `r` and initialize it with `n`. Then while `r` is greater than `n` divided by `r`, we replace `r` with the mean of `r` and `n` divided by `r`. The final value of `r` is our result.

Now, the same thing as code:

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

I’m not saying that, given that code, you’ll immediately see that we’re computing a square root, but admit it (if you read code) reading the text was painful. And you know what, **it was painful to write too**.

Equations, algorithms or programs need precision and unambiguous descriptions. But we also want them to be concise, clear and straightforward. Natural languages are not good at this; symbolic languages and programming languages are designed precisely for this. And this matters for vibe coding: the more precisely you need to talk to the gnomes, the closer you get to writing code anyway.

## Vibe coding or not vibe coding?

> _Let's put an LLM in my compiler ..._

So, vibe coding or not? Both. The question isn't whether to put the gnomes to work, it's how they fit into the way we build software.

If I were to imagine the future of software engineering, I would see a convergence between coding assistants and regular development tools, just like IDEs, compilers, linters and others already work together to help you on your tasks. This kind of convergence is asymptotic, it never ends -- and that's fine.

Where I think things are heading:
* Chat-like interfaces are good for planning, launching tasks, asking questions and automating plumbing;
* Programming languages should continue to evolve towards less boilerplate, less verbosity;
* Coding assistants should integrate more structured configuration mechanisms (how many times did you ask claude to run the checks before committing code?)

In short, the gnomes will get better when we stop treating them as magic oracles and start treating them as what they are: another layer in the toolchain.

### The cost of entry

Now, I have some concerns. One in particular keeps me up at night: the price.

Running vibe coding tools has a cost, and it's not cheap. When I was a student, with an old computer and a Linux distro (or FreeBSD, or OpenBSD ...) you were able to write code and contribute to any open source project. It was even possible to do it without an internet connection as long as you could access it from time to time. The barrier to entry was essentially zero.

That is changing. If effective coding increasingly requires AI tools, and AI tools require paid subscriptions or expensive hardware, then we are building a paywall around software development itself. There are attempts to provide models that run on your own hardware, but let's be realistic, you need an expensive setup to do so -- it's not working on your average laptop. This could quietly reshape who gets to participate in open source, and that worries me.

### Teaching programming in the age of AI

The other concern is education. How do we teach programming in this world? What's the risk of having new engineers that are not able to write code by themselves? If students learn to prompt before they learn to think in code, do they ever build the deep understanding needed to debug, architect, or reason about systems? I don't have an answer yet, but it's a question worth taking seriously.

## Final words ...

> _I wrote this one shot, which hasn't happened in years. I will save it twice (to keep the original) and run some AI assistant on it to fix my poor English and improve my prose!_

This is a glimpse of where my thinking stands today. There are far more things to explore -- we're just at the beginning of the journey. If you haven't tried putting the gnomes to work, do it, it's still fun. And if the geek in you resists, remember: even compilers were controversial once. The engineer will come around. The geek might even enjoy it -- after all, gnomes are kind of cool.
