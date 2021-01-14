--
Title: Introduction to system programming
--

System programming is one of the major topics of software engineering. The boundary of what is _system_ in
programming is often fuzzy, but the concept behing may serve engineers in many context. It's part of my day to day
job and, in my previous carreer, it was one of my teaching topics.

There are various way to define system programming, but there are some common aspects that can be used to draw a
first scope of the topic:

* Interractions with files system
* Communications between processes
* Processes management
* Interraction with the operating system or underlying hardware components

I tend to describe system programming as **buidling programs that interract with non-human actors**.

So, I wanted to write some posts (let say, I will start with one) about system programming in general, putting down
theory and experience.

## How?

A large part of system programming is about system interractions, you need to choose a system. I will use POSIX
like systems and Linux (but I will try to stick to POSIX as much as possible).

The second question is about the programming language. Lot's of material around system programming are using C, for
a lot of good reasons, but that's not mandatory. Theoretically, you can do system programming with a lot of
different languages. I believe that C++ could be a good choice here, as it is close enough to C and provides some
comfortable abstractions.

So, if you want to run the provided code or make your own experiments, you will need POSIX like system (Linux,
MacOS, FreeBSD, OpenBSD ... ) with a compiler supporting C++17 (I am using `clang` 10), a terminal and a text editor.
A version of `make` can be useful, but you can do without.

## System calls

The lower level of interractions with the system for a userland program are syscalls. For the programmer, a syscall
looks like a regular function, but behind the scene it triggers a context switch with the system. Since it's an
introduction, we won't dive deeper and use them as _normal functions_. Syscalls are described in section 2 of the
man pages (some function of section 3 are in fact wrappers around syscalls too). You can try `man 2 read` in your
shell for an example. I will use the usual convention `read(2)` that indicates that the man page of the function
`read` is in section `2`.

Most syscalls returns an `int` with negative values indicating failures. They all set the global symbol `errno` to
the corresponding error code. The meaning of the error code for the current function is described in the man page
of the function, an example from the man page of `read`:

```
ERRORS
     The pread(), read(), and readv() calls will succeed unless:

     [EAGAIN]           The file was marked for non-blocking I/O, and no data were ready to be read.

     [EBADF]            fildes is not a valid file or socket descriptor open for reading.

     [EFAULT]           Buf points outside the allocated address space.

...
```

### A first example with `read(2)`

`read(2)` is a good syscall examples. So, I will start with a small demo function that will read bytes from a file
descriptor (more that later) and add the content to a `std::string`. I will use this example to illustrate both the
way to use `read(2)` and the way to handle errors.

```c++
/*
 * demo(fd) reads bytes from fd and output a string
 */
std::string demo(int fd)
{
    // We need a buffer
    constexpr size_t buff_size = 16;
    char             buff[buff_size];

    // the output string
    std::string output {};

    // read return value
    int r = 0;

    // end-of-file is reached when read returns 0
    while ((r = read(fd, buff, buff_size)) != 0) {
        if (r < 0) {
            // read failed
            if (errno == EINTR) {
                // Reading was interrupt, just restart, see read(2)
                continue;
            }
            // for other error, let's just display it and leave the program
            err(1, "error while reading");
        }
        // insert what was read at the end of output
        output.insert(end(output), buff, buff + r);
    }
    return output;
}
```

This function uses `read(2)` to read bytes from `fd` into `buff` and it reads at by blocks of atmost `buff_size`
bytes, it stores the read bytes into `output` and continue until `read(2)` returns `0`, indicating the end-of-file.

First, `read(2)` is a generic reading syscall, it can read from any readable resources (like files or terminal). It
will read as much as it can in one shot (but no more than the amount asked for) and will block if no data are
available. If you are reading from a file, you will always get the amount you were asking for until you reach the
end. But for stream like resources (terminal, pipes, sockets ... ) `read(2)` will block until data are available
and returns as soon as it can read something, even if there are less bytes than what you asked for. It will then
return the amount of bytes read. If it return `0`, it means that you reach the end of the resource.

If something goes wrong, `read(2)` will return a negative value (usually `-1`) and set `errno` to the appropriate
error code. In some cases, you may be interrupt while waiting for inputs, and in that case the error code will be
`EINTR` and you should resume reading. In case of non-blocking resources, you may also get `EAGAIN` indicating that
no data where available.

In our demo, if the error is not `EINTR`, we _die_ using `err(3)` that will print the name of the program, your
message (using `printf` like format string) and the message corresponding to `errno` value.

You can try this function with the standard input (`STDIN_FILENO`), like in the following example:

```c++
int main()
{
    std::cout << "Reading from stdin ...\n";
    auto output = demo(STDIN_FILENO);
    std::cout << "Content:\n";
    std::cout << output << std::endl;
}
```

You will need to include the missing header to get a working program:

```c++
#include <cerrno>
#include <err.h>
#include <iostream>
#include <string>
#include <unistd.h>
```

In a terminal, the _end of file_ is reached when the user press `^D`.

### An example using `write(2)`

`write(2)` is very similar to read in it usage, I let you read the code sample:

```c++
#include <cerrno>
#include <err.h>
#include <iostream>
#include <string>
#include <unistd.h>

int main()
{
    constexpr char msg[] = "write(2) example\n";
    constexpr int  len   = sizeof(msg);

    int written = 0;
    do {
        // write to stdout the content of msg shifted by the amount already written
        auto w = write(STDOUT_FILENO, msg + written, len - written);
        if (w < 0) {
            // Interrupt ?
            if (errno == EINTR) {
                continue;
            }
            // else die
            err(1, "error while writing");
        }
        written += w;
    } while (written < len);
}
```

## Files

Files manipulations are very common in system programming. We may distinguish operations on the content of the files
(like reading or writing) and operations on the files themselves and the filesystem in general (listing content of
directories, copying files, getting informations ... )

Reading and writing content belongs to input/output operations (I/Os) and are often generic with regards to their
targets (it could be a file, but also a terminal, a pipe, a network socket). In POSIX systems, the underlying
concept used to manipulate the content of files are called **file descriptors**. Languages and their standard
library may provide some abstractions above it, but that's what you have behind.

### File descriptors

A file descriptor is an integer that will serve as a handle for an I/O resource. You can think of it as an index to
an array of resources managed by your system. Any time you want to perform an operation on that resource, you will
call a syscall with at least the file descriptor as a parameter.

For example, to close an already opened resource `fd`, you will call `close(fd)`, to read some bytes you will call
`read(fd, buff, amount)` and so on. All I/O resources can be used with at least `close(2)`, 'read(2)` or
`write(2)`. Some resources may support other operations depending of their kind.

At startup, your program will have, at least, three file descriptor already opened:
* `STDIN_FILENO` : the standard input
* `STDOUT_FILENO`: the standard output
* `STDERR_FILENO`: the standard error output

Those descriptors are usually mapped to the terminal but can be redirected to some other resources by the parent
process or the process itself.

Since file descriptors are not limited to files, you can obtain one from several syscalls, but the simplest way to
get a file descriptor is to open a file, using `open(2)`.

