---
title: Let's talk about errors
---

After a long break (more a long period of unfinished draft articles), I wanted to talk about errors and errors management. That's a topic that all programmers and software engineers are confronted with almost everyday. Both as a software engineer and as CS/programming teacher, I spent a lot of time thinking about it. I have given it more thought recently as I have been working on Go projects, and honestly, coding in Go gives you the sensation that error handling is all you do.

So, this post is an _unfinished_ product of my thinking, don't take it to seriously, if you disagree with me, fine, at least I would have push you to think about it!

Before diving in the topic, I have one _advice_: follow the model of your environment, programming language, tools ... Don't rebuild your own system, use error values in Go, use exceptions in Python or C++ and pray Cthulhu if you are using C.

## The problem(s)

So, we all know that **we must handle errors**, but we also all know that we often fail at doing it, most of the time. Before trying to understand why we fail, let's see what may happen when we fail.

Inexistent or bad error handling can result in:
* Program crashing with obscure error message.
* Program don't crash (or crash later) but do its job based on a broken state.

The later point is the most dangerous one. I will illustrate my point with a **broken** example in C. Let's consider this piece of code:

```c
    struct my_data content;
    char buff[FILE_SIZE];
    int fd = open(file_path, O_RDONLY);
    read(fd, buff, FILE_SIZE);
    if (!parse(buff, &content)) {
        errx(1, "failed to parse %s", file_path);
    }
```

Before you scream, remember that it's broken on purpose.

Rather than commenting how broken this code is, we will see the potential consequences of using it. First, we may need a bit of context. Since the code is using a fixed size for the buffer, we can assume that this code expect a file with a fixed or bounded size. This is the kind of code that makes a lot of assumption on what is happening around.
If there is an issue with the target file and either `open` or `read` failed, in the best case, `parse` will failed too, and we'll get a _wrong error message_. But now, let's see what may really happen. If the file can not be opened, `open` returns a negative value, then `read` will fail too **without** changing the content of `buff`. So, in that case, what we pass to `parse` will be the content of the memory location where `buff` is.

If this piece of code appears in a loop, we will have the content of the previous opened file. And, I sure you got it now, `parse` will succeed, because it succeeds before. Now we end-up with a program that silently continues after an error that should probably be fatal.

So even if obscure error messages, or silent crashes, are annoying, they are not as dangerous as a program that silently continues and does its job with faulty data and wrong states. So, as I see it, error handling has two goals with priority:
1. No error should silently be ignored.
2. All error should, at some point, be handled in a meaningful way.

Now let's have a look at the tools and techniques available to handle errors.

## Errors as values

So, even if your programming language has no specific support for error handling, you can still rely on this. The biggest problem is to make clear and _easy_ to use.

I will start with _the old way_, and the classical example from the POSIX model. Consider the syscall `open(2)`.

```c
int open(const char *path, int oflag, ...);
```

Errors are identified by a negative result (in fact `-1` exactly) and the nature of the error is reported with a numeric code in `errno`. At least, `read` and `write` and most POSIX syscalls behave the same.

The main issue of this model is that errors are hidden inside the standard behavior. If you don't check the return value of `open(2)`, you get a value that seems to be usable with `read` or `write`. Of course those will fail too, but again you need to check the return value of those functions.

Obviously, having a different channel for errors sounds better. Of course in C, this can be tricky as you don't have an easy and standard way of returning more than one value. C++17 `filesystem` library has some example using reference to pass a placeholder for an error (they also provide versions with exceptions).

```c++
bool std::filesystem::exists( const std::filesystem::path& p, std::error_code& ec ) noexcept;
```

Languages like Go leverage their support for multiple return values to handle errors:

```go
func div(a, b int) (int, error) {
	if b == 0 {
		return 0, fmt.Errorf("can divide %d with 0", a)
	}
	return a / b, nil
}
```

The single error interface emphases the nature of the returned value (with the convention to always put the error last).

> _Note that it is still possible to ignore errors, but at least you do it knowingly._

Let's have a look at how those errors are used on the other side:
```go
func UselessCode(a, b int) (int, error) {
	c, err := div(a, b)
	if err != nil {
		return 0, err
	}
	d, err := div(b, a)
	if err != nil {
		return 0, err
	}
	return c + d, nil
}
```

The pattern:
```go
	if err != nil {
		return err
	}
```
is omnipresent in Go.

Are we handling the error in a meaningful way? No.

This code is just an explicit exception mechanism. The error is just forwarded to another context. It may be handled somewhere later, but this code is not doing error handling.

Before moving to the next approach, I want to emphasis what is probably the worst aspect of error as value: **readability**. Consider my function `UselessCode`, this function has 9 lines of code but only one is really meaningfull. And, even that line is not as _clear_ as it should be. Without the error forwarding code, we get:

```go
func UselessCode(a, b int) int {
	return div(a, b) + div(b, a)
}
```

Now intent is way more clear, but we miss error forwarding. This is where exceptions come into action.

## Exceptions

Exceptions are a mechanism that requires support from your language (in C, you can try some hacks using things like `setjmp` and `longjmp`, of course).

The nice point of exceptions is that:

* They are always _handled_, one way or another, if you ignore them, they just kill your program.
* You don't need to explicitly forward them.
