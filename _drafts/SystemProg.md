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
shell for an example.

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

Let's write a first demonstration, the following example is using some classical syscalls (`open(2)` and
`close(2)`) and some mechanism to handle errors.

```c++
/*
 * Simple syscall example: open a file
 */

#include <cerrno>
#include <err.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        // No args, leave with an error message
        errx(1, "usage: %s <file>", argv[0]);
    }
    // Try to open the file
    if (int fd = open(argv[1], O_RDONLY); fd < 0) {
        // negative value means error, let err(3) display an appropriate error message
        err(1, "error while opening %s", argv[1]);
    } else {
        // OK, file is opened, we have nothing else to do
        std::cout << "We have opened " << argv[1] << std::endl;
        // Let's properly close it
        close(fd);
    }
    return 0; // not mandatory
}
```

So, we have some includes, `fcntl.h` is for `open(2)`, `err.h` provides error printing functions like `err(3)` and
`unistd.h` provides `close(2)`. The functions `err(3)` and `errx(3)` will quit the current program with an error
message (using printf-like formats) completed (for `err`) with a message corresponding to the current value of
`errno`.

In the initializing statement of the second `if`, we are calling the syscall `open(2)` to open the file name passed
as first argument. `O_RDONLY` is the opening mode, in this case read-only, it's an integer obtained by combining
flags with bit-or (`|`). If the value returned is negative, we have an error and `errno` is set accordingly.
Otherwise, the file has been opened successfully, and for this example we just close it using `close(2)`.

Most syscalls work like `open(2)`, they return a negative value (usually `-1`) to indicate an error. In fact, we
could have check the return value of `close(2)` too.

Let build it and run it:

```
shell> clang++ -Wall -Wextra -std=c++17 -o open_file open_file.cc
shell> ./open_file
open_file: usage: ./open_file <file>
shell> ./open_file open_file.cc
We have opened open_file.cc
shell> ./open_file foo
open_file: error while opening foo: No such file or directory
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

The spirit of `read(2)` and `write(2)` are central to the way we interact with files. They both take a file
descriptor, a buffer and an amount of data, and they both return an integer indicating the amount of bytes read or
write, or a negative value in case of error.

`read(2)` will always read what is actually available and return as soon as it can. Thus it may read less bytes
than requested and block if nothing is available while the resource is still opened. When reaching the _end of
file_ it returns `0`. The _end of file_ is sent when the reading cursor of the file reaches the end, or when a
resource is closed.

The next example will read from the standard input, until the user type `^D` (or the intput is finished somehow).
We will read byte by byte for the example and directly print it back. In a terminal, the user input is only sent
when the user enters a newline, so it will looks like we are reading the full line but in fact it will always be
byte by byte.

```c++
/*
 * read_stdin.cc
 */

#include <cerrno>
#include <err.h>
#include <iostream>
#include <string_view>
#include <unistd.h>

int main()
{
    int          r;
    const size_t amount = 1;
    char         buff[amount];

    while ((r = read(STDIN_FILENO, buff, amount)) != 0) {
        if (r < 0) {
            // We may have been interrupt, the standard tells us to "retry"
            if (errno == EINTR) {
                continue;
            }
            err(1, "error while reading");
        }
        std::cout << std::string_view(buff, r);
    }
}
```

You can mix this example with the previous to read from a file by replacing `STDIN_FILENO` by the variable `fd`.`
